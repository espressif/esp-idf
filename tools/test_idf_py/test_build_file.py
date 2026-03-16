#!/usr/bin/env python
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import re
import shutil
import subprocess

# Import the module under test
import sys
import tempfile
from collections.abc import Generator

import pytest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'idf_py_actions'))
from build_file_ext import _classify_dependencies
from build_file_ext import _generate_container_project
from build_file_ext import _is_bool_or_number
from build_file_ext import _parse_frontmatter
from build_file_ext import _write_if_changed

IDF_PATH = os.environ.get('IDF_PATH', os.path.join(os.path.dirname(__file__), '..', '..'))
IDF_PY = os.path.join(IDF_PATH, 'tools', 'idf.py')


@pytest.fixture
def tmp_dir() -> Generator[str]:
    d = tempfile.mkdtemp()
    yield d
    shutil.rmtree(d)


class TestParseFrontmatter:
    def _write_c_file(self, tmp_dir: str, content: str) -> str:
        path = os.path.join(tmp_dir, 'test.c')
        with open(path, 'w') as f:
            f.write(content)
        return path

    def test_no_frontmatter(self, tmp_dir: str) -> None:
        path = self._write_c_file(tmp_dir, '#include <stdio.h>\nvoid app_main(void) {}\n')
        assert _parse_frontmatter(path) == {}

    def test_basic_frontmatter(self, tmp_dir: str) -> None:
        path = self._write_c_file(
            tmp_dir,
            """/*
 idf-build-file:
   config:
     - "CONFIG_IDF_TARGET=esp32"
   dependencies:
     - log
     - "espressif/zlib==1.2.3"
 */

#include <stdio.h>
void app_main(void) {}
""",
        )
        result = _parse_frontmatter(path)
        assert result['config'] == ['CONFIG_IDF_TARGET=esp32']
        assert 'log' in result['dependencies']
        assert 'espressif/zlib==1.2.3' in result['dependencies']

    def test_frontmatter_with_extra_comments(self, tmp_dir: str) -> None:
        path = self._write_c_file(
            tmp_dir,
            """/* some other comment */

/*
 idf-build-file:
   config:
     - "CONFIG_SPIRAM=y"
 */

/* another comment */
void app_main(void) {}
""",
        )
        result = _parse_frontmatter(path)
        assert result['config'] == ['CONFIG_SPIRAM=y']

    def test_empty_frontmatter(self, tmp_dir: str) -> None:
        path = self._write_c_file(
            tmp_dir,
            """/*
 idf-build-file:
 */
void app_main(void) {}
""",
        )
        result = _parse_frontmatter(path)
        assert result == {}


class TestClassifyDependencies:
    def test_builtin_only(self) -> None:
        builtin, managed = _classify_dependencies(['log', 'vfs', 'driver'])
        assert builtin == ['log', 'vfs', 'driver']
        assert managed == {}

    def test_managed_only(self) -> None:
        builtin, managed = _classify_dependencies(['espressif/zlib==1.2.3', 'espressif/led_strip>=2.0'])
        assert builtin == []
        assert managed == {
            'espressif/zlib': {'version': '1.2.3'},
            'espressif/led_strip': {'version': '>=2.0'},
        }

    def test_mixed(self) -> None:
        builtin, managed = _classify_dependencies(['log', 'espressif/zlib==1.2.3', 'vfs'])
        assert builtin == ['log', 'vfs']
        assert 'espressif/zlib' in managed

    def test_empty(self) -> None:
        builtin, managed = _classify_dependencies([])
        assert builtin == []
        assert managed == {}

    def test_path_based_dependency(self) -> None:
        deps = [
            'log',
            {'protocol_examples_common': {'path': '${IDF_PATH}/examples/common_components/protocol_examples_common'}},
        ]
        builtin, managed = _classify_dependencies(deps)
        assert builtin == ['log']
        assert 'protocol_examples_common' in managed
        assert (
            managed['protocol_examples_common']['path']
            == '${IDF_PATH}/examples/common_components/protocol_examples_common'
        )


class TestIsBoolOrNumber:
    def test_booleans(self) -> None:
        assert _is_bool_or_number('y') is True
        assert _is_bool_or_number('n') is True

    def test_numbers(self) -> None:
        assert _is_bool_or_number('42') is True
        assert _is_bool_or_number('0x1000') is True
        assert _is_bool_or_number('0') is True

    def test_strings(self) -> None:
        assert _is_bool_or_number('esp32') is False
        assert _is_bool_or_number('hello') is False

    def test_empty(self) -> None:
        assert _is_bool_or_number('') is True


class TestWriteIfChanged:
    def test_creates_new_file(self, tmp_dir: str) -> None:
        path = os.path.join(tmp_dir, 'test.txt')
        assert _write_if_changed(path, 'hello') is True
        with open(path) as f:
            assert f.read() == 'hello'

    def test_no_write_if_same(self, tmp_dir: str) -> None:
        path = os.path.join(tmp_dir, 'test.txt')
        with open(path, 'w') as f:
            f.write('hello')
        assert _write_if_changed(path, 'hello') is False

    def test_overwrites_if_different(self, tmp_dir: str) -> None:
        path = os.path.join(tmp_dir, 'test.txt')
        with open(path, 'w') as f:
            f.write('hello')
        assert _write_if_changed(path, 'world') is True
        with open(path) as f:
            assert f.read() == 'world'


class TestGenerateContainerProject:
    def test_generates_project_files(self, tmp_dir: str) -> None:
        src = os.path.join(tmp_dir, 'test.c')
        with open(src, 'w') as f:
            f.write('void app_main(void) {}\n')

        container = os.path.join(tmp_dir, 'container')
        _generate_container_project(src, {}, container)

        assert os.path.exists(os.path.join(container, 'CMakeLists.txt'))
        assert os.path.exists(os.path.join(container, 'main', 'CMakeLists.txt'))
        assert os.path.exists(os.path.join(container, 'main', 'test.c'))

        with open(os.path.join(container, 'CMakeLists.txt')) as f:
            cmake_content = f.read()
        assert 'cmakev2/idf.cmake' in cmake_content
        assert 'idf_project_default()' in cmake_content

    def test_generates_sdkconfig_defaults(self, tmp_dir: str) -> None:
        src = os.path.join(tmp_dir, 'test.c')
        with open(src, 'w') as f:
            f.write('void app_main(void) {}\n')

        frontmatter = {'config': ['CONFIG_SPIRAM=y', 'CONFIG_IDF_TARGET=esp32']}
        container = os.path.join(tmp_dir, 'container')
        _generate_container_project(src, frontmatter, container)

        with open(os.path.join(container, 'sdkconfig.defaults')) as f:
            content = f.read()
        assert 'CONFIG_SPIRAM=y' in content
        assert 'CONFIG_IDF_TARGET="esp32"' in content

    def test_generates_priv_requires_for_builtin_deps(self, tmp_dir: str) -> None:
        src = os.path.join(tmp_dir, 'test.c')
        with open(src, 'w') as f:
            f.write('void app_main(void) {}\n')

        frontmatter = {'dependencies': ['log', 'vfs']}
        container = os.path.join(tmp_dir, 'container')
        _generate_container_project(src, frontmatter, container)

        with open(os.path.join(container, 'main', 'CMakeLists.txt')) as f:
            content = f.read()
        assert 'PRIV_REQUIRES log vfs' in content

    def test_generates_component_yml_for_managed_deps(self, tmp_dir: str) -> None:
        src = os.path.join(tmp_dir, 'test.c')
        with open(src, 'w') as f:
            f.write('void app_main(void) {}\n')

        frontmatter = {'dependencies': ['espressif/zlib==1.2.3']}
        container = os.path.join(tmp_dir, 'container')
        _generate_container_project(src, frontmatter, container)

        yml_path = os.path.join(container, 'main', 'idf_component.yml')
        assert os.path.exists(yml_path)
        with open(yml_path) as f:
            content = f.read()
        assert 'espressif/zlib' in content

    def test_generates_component_yml_for_path_deps(self, tmp_dir: str) -> None:
        src = os.path.join(tmp_dir, 'test.c')
        with open(src, 'w') as f:
            f.write('void app_main(void) {}\n')

        frontmatter = {
            'dependencies': [
                {
                    'protocol_examples_common': {
                        'path': '${IDF_PATH}/examples/common_components/protocol_examples_common'
                    }
                },
            ]
        }
        container = os.path.join(tmp_dir, 'container')
        _generate_container_project(src, frontmatter, container)

        yml_path = os.path.join(container, 'main', 'idf_component.yml')
        assert os.path.exists(yml_path)
        with open(yml_path) as f:
            content = f.read()
        assert 'protocol_examples_common' in content

    def test_copies_source_file(self, tmp_dir: str) -> None:
        src = os.path.join(tmp_dir, 'test.c')
        with open(src, 'w') as f:
            f.write('void app_main(void) {}\n')

        container = os.path.join(tmp_dir, 'container')
        _generate_container_project(src, {}, container)

        dest = os.path.join(container, 'main', 'test.c')
        assert os.path.isfile(dest)
        assert not os.path.islink(dest)
        with open(dest) as f:
            assert f.read() == 'void app_main(void) {}\n'


class TestBuildFileEndToEnd:
    """End-to-end tests that invoke idf.py build-file as a subprocess."""

    def _run_build_file(self, src: str, *extra_args: str) -> tuple[subprocess.CompletedProcess, str, str]:
        """Run idf.py build-file on the given source file.

        Returns (completed_process, combined_output, container_dir).
        """
        result = subprocess.run(
            [sys.executable, IDF_PY, 'build-file', src, *extra_args],
            capture_output=True,
            text=True,
            timeout=10 * 60,
        )
        output = result.stdout + result.stderr
        # Strip ANSI color codes so the container dir path can be extracted
        plain_output = re.sub(r'\x1b\[[0-9;]*m', '', output)
        match = re.search(r'Container project: (.+)', plain_output)
        container_dir = match.group(1).strip() if match else ''
        return result, output, container_dir

    @staticmethod
    def _read_sdkconfig(container_dir: str) -> str:
        with open(os.path.join(container_dir, 'sdkconfig')) as f:
            return f.read()

    def test_build_file_with_size(self, tmp_dir: str) -> None:
        """Test that build-file composes with the size command."""
        src = os.path.join(tmp_dir, 'hello.c')
        with open(src, 'w') as f:
            f.write(
                '/*\n'
                ' idf-build-file:\n'
                '   config:\n'
                '     - CONFIG_IDF_TARGET=esp32\n'
                '   dependencies:\n'
                '     - log\n'
                ' */\n'
                '#include "esp_log.h"\n'
                'static const char *TAG = "hello";\n'
                'void app_main(void) { ESP_LOGI(TAG, "Hello"); }\n'
            )

        result = subprocess.run(
            [sys.executable, IDF_PY, 'build-file', src, 'size'],
            capture_output=True,
            text=True,
            timeout=10 * 60,
        )
        output = result.stdout + result.stderr
        assert result.returncode == 0, f'idf.py build-file + size failed:\n{output}'
        # Verify build completed
        assert 'Project build complete' in output
        # Verify size command ran (it prints memory usage)
        assert 'Total image size' in output or 'Used static' in output

    def test_config_change_takes_effect_on_rebuild(self, tmp_dir: str) -> None:
        """Changing an sdkconfig option in the frontmatter must be applied on rebuild."""
        src = os.path.join(tmp_dir, 'hello.c')

        def write_src(extra_config: str = '') -> None:
            with open(src, 'w') as f:
                f.write(
                    '/*\n'
                    ' idf-build-file:\n'
                    '   config:\n'
                    '     - CONFIG_IDF_TARGET=esp32\n'
                    f'{extra_config}'
                    ' */\n'
                    'void app_main(void) {}\n'
                )

        write_src()
        result, output, container_dir = self._run_build_file(src)
        assert result.returncode == 0, f'initial build failed:\n{output}'
        assert container_dir, f'container project dir not found in output:\n{output}'
        assert 'CONFIG_COMPILER_OPTIMIZATION_SIZE=y' not in self._read_sdkconfig(container_dir)

        write_src('     - CONFIG_COMPILER_OPTIMIZATION_SIZE=y\n')
        result, output, container_dir = self._run_build_file(src)
        assert result.returncode == 0, f'rebuild after config change failed:\n{output}'
        assert 'CONFIG_COMPILER_OPTIMIZATION_SIZE=y' in self._read_sdkconfig(container_dir), (
            'config change in frontmatter was not applied on rebuild'
        )

    def test_target_change_takes_effect_on_rebuild(self, tmp_dir: str) -> None:
        """Changing CONFIG_IDF_TARGET in the frontmatter must be applied on rebuild."""
        src = os.path.join(tmp_dir, 'hello.c')

        def write_src(target: str) -> None:
            with open(src, 'w') as f:
                f.write(
                    '/*\n'
                    ' idf-build-file:\n'
                    '   config:\n'
                    f'     - CONFIG_IDF_TARGET={target}\n'
                    ' */\n'
                    'void app_main(void) {}\n'
                )

        write_src('esp32')
        result, output, container_dir = self._run_build_file(src)
        assert result.returncode == 0, f'initial build failed:\n{output}'
        assert container_dir, f'container project dir not found in output:\n{output}'
        assert 'CONFIG_IDF_TARGET="esp32"' in self._read_sdkconfig(container_dir)

        write_src('esp32c3')
        result, output, container_dir = self._run_build_file(src)
        assert result.returncode == 0, f'rebuild after target change failed:\n{output}'
        assert 'CONFIG_IDF_TARGET="esp32c3"' in self._read_sdkconfig(container_dir), (
            'target change in frontmatter was not applied on rebuild'
        )
