# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import logging
import subprocess
import sys
from pathlib import Path

import pytest
from test_build_system_helpers import EnvDict
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import file_contains
from test_build_system_helpers import normalize_output
from test_build_system_helpers import replace_in_file

# Setting CMAKE_SYSTEM_NAME makes CMake treat the build as cross-compiling, while
# still using the host compiler, so no cross toolchain is needed for the test.
CUSTOM_TOOLCHAIN_FILE_CONTENT = """
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_HOST_SYSTEM_PROCESSOR})
"""

pytestmark = pytest.mark.skipif(sys.platform != 'linux', reason='Cross-compiling for linux is tested on Linux hosts')


def write_toolchain_file(test_app_copy: Path) -> Path:
    toolchain_file = test_app_copy / 'custom-toolchain.cmake'
    toolchain_file.write_text(CUSTOM_TOOLCHAIN_FILE_CONTENT)
    return toolchain_file


def cross_compiling() -> bool:
    return bool(json.loads(Path('build/project_description.json').read_text())['cross_compiling'])


def test_linux_default_toolchain_file(idf_py: IdfPyFunc, test_app_copy: Path, tmp_path: Path) -> None:
    logging.info('Linux target uses the host toolchain by default')
    idf_py('--preview', '-DIDF_TARGET=linux', 'reconfigure')
    assert file_contains('build/CMakeCache.txt', 'toolchain-linux.cmake')
    assert not cross_compiling()

    logging.info("Another ESP-IDF checkout's toolchain-linux.cmake is not treated as a custom toolchain file")
    other_toolchain_file = tmp_path / 'esp-idf' / 'tools' / 'cmake' / 'toolchain-linux.cmake'
    other_toolchain_file.parent.mkdir(parents=True)
    other_toolchain_file.write_text('')
    ret = idf_py('--preview', f'-DCMAKE_TOOLCHAIN_FILE={other_toolchain_file}', 'reconfigure')
    assert 'Using custom toolchain file' not in ret.stdout
    assert not cross_compiling()

    logging.info('Changing to a custom toolchain file in an existing build directory is an error')
    toolchain_file = write_toolchain_file(test_app_copy)
    ret = idf_py('--preview', f'-DCMAKE_TOOLCHAIN_FILE={toolchain_file}', 'reconfigure', check=False)
    assert ret.returncode != 0
    assert 'To change the toolchain file, clear the build directory' in normalize_output(ret.stderr)


def test_linux_custom_toolchain_file(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Linux target accepts a custom toolchain file passed with -DCMAKE_TOOLCHAIN_FILE')
    toolchain_file = write_toolchain_file(test_app_copy)
    ret = idf_py('--preview', '-DIDF_TARGET=linux', f'-DCMAKE_TOOLCHAIN_FILE={toolchain_file}', 'reconfigure')
    assert 'Using custom toolchain file' in ret.stdout
    assert file_contains('build/CMakeCache.txt', f'CMAKE_TOOLCHAIN_FILE:FILEPATH={toolchain_file}')
    assert cross_compiling()

    logging.info('Custom toolchain file is kept on reconfigure')
    ret = idf_py('--preview', 'reconfigure')
    assert 'Using custom toolchain file' in ret.stdout
    assert cross_compiling()

    logging.info('Application can be built and run with a custom toolchain file')
    replace_in_file(
        test_app_copy / 'main' / 'build_test_app.c',
        '// placeholder_inside_main',
        'printf("cross build ok\\n"); fflush(stdout); exit(0);',
    )
    replace_in_file(
        test_app_copy / 'main' / 'build_test_app.c', '#include <stdio.h>', '#include <stdio.h>\n#include <stdlib.h>'
    )
    idf_py('--preview', 'build')
    ret = subprocess.run(
        [str(test_app_copy / 'build' / 'build_test_app.elf')], capture_output=True, text=True, timeout=30
    )
    assert ret.returncode == 0
    assert 'cross build ok' in ret.stdout

    logging.info('idf.py monitor refuses to run a cross-compiled application')
    ret = idf_py('--preview', 'monitor', check=False)
    assert ret.returncode != 0
    assert 'cross-compiled' in ret.stderr


def test_linux_custom_toolchain_file_from_env(idf_py: IdfPyFunc, default_idf_env: EnvDict, test_app_copy: Path) -> None:
    logging.info('Linux target accepts a custom toolchain file passed with CMAKE_TOOLCHAIN_FILE environment variable')
    default_idf_env['CMAKE_TOOLCHAIN_FILE'] = str(write_toolchain_file(test_app_copy))
    idf_py('--preview', '-DIDF_TARGET=linux', 'reconfigure')
    assert cross_compiling()


def test_chip_target_rejects_custom_toolchain_file(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Chip targets do not accept a custom toolchain file')
    toolchain_file = write_toolchain_file(test_app_copy)
    ret = idf_py('-DIDF_TARGET=esp32', f'-DCMAKE_TOOLCHAIN_FILE={toolchain_file}', 'reconfigure', check=False)
    assert ret.returncode != 0
    assert normalize_output(
        "CMAKE_TOOLCHAIN_FILE 'custom-toolchain' does not match currently selected IDF_TARGET 'esp32'"
    ) in normalize_output(ret.stderr)
