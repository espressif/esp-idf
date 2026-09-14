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
import subprocess
import sys
import tempfile
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


class _MockMCPServer:
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
def mcp_ext(monkeypatch: pytest.MonkeyPatch, tmp_path: Path) -> tuple[types.ModuleType, _MockMCPServer]:
    """
    Import (or reimport) mcp_ext with all external dependencies mocked.
    Returns the module object plus a _MockMCPServer instance that was used so
    that tests can inspect registered tools/resources.
    """
    # Build a fresh _MockMCPServer for this test
    mock_mcp_instance = _MockMCPServer('ESP-IDF')

    # Stub rich_click
    rich_click = types.ModuleType('rich_click')
    rich_click.Context = object  # type: ignore[attr-defined]

    # Stub esp_pylib modules used by mcp_ext
    esp_pylib_pkg = types.ModuleType('esp_pylib')
    logger_mod = types.ModuleType('esp_pylib.logger')
    serial_ports_mod = types.ModuleType('esp_pylib.serial_ports')

    class _MockLog:
        def note(self, *args: Any, **kwargs: Any) -> None:
            pass

        def err(self, *args: Any, **kwargs: Any) -> None:
            pass

        def set_info_stream(self, *args: Any, **kwargs: Any) -> None:
            pass

    logger_mod.log = _MockLog()  # type: ignore[attr-defined]
    serial_ports_mod.get_port_names = lambda **kwargs: []  # type: ignore[attr-defined]

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
    tools_mod.get_sdkconfig_value = mock.Mock(return_value=None)  # type: ignore[attr-defined]

    idf_py_actions_pkg.errors = errors_mod  # type: ignore[attr-defined]
    idf_py_actions_pkg.tools = tools_mod  # type: ignore[attr-defined]

    # Stub mcp.server — MCPServer constructor returns our mock (mcp >= 2.0 path)
    mcp_pkg = types.ModuleType('mcp')
    mcp_server_pkg = types.ModuleType('mcp.server')
    mcp_server_pkg.MCPServer = lambda name: mock_mcp_instance  # type: ignore[attr-defined]

    # Stub monitor exit codes (same values as esp_idf_monitor.base.constants).
    esp_idf_monitor_pkg = types.ModuleType('esp_idf_monitor')
    esp_idf_monitor_base = types.ModuleType('esp_idf_monitor.base')
    esp_idf_monitor_constants = types.ModuleType('esp_idf_monitor.base.constants')
    esp_idf_monitor_constants.EXIT_EXPECT_TIMEOUT = 110  # type: ignore[attr-defined]
    esp_idf_monitor_constants.EXIT_SCRIPT_ERROR = 2  # type: ignore[attr-defined]

    stubs = {
        'rich_click': rich_click,
        'esp_pylib': esp_pylib_pkg,
        'esp_pylib.logger': logger_mod,
        'esp_pylib.serial_ports': serial_ports_mod,
        'idf_py_actions': idf_py_actions_pkg,
        'idf_py_actions.errors': errors_mod,
        'idf_py_actions.tools': tools_mod,
        'mcp': mcp_pkg,
        'mcp.server': mcp_server_pkg,
        'esp_idf_monitor': esp_idf_monitor_pkg,
        'esp_idf_monitor.base': esp_idf_monitor_base,
        'esp_idf_monitor.base.constants': esp_idf_monitor_constants,
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
    mcp_ext_module: tuple[types.ModuleType, _MockMCPServer],
    mock_mcp_instance: _MockMCPServer,
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
    def test_valid_project(self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer]) -> None:
        mod, _ = mcp_ext
        proj = _make_valid_project(tmp_path / 'my_proj')
        assert mod._is_valid_project_dir(str(proj)) is True

    def test_missing_directory(self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer]) -> None:
        mod, _ = mcp_ext
        assert mod._is_valid_project_dir(str(tmp_path / 'nonexistent')) is False

    def test_directory_without_cmakelists(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer]
    ) -> None:
        mod, _ = mcp_ext
        d = tmp_path / 'no_cmake'
        d.mkdir()
        assert mod._is_valid_project_dir(str(d)) is False

    def test_cmakelists_without_idf_line(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer]
    ) -> None:
        mod, _ = mcp_ext
        proj = _make_invalid_project(tmp_path / 'plain')
        assert mod._is_valid_project_dir(str(proj)) is False

    def test_cmakelists_with_spaces_in_include(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer]
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
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer]
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

    def test_empty_string(self, mcp_ext: tuple[types.ModuleType, _MockMCPServer]) -> None:
        mod, _ = mcp_ext
        assert mod._is_valid_project_dir('') is False


# ---------------------------------------------------------------------------
# Tests: resolve_default_project_dir
# ---------------------------------------------------------------------------


class TestResolveDefaultProjectDir:
    def test_env_var_takes_priority_over_default(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, _ = mcp_ext
        env_proj = _make_valid_project(tmp_path / 'env_proj')
        default = _make_valid_project(tmp_path / 'default')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', str(env_proj))
        result = mod.resolve_default_project_dir(str(default))
        assert result == str(env_proj)

    def test_default_used_when_env_not_set(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, _ = mcp_ext
        default = _make_valid_project(tmp_path / 'default')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        result = mod.resolve_default_project_dir(str(default))
        assert result == str(default)

    def test_returns_none_when_nothing_valid(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
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
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'bad')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        tools, _ = _start_server(mcp_ext, mock_mcp, str(invalid))
        result = tools['build_project'](project_dir=None)
        assert 'No valid ESP-IDF project directory found' in result

    def test_explicit_invalid_dir_returns_error_not_fallback(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
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
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
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
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
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
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'bad')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        tools, _ = _start_server(mcp_ext, mock_mcp, str(invalid))
        result = tools['set_target']('esp32c6')
        assert 'No valid ESP-IDF project directory found' in result

    def test_explicit_dir_sets_target(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
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
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'bad')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        tools, _ = _start_server(mcp_ext, mock_mcp, str(invalid))
        result = tools['flash_project']()
        assert 'No valid ESP-IDF project directory found' in result

    def test_port_and_dir_forwarded(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
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
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
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
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
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
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        tools, _ = _start_server(mcp_ext, mock_mcp, str(tmp_path))

        result = tools['create_project']('my_app', path=str(tmp_path / 'nonexistent'))
        assert 'does not exist' in result

    def test_returns_error_on_idf_failure(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
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
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'bad')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        tools, _ = _start_server(mcp_ext, mock_mcp, str(invalid))
        result = tools['clean_project']()
        assert 'No valid ESP-IDF project directory found' in result

    def test_explicit_dir_cleans(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
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
# Tests: monitor script building
# ---------------------------------------------------------------------------


class TestBuildMonitorScript:
    """Covers only what the MCP server itself is responsible for: framing the
    agent's command body (exit/comments) and computing the effective
    timeout. Validity of esp-idf-monitor's own 'expect --timeout' syntax is
    esp-idf-monitor's job, not duplicated here."""

    def test_commands_without_an_actual_command_are_rejected(
        self, mcp_ext: tuple[types.ModuleType, _MockMCPServer]
    ) -> None:
        mod, _ = mcp_ext
        with pytest.raises(ValueError, match='No monitor commands given'):
            mod.assemble_monitor_script_from_agent_commands('   \n# just a note\n')

    def test_bare_expect_gets_default_timeout_and_framing(
        self, mcp_ext: tuple[types.ModuleType, _MockMCPServer]
    ) -> None:
        mod, _ = mcp_ext
        script, effective = mod.assemble_monitor_script_from_agent_commands('expect Hello world!')
        assert script == 'expect --timeout 20 Hello world!\nexit\n'
        assert effective == 20.0

    @pytest.mark.parametrize(
        'commands, timeout_sec, expected_effective, expected_snippet',
        [
            ('expect --timeout 45 ALL TESTS PASSED', 20.0, 45.0, 'expect --timeout 45 ALL TESTS PASSED'),
            (
                'expect --timeout 5 first\nexpect --timeout 90 second\nexpect third',
                30.0,
                125.0,
                'expect --timeout 30 third',
            ),
            ('sleep 120\nexpect ready\nsleep 30', 20.0, 170.0, 'expect --timeout 20 ready'),
            ('expect --timeout abc pattern', 20.0, 20.0, 'expect --timeout abc pattern'),
            ('send hello', 20.0, 20.0, 'send hello'),
        ],
        ids=[
            'keeps_explicit_timeout',
            'expect_timeouts_are_summed_and_default_is_injected',
            'sleeps_are_added_to_the_sum',
            'malformed_timeout_left_to_the_monitor',
            'send_only_uses_timeout_sec_as_kill_bound',
        ],
    )
    def test_timeout_handling(
        self,
        mcp_ext: tuple[types.ModuleType, _MockMCPServer],
        commands: str,
        timeout_sec: float,
        expected_effective: float,
        expected_snippet: str,
    ) -> None:
        mod, _ = mcp_ext
        script, effective = mod.assemble_monitor_script_from_agent_commands(commands, timeout_sec=timeout_sec)
        assert expected_snippet in script
        assert effective == expected_effective

    def test_script_at_max_duration_is_accepted(self, mcp_ext: tuple[types.ModuleType, _MockMCPServer]) -> None:
        mod, _ = mcp_ext
        _, effective = mod.assemble_monitor_script_from_agent_commands(
            'sleep 580\nexpect --timeout 20 ready', timeout_sec=20.0
        )
        assert effective == 600.0

    @pytest.mark.parametrize(
        'commands, timeout_sec',
        [
            ('sleep 99999\nexpect ready', 20.0),
            ('expect --timeout 601 ready', 20.0),
            ('expect --timeout 400 a\nexpect --timeout 250 b', 20.0),
        ],
        ids=['long_sleep', 'long_expect', 'sum_of_expects'],
    )
    def test_script_over_max_duration_is_rejected(
        self, mcp_ext: tuple[types.ModuleType, _MockMCPServer], commands: str, timeout_sec: float
    ) -> None:
        mod, _ = mcp_ext
        with pytest.raises(ValueError, match='The limit is'):
            mod.assemble_monitor_script_from_agent_commands(commands, timeout_sec=timeout_sec)

    @pytest.mark.parametrize(
        'commands, expected_script',
        [
            ('expect uptime', 'expect --timeout 20 uptime\nexit\n'),
            ('reset\nexpect done\nexit\n', 'reset\nexpect --timeout 20 done\nexit\n'),
            (
                'expect first\nreset\nexpect second',
                'expect --timeout 20 first\nreset\nexpect --timeout 20 second\nexit\n',
            ),
        ],
        ids=['exit_appended_and_reset_not_prepended', 'agent_reset_and_exit_kept', 'mid_script_reset_kept'],
    )
    def test_script_framing(
        self, mcp_ext: tuple[types.ModuleType, _MockMCPServer], commands: str, expected_script: str
    ) -> None:
        mod, _ = mcp_ext
        script, _ = mod.assemble_monitor_script_from_agent_commands(commands)
        assert script == expected_script

    @pytest.mark.parametrize('command', ['flash', 'app-flash', 'log', 'output'])
    def test_disallowed_commands_are_rejected(
        self, mcp_ext: tuple[types.ModuleType, _MockMCPServer], command: str
    ) -> None:
        mod, _ = mcp_ext
        with pytest.raises(ValueError, match=rf'Unsupported monitor command {command!r}'):
            mod.assemble_monitor_script_from_agent_commands(f'{command}\nexpect ready')


# ---------------------------------------------------------------------------
# Tests: monitor_device tool
# ---------------------------------------------------------------------------


@pytest.fixture()
def monitor_tools(
    mcp_ext: tuple[types.ModuleType, _MockMCPServer],
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> dict[str, Callable[..., Any]]:
    """Registered tools with monitor logs redirected into tmp_path."""
    _, mock_mcp = mcp_ext
    monkeypatch.setattr(tempfile, 'tempdir', str(tmp_path))
    tools, _ = _start_server(mcp_ext, mock_mcp, str(tmp_path))
    return tools


def _log_path_from(result: str) -> Path:
    """Extract the log file path the tool reported."""
    for line in result.splitlines():
        if line.startswith('Log file: '):
            return Path(line.split(': ', 1)[1])
    raise AssertionError(f'no serial log path in result:\n{result}')


class TestMonitorDevice:
    """Covers what the MCP tool itself is responsible for: turning a script and
    exit code into a status message, wiring subprocess/log-file plumbing, and
    forwarding its own arguments. Behaviour of esp-idf-monitor's command-stream
    mode (e.g. what makes an 'expect --timeout' line valid) is exercised
    by esp-idf-monitor's own tests, not duplicated here."""

    @pytest.mark.parametrize(
        'commands, timeout_sec, expected_snippet',
        [
            ('   ', 20.0, 'No monitor commands given'),
            ('expect ready', 0, 'timeout_sec must be a finite number greater than 0'),
            ('flash\nexpect ready', 20.0, "Unsupported monitor command 'flash'"),
        ],
        ids=['empty_commands', 'invalid_timeout_sec', 'disallowed_command'],
    )
    def test_guard_clauses_return_error_without_running(
        self, monitor_tools: dict[str, Callable[..., Any]], commands: str, timeout_sec: float, expected_snippet: str
    ) -> None:
        with mock.patch('subprocess.run') as mock_run:
            result = monitor_tools['monitor_device'](commands=commands, timeout_sec=timeout_sec)

        assert expected_snippet in result
        mock_run.assert_not_called()

    @pytest.mark.parametrize(
        'returncode, output, expected_status',
        [
            (0, 'Hello world!\n', 'completed successfully (exit code 0)'),
            (110, "boot\nExpect pattern 'nope' timed out after 20.0s", 'not seen before its --timeout elapsed'),
            (2, 'Invalid expect timeout value: (must be a finite number > 0)', 'rejected the script'),
            (1, 'could not open port', 'exited with code 1'),
        ],
        ids=['success', 'expect_timeout', 'script_error', 'unexpected_code'],
    )
    def test_status_message_and_log_reflect_exit_code(
        self,
        monitor_tools: dict[str, Callable[..., Any]],
        returncode: int,
        output: str,
        expected_status: str,
    ) -> None:
        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=returncode, stdout=output, stderr=None)
            result = monitor_tools['monitor_device'](commands='expect ready')

        assert expected_status in result
        if returncode != 0:
            # Why the monitor stopped is its last output, so the tail carries it.
            assert output in result
        log_file = _log_path_from(result)
        assert log_file.is_absolute()
        assert log_file.read_text(encoding='utf-8') == output

    @pytest.mark.parametrize(
        'call_kwargs, expected_in_cmd, expected_not_in_cmd',
        [
            ({'port': '/dev/ttyUSB0'}, ['-p', '/dev/ttyUSB0'], []),
            ({'no_reset': True}, ['--no-reset'], []),
        ],
        ids=['port_forwarded', 'no_reset_passed_to_the_monitor'],
    )
    def test_cli_arguments_are_forwarded(
        self,
        monitor_tools: dict[str, Callable[..., Any]],
        call_kwargs: dict[str, Any],
        expected_in_cmd: list[str],
        expected_not_in_cmd: list[str],
    ) -> None:
        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=0, stdout='', stderr='')
            monitor_tools['monitor_device'](commands='expect ready', **call_kwargs)

        cmd = mock_run.call_args[0][0]
        assert cmd[:3] == [sys.executable, '-m', 'esp_idf_monitor']
        for item in expected_in_cmd:
            assert item in cmd
        for item in expected_not_in_cmd:
            assert item not in cmd

    def test_hard_timeout_is_twice_the_effective_timeout(self, monitor_tools: dict[str, Callable[..., Any]]) -> None:
        """The effective-timeout math itself (sum of waits plus sleeps) is covered
        by TestBuildMonitorScript; this only checks the tool wires 2x it in."""
        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=0, stdout='', stderr='')
            monitor_tools['monitor_device'](commands='sleep 60\nexpect --timeout 20 ready')

        assert mock_run.call_args[1]['timeout'] == 2 * 80.0

    def test_process_timeout_keeps_partial_output(self, monitor_tools: dict[str, Callable[..., Any]]) -> None:
        with mock.patch('subprocess.run') as mock_run:
            mock_run.side_effect = subprocess.TimeoutExpired(
                cmd=['python', '-m', 'esp_idf_monitor'],
                timeout=60.0,
                output=b'partial serial output\nExpect still waiting\n',
            )
            result = monitor_tools['monitor_device'](commands='expect ready')

        assert 'was killed after 40 seconds' in result
        assert 'Expect still waiting' in result
        logged = _log_path_from(result).read_text(encoding='utf-8')
        assert logged == 'partial serial output\nExpect still waiting\n'

    def test_launch_failure_is_reported(self, monitor_tools: dict[str, Callable[..., Any]]) -> None:
        with mock.patch('subprocess.run') as mock_run:
            mock_run.side_effect = OSError('no such interpreter')
            result = monitor_tools['monitor_device'](commands='expect ready')

        assert 'Failed to run the monitor' in result
        assert 'no such interpreter' in result

    def test_log_write_failure_falls_back_to_inline_tail(
        self,
        monitor_tools: dict[str, Callable[..., Any]],
        monkeypatch: pytest.MonkeyPatch,
    ) -> None:
        monkeypatch.setattr(Path, 'mkdir', mock.Mock(side_effect=OSError('disk full')))

        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=0, stdout='important tail\n', stderr='')
            result = monitor_tools['monitor_device'](commands='expect ready')

        assert 'Log file could not be written (disk full)' in result
        assert 'important tail' in result

    def test_long_output_is_not_inlined(self, monitor_tools: dict[str, Callable[..., Any]]) -> None:
        """The whole point of the log file: a huge serial dump must not come back
        through the tool result."""
        big_output = 'x' * 200000
        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=0, stdout=big_output, stderr='')
            result = monitor_tools['monitor_device'](commands='expect ready')

        assert len(result) < 500
        assert _log_path_from(result).read_text(encoding='utf-8') == big_output

    def test_both_streams_are_captured_through_one_pipe(self, monitor_tools: dict[str, Callable[..., Any]]) -> None:
        """esp-idf-monitor prints decoded panic backtraces on stderr, so a log
        built from stdout alone would lose exactly what the ELF files enable."""
        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=0, stdout='', stderr=None)
            monitor_tools['monitor_device'](commands='expect ready')

        assert mock_run.call_args[1]['stderr'] == subprocess.STDOUT

    def test_no_baud_elf_or_metadata_when_description_absent(
        self, monitor_tools: dict[str, Callable[..., Any]]
    ) -> None:
        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=0, stdout='', stderr='')
            monitor_tools['monitor_device'](commands='expect ready')

        cmd = mock_run.call_args[0][0]
        assert '-b' not in cmd
        assert '--toolchain-prefix' not in cmd
        assert '--target' not in cmd
        assert '--revision' not in cmd
        assert '--decode-coredumps' not in cmd
        assert '--decode-panic' not in cmd
        assert not any(str(arg).endswith('.elf') for arg in cmd)

    def test_forwards_baud_elf_order_and_metadata_when_description_has_elfs(
        self,
        mcp_ext: tuple[types.ModuleType, _MockMCPServer],
        tmp_path: Path,
        monkeypatch: pytest.MonkeyPatch,
    ) -> None:
        mod, mock_mcp = mcp_ext
        monkeypatch.setattr(tempfile, 'tempdir', str(tmp_path))
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        proj = _make_valid_project(tmp_path / 'proj')
        build_dir = proj / 'build'
        boot_dir = build_dir / 'bootloader'
        boot_dir.mkdir(parents=True)
        app_elf = build_dir / 'hello_world.elf'
        boot_elf = boot_dir / 'bootloader.elf'
        app_elf.write_bytes(b'')
        boot_elf.write_bytes(b'')
        (build_dir / 'project_description.json').write_text(
            json.dumps(
                {
                    'app_elf': 'hello_world.elf',
                    'monitor_baud': '115200',
                    'monitor_toolprefix': 'xtensa-esp32-elf-',
                    'target': 'esp32',
                    'min_rev': '3',
                    'config_file': str(proj / 'sdkconfig'),
                }
            ),
            encoding='utf-8',
        )
        monkeypatch.setattr(
            mod,
            'get_sdkconfig_value',
            lambda _path, key: {
                'CONFIG_ESP_COREDUMP_DECODE': 'info',
                'CONFIG_IDF_TARGET_ARCH_RISCV': 'y',
            }.get(key),
        )
        tools, _ = _start_server(mcp_ext, mock_mcp, str(proj))

        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=0, stdout='', stderr='')
            tools['monitor_device'](commands='expect ready')

        cmd = mock_run.call_args[0][0]
        assert cmd[cmd.index('-b') + 1] == '115200'
        elf_args = [arg for arg in cmd if str(arg).endswith('.elf')]
        assert elf_args[0] == str(app_elf)
        assert str(boot_elf) in elf_args
        assert cmd[cmd.index('--toolchain-prefix') + 1] == 'xtensa-esp32-elf-'
        assert cmd[cmd.index('--target') + 1] == 'esp32'
        assert cmd[cmd.index('--revision') + 1] == '3'
        assert cmd[cmd.index('--decode-coredumps') + 1] == 'info'
        assert cmd[cmd.index('--decode-panic') + 1] == 'backtrace'

    def test_explicit_baud_overrides_description_monitor_baud(
        self,
        mcp_ext: tuple[types.ModuleType, _MockMCPServer],
        tmp_path: Path,
        monkeypatch: pytest.MonkeyPatch,
    ) -> None:
        _, mock_mcp = mcp_ext
        monkeypatch.setattr(tempfile, 'tempdir', str(tmp_path))
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        proj = _make_valid_project(tmp_path / 'proj')
        build_dir = proj / 'build'
        build_dir.mkdir()
        (build_dir / 'project_description.json').write_text(
            json.dumps({'monitor_baud': '115200', 'app_elf': 'hello_world.elf'}),
            encoding='utf-8',
        )
        tools, _ = _start_server(mcp_ext, mock_mcp, str(proj))

        with mock.patch('subprocess.run') as mock_run:
            mock_run.return_value = mock.Mock(returncode=0, stdout='', stderr='')
            tools['monitor_device'](commands='expect ready', baud='9600')

        cmd = mock_run.call_args[0][0]
        assert cmd[cmd.index('-b') + 1] == '9600'
        assert '115200' not in cmd


# ---------------------------------------------------------------------------
# Test: server starts without error when project_path is not valid
# ---------------------------------------------------------------------------


class TestServerStartsOutsideProject:
    def test_no_fatal_error_when_project_path_invalid(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
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

        # MCPServer was still initialised
        assert mock_mcp.name == 'ESP-IDF'

    def test_tools_registered_even_when_project_path_invalid(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'not_a_project')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')

        tools, _ = _start_server(mcp_ext, mock_mcp, str(invalid))
        assert 'build_project' in tools
        assert 'set_target' in tools
        assert 'flash_project' in tools
        assert 'clean_project' in tools
        assert 'monitor_device' in tools


# ---------------------------------------------------------------------------
# Tests: resources use resolve_default_project_dir
# ---------------------------------------------------------------------------


class TestGetProjectStatus:
    def test_returns_error_json_when_no_valid_dir(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'bad')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        _, resources = _start_server(mcp_ext, mock_mcp, str(invalid))

        result = json.loads(resources['project://status']())
        assert 'error' in result
        assert 'idf_version' in result

    def test_returns_status_when_valid_project(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
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
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
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
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        invalid = _make_invalid_project(tmp_path / 'bad')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        _, resources = _start_server(mcp_ext, mock_mcp, str(invalid))

        result = json.loads(resources['project://config']())
        assert 'error' in result

    def test_returns_no_build_dir_when_build_missing(
        self, tmp_path: Path, mcp_ext: tuple[types.ModuleType, _MockMCPServer], monkeypatch: pytest.MonkeyPatch
    ) -> None:
        mod, mock_mcp = mcp_ext
        proj = _make_valid_project(tmp_path / 'proj')
        monkeypatch.setenv('IDF_MCP_WORKSPACE_FOLDER', '')
        _, resources = _start_server(mcp_ext, mock_mcp, str(proj))

        # build/ does not exist → build_dir_exists: False
        result = json.loads(resources['project://config']())
        assert result.get('build_dir_exists') is False
