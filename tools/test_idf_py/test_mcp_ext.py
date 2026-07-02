# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Unit tests for idf_py_actions/mcp_ext.py.

No full IDF environment is required — all external imports and subprocess
calls are mocked.
"""

import importlib
import importlib.util
import json
import os
import sys
import types
from collections.abc import Callable
from pathlib import Path
from typing import Any
from unittest import mock

import pytest

# ---------------------------------------------------------------------------
# Helpers for creating fake project directories
# ---------------------------------------------------------------------------

IDF_CMAKE_LINE = r'include($ENV{IDF_PATH}/tools/cmake/project.cmake)'


def _make_valid_project(path: Path) -> Path:
    """Write a minimal valid ESP-IDF CMakeLists.txt into *path*."""
    path.mkdir(parents=True, exist_ok=True)
    (path / 'CMakeLists.txt').write_text(
        f'cmake_minimum_required(VERSION 3.16)\n{IDF_CMAKE_LINE}\nproject(hello_world)\n',
        encoding='utf-8',
    )
    return path


def _make_invalid_project(path: Path) -> Path:
    """Write a CMakeLists.txt that does NOT include the IDF line."""
    path.mkdir(parents=True, exist_ok=True)
    (path / 'CMakeLists.txt').write_text(
        'cmake_minimum_required(VERSION 3.16)\nproject(plain_cmake)\n',
        encoding='utf-8',
    )
    return path


# ---------------------------------------------------------------------------
# Fixture: stub out all non-stdlib imports so the module can be loaded
# without an IDF installation.
# ---------------------------------------------------------------------------


class _MockFastMCP:
    """Captures tool/resource registrations so tests can invoke them."""

    def __init__(self, name: str) -> None:
        self.name = name
        self.tools: dict[str, Callable[..., Any]] = {}
        self.resources: dict[str, Callable[..., Any]] = {}

    def tool(self, **kwargs: Any) -> Callable[[Callable[..., Any]], Callable[..., Any]]:
        def decorator(fn: Callable[..., Any]) -> Callable[..., Any]:
            self.tools[fn.__name__] = fn
            return fn

        return decorator

    def resource(self, uri: str) -> Callable[[Callable[..., Any]], Callable[..., Any]]:
        def decorator(fn: Callable[..., Any]) -> Callable[..., Any]:
            self.resources[uri] = fn
            return fn

        return decorator

    def run(self) -> None:
        pass  # don't block in tests


@pytest.fixture()
def mcp_ext(monkeypatch: pytest.MonkeyPatch, tmp_path: Path) -> tuple[types.ModuleType, _MockFastMCP]:
    """
    Import (or reimport) mcp_ext with all external dependencies mocked.
    Returns the module object plus a _MockFastMCP instance that was used so
    that tests can inspect registered tools/resources.
    """
    # Build a fresh _MockFastMCP for this test
    mock_mcp_instance = _MockFastMCP('ESP-IDF')

    # Stub rich_click
    rich_click = types.ModuleType('rich_click')
    rich_click.Context = object  # type: ignore[attr-defined]

    # Stub idf_py_actions hierarchy
    idf_py_actions_pkg = types.ModuleType('idf_py_actions')
    errors_mod = types.ModuleType('idf_py_actions.errors')

    class FatalError(Exception):
        pass

    errors_mod.FatalError = FatalError  # type: ignore[attr-defined]

    tools_mod = types.ModuleType('idf_py_actions.tools')
    tools_mod.PropertyDict = dict  # type: ignore[attr-defined]
    tools_mod.get_target = mock.Mock(return_value='esp32')  # type: ignore[attr-defined]
    tools_mod.idf_version = mock.Mock(return_value='5.4.0')  # type: ignore[attr-defined]

    idf_py_actions_pkg.errors = errors_mod  # type: ignore[attr-defined]
    idf_py_actions_pkg.tools = tools_mod  # type: ignore[attr-defined]

    # Stub mcp.server.fastmcp — FastMCP constructor returns our mock
    mcp_pkg = types.ModuleType('mcp')
    mcp_server_pkg = types.ModuleType('mcp.server')
    fastmcp_mod = types.ModuleType('mcp.server.fastmcp')
    fastmcp_mod.FastMCP = lambda name: mock_mcp_instance  # type: ignore[attr-defined]

    stubs = {
        'rich_click': rich_click,
        'idf_py_actions': idf_py_actions_pkg,
        'idf_py_actions.errors': errors_mod,
        'idf_py_actions.tools': tools_mod,
        'mcp': mcp_pkg,
        'mcp.server': mcp_server_pkg,
        'mcp.server.fastmcp': fastmcp_mod,
    }
    for name, stub_mod in stubs.items():
        monkeypatch.setitem(sys.modules, name, stub_mod)

    # Load mcp_ext directly from its file so that the stub 'idf_py_actions'
    # package (which is not a real package) doesn't prevent import.
    mcp_ext_path = Path(__file__).parent.parent / 'idf_py_actions' / 'mcp_ext.py'
    spec = importlib.util.spec_from_file_location('idf_py_actions.mcp_ext', mcp_ext_path)
    mod = importlib.util.module_from_spec(spec)  # type: ignore[arg-type]
    # Register under both names so cross-references inside the module work
    monkeypatch.setitem(sys.modules, 'idf_py_actions.mcp_ext', mod)
    spec.loader.exec_module(mod)  # type: ignore[union-attr]

    return mod, mock_mcp_instance


# ---------------------------------------------------------------------------
# Helper: call start_mcp_server and return registered tools/resources
# ---------------------------------------------------------------------------


def _start_server(
    mcp_ext_module: tuple[types.ModuleType, _MockFastMCP],
    mock_mcp_instance: _MockFastMCP,
    project_path: str,
) -> tuple[dict[str, Callable[..., Any]], dict[str, Callable[..., Any]]]:
    """
    Call action_extensions / start_mcp_server with *project_path* so that
    tools and resources are registered on *mock_mcp_instance*.
    """
    mod, _ = mcp_ext_module
    ext = mod.action_extensions({}, project_path)
    callback = ext['actions']['mcp-server']['callback']
    # ctx and args are only used inside resources; use simple stubs
    fake_args = {'build_dir': os.path.join(project_path, 'build')}
    callback('mcp-server', ctx=None, args=fake_args)
    return mock_mcp_instance.tools, mock_mcp_instance.resources


# ---------------------------------------------------------------------------
# Tests: _is_valid_project_dir
# ---------------------------------------------------------------------------


class TestIsValidProjectDir:
    def test_valid_project(self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP]) -> None:
        mod, _ = mcp_ext
        proj = _make_valid_project(tmp_path / 'my_proj')
        assert mod._is_valid_project_dir(str(proj)) is True

    def test_missing_directory(self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP]) -> None:
        mod, _ = mcp_ext
        assert mod._is_valid_project_dir(str(tmp_path / 'nonexistent')) is False

    def test_directory_without_cmakelists(self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP]) -> None:
        mod, _ = mcp_ext
        d = tmp_path / 'no_cmake'
        d.mkdir()
        assert mod._is_valid_project_dir(str(d)) is False

    def test_cmakelists_without_idf_line(self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP]) -> None:
        mod, _ = mcp_ext
        proj = _make_invalid_project(tmp_path / 'plain')
        assert mod._is_valid_project_dir(str(proj)) is False

    def test_cmakelists_with_spaces_in_include(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP]
    ) -> None:
        mod, _ = mcp_ext
        path = tmp_path / 'spaced'
        path.mkdir()
        (path / 'CMakeLists.txt').write_text(
            'cmake_minimum_required(VERSION 3.16)\n'
            'include( $ENV{IDF_PATH}/tools/cmake/project.cmake )\n'
            'project(hello_world)\n',
            encoding='utf-8',
        )
        assert mod._is_valid_project_dir(str(path)) is True

    def test_commented_out_include_is_rejected(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP]
    ) -> None:
        mod, _ = mcp_ext
        path = tmp_path / 'commented'
        path.mkdir()
        (path / 'CMakeLists.txt').write_text(
            'cmake_minimum_required(VERSION 3.16)\n'
            '# include($ENV{IDF_PATH}/tools/cmake/project.cmake)\n'
            'project(hello_world)\n',
            encoding='utf-8',
        )
        assert mod._is_valid_project_dir(str(path)) is False

    def test_empty_string(self, mcp_ext: tuple[types.ModuleType, _MockFastMCP]) -> None:
        mod, _ = mcp_ext
        assert mod._is_valid_project_dir('') is False


# ---------------------------------------------------------------------------
# Tests: resolve_default_project_dir
# ---------------------------------------------------------------------------


class TestResolveDefaultProjectDir:
    def test_env_var_takes_priority_over_default(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, _ = mcp_ext
        env_proj = _make_valid_project(tmp_path / 'env_proj')
        default = _make_valid_project(tmp_path / 'default')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', str(env_proj))
        result = mod.resolve_default_project_dir(str(default))
        assert result == str(env_proj)

    def test_default_used_when_env_not_set(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, _ = mcp_ext
        default = _make_valid_project(tmp_path / 'default')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        result = mod.resolve_default_project_dir(str(default))
        assert result == str(default)

    def test_returns_none_when_nothing_valid(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, _ = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'bad')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        result = mod.resolve_default_project_dir(str(invalid))
        assert result is None


# ---------------------------------------------------------------------------
# Tests: tools
# ---------------------------------------------------------------------------


class TestBuildProject:
    def test_returns_error_when_no_valid_dir(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'bad')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        tools, _ = _start_server(mcp_ext, mock_mcp, str(invalid))
        result = tools['build_project'](project_dir=None)
        assert 'No valid ESP-IDF project directory found' in result

    def test_explicit_invalid_dir_returns_error_not_fallback(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        bad = _make_invalid_project(tmp_path / 'bad')
        good_env = _make_valid_project(tmp_path / 'good_env')
        # Even though IDF_MCP_WORKSPACE_FOLDER is a valid project, passing an
        # explicit but invalid project_dir must return an error, not silently
        # fall through to the env var project.
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', str(good_env))
        tools, _ = _start_server(mcp_ext, mock_mcp, str(bad))
        result = tools['build_project'](project_dir=str(bad))
        assert str(bad) in result
        assert 'not a valid' in result

    def test_explicit_dir_builds_in_correct_location(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        proj = _make_valid_project(tmp_path / 'proj')
        invalid = _make_invalid_project(tmp_path / 'bad')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        monkeypatch.setenv('IDF_PATH', str(tmp_path))
        tools, _ = _start_server(mcp_ext, mock_mcp, str(invalid))

        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=0, stderr='')
            result = tools['build_project'](project_dir=str(proj))

        assert result == 'Successfully built project'
        call_args = mock_run.call_args
        cmd = call_args[0][0]
        assert '-C' in cmd
        assert str(proj) in cmd
        assert cmd[cmd.index('-C') + 1] == str(proj)
        # cwd is intentionally not passed — -C is authoritative for idf.py
        assert call_args[1].get('cwd') is None

    def test_build_failure_returns_error(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        proj = _make_valid_project(tmp_path / 'proj')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        monkeypatch.setenv('IDF_PATH', str(tmp_path))
        tools, _ = _start_server(mcp_ext, mock_mcp, str(proj))

        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=1, stderr='cmake error')
            result = tools['build_project']()

        assert 'Build failed' in result
        assert 'cmake error' in result


class TestSetTarget:
    def test_returns_error_when_no_valid_dir(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'bad')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        tools, _ = _start_server(mcp_ext, mock_mcp, str(invalid))
        result = tools['set_target']('esp32c6')
        assert 'No valid ESP-IDF project directory found' in result

    def test_explicit_dir_sets_target(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        proj = _make_valid_project(tmp_path / 'proj')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        monkeypatch.setenv('IDF_PATH', str(tmp_path))
        tools, _ = _start_server(mcp_ext, mock_mcp, str(proj))

        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=0, stderr='')
            result = tools['set_target']('esp32s3', project_dir=str(proj))

        assert result == 'Target set to: esp32s3'
        cmd = mock_run.call_args[0][0]
        assert 'set-target' in cmd
        assert 'esp32s3' in cmd
        assert '-C' in cmd
        assert cmd[cmd.index('-C') + 1] == str(proj)


class TestFlashProject:
    def test_returns_error_when_no_valid_dir(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'bad')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        tools, _ = _start_server(mcp_ext, mock_mcp, str(invalid))
        result = tools['flash_project']()
        assert 'No valid ESP-IDF project directory found' in result

    def test_port_and_dir_forwarded(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        proj = _make_valid_project(tmp_path / 'proj')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        monkeypatch.setenv('IDF_PATH', str(tmp_path))
        tools, _ = _start_server(mcp_ext, mock_mcp, str(proj))

        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=0, stderr='')
            result = tools['flash_project'](port='/dev/ttyUSB0', project_dir=str(proj))

        assert 'Successfully flashed' in result
        assert '/dev/ttyUSB0' in result
        cmd = mock_run.call_args[0][0]
        assert '-p' in cmd
        assert '/dev/ttyUSB0' in cmd
        assert '-C' in cmd
        assert cmd[cmd.index('-C') + 1] == str(proj)


class TestCreateProject:
    def test_creates_project_with_explicit_path(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        monkeypatch.setenv('IDF_PATH', str(tmp_path))
        # Start from a non-project directory — that is the whole point of this tool
        tools, _ = _start_server(mcp_ext, mock_mcp, str(tmp_path))

        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=0, stdout='', stderr='')
            result = tools['create_project']('my_app', path=str(tmp_path))

        assert 'my_app' in result
        assert str(tmp_path) in result
        cmd = mock_run.call_args[0][0]
        assert 'create-project' in cmd
        assert 'my_app' in cmd
        assert '-C' in cmd
        assert cmd[cmd.index('-C') + 1] == str(tmp_path)

    def test_uses_project_path_when_no_path_given(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        monkeypatch.setenv('IDF_PATH', str(tmp_path))
        tools, _ = _start_server(mcp_ext, mock_mcp, str(tmp_path))

        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=0, stdout='', stderr='')
            tools['create_project']('my_app')

        cmd = mock_run.call_args[0][0]
        assert cmd[cmd.index('-C') + 1] == str(tmp_path)

    def test_returns_error_when_parent_dir_missing(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        tools, _ = _start_server(mcp_ext, mock_mcp, str(tmp_path))

        result = tools['create_project']('my_app', path=str(tmp_path / 'nonexistent'))
        assert 'does not exist' in result

    def test_returns_error_on_idf_failure(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        monkeypatch.setenv('IDF_PATH', str(tmp_path))
        tools, _ = _start_server(mcp_ext, mock_mcp, str(tmp_path))

        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=3, stdout='', stderr='directory not empty')
            result = tools['create_project']('my_app', path=str(tmp_path))

        assert 'Failed to create project' in result
        assert 'directory not empty' in result


class TestCleanProject:
    def test_returns_error_when_no_valid_dir(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'bad')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        tools, _ = _start_server(mcp_ext, mock_mcp, str(invalid))
        result = tools['clean_project']()
        assert 'No valid ESP-IDF project directory found' in result

    def test_explicit_dir_cleans(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        proj = _make_valid_project(tmp_path / 'proj')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        monkeypatch.setenv('IDF_PATH', str(tmp_path))
        tools, _ = _start_server(mcp_ext, mock_mcp, str(proj))

        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=0, stderr='')
            result = tools['clean_project'](project_dir=str(proj))

        assert result == 'Project cleaned successfully'
        cmd = mock_run.call_args[0][0]
        assert 'clean' in cmd
        assert '-C' in cmd
        assert cmd[cmd.index('-C') + 1] == str(proj)


# ---------------------------------------------------------------------------
# Test: server starts without error when project_path is not valid
# ---------------------------------------------------------------------------


class TestServerStartsOutsideProject:
    def test_no_fatal_error_when_project_path_invalid(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        """start_mcp_server must not raise when project_path is not a valid project."""
        mod, mock_mcp = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'not_a_project')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')

        ext = mod.action_extensions({}, str(invalid))
        callback = ext['actions']['mcp-server']['callback']
        fake_args = {'build_dir': str(invalid / 'build')}

        # Should not raise
        callback('mcp-server', ctx=None, args=fake_args)

        # FastMCP was still initialised
        assert mock_mcp.name == 'ESP-IDF'

    def test_tools_registered_even_when_project_path_invalid(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'not_a_project')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')

        tools, _ = _start_server(mcp_ext, mock_mcp, str(invalid))
        assert 'build_project' in tools
        assert 'set_target' in tools
        assert 'flash_project' in tools
        assert 'clean_project' in tools


# ---------------------------------------------------------------------------
# Tests: resources use resolve_default_project_dir
# ---------------------------------------------------------------------------


class TestGetProjectStatus:
    def test_returns_error_json_when_no_valid_dir(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'bad')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        _, resources = _start_server(mcp_ext, mock_mcp, str(invalid))

        result = json.loads(resources['project://status']())
        assert 'error' in result
        assert 'idf_version' in result

    def test_returns_status_when_valid_project(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        proj = _make_valid_project(tmp_path / 'proj')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        _, resources = _start_server(mcp_ext, mock_mcp, str(proj))

        result = json.loads(resources['project://status']())
        assert result['project_path'] == str(proj)
        assert result['target'] == 'esp32'
        assert 'error' not in result

    def test_uses_env_var_when_project_path_invalid(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'bad')
        env_proj = _make_valid_project(tmp_path / 'env_proj')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', str(env_proj))
        _, resources = _start_server(mcp_ext, mock_mcp, str(invalid))

        result = json.loads(resources['project://status']())
        assert result['project_path'] == str(env_proj)
        assert 'error' not in result


class TestGetProjectConfig:
    def test_returns_error_json_when_no_valid_dir(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'bad')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        _, resources = _start_server(mcp_ext, mock_mcp, str(invalid))

        result = json.loads(resources['project://config']())
        assert 'error' in result

    def test_returns_no_build_dir_when_build_missing(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockFastMCP], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        proj = _make_valid_project(tmp_path / 'proj')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        _, resources = _start_server(mcp_ext, mock_mcp, str(proj))

        # build/ does not exist → build_dir_exists: False
        result = json.loads(resources['project://config']())
        assert result.get('build_dir_exists') is False
