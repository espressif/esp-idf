# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import os
import re
import sys
import typing
from pathlib import Path
from typing import List, Pattern, Union

import pytest
from test_build_system_helpers import (APP_BINS, BOOTLOADER_BINS, PARTITION_BIN, EnvDict, IdfPyFunc, get_idf_build_env,
                                       replace_in_file, run_cmake)


def run_cmake_and_build(*cmake_args: str, env: typing.Optional[EnvDict] = None) -> None:
    """
    Run cmake command with given arguments and build afterwards, raise an exception on failure
    :param cmake_args: arguments to pass cmake
    :param env: environment variables to run the cmake with; if not set, the default environment is used
    """
    run_cmake(*cmake_args, env=env)
    run_cmake('--build', '.')


def assert_built(paths: Union[List[str], List[Path]]) -> None:
    for path in paths:
        assert os.path.exists(path)


def check_file_contains(filename: Union[str, Path], what: Union[str, Pattern]) -> None:
    with open(filename, 'r', encoding='utf-8') as f:
        data = f.read()
        if isinstance(what, str):
            assert what in data
        else:
            assert re.search(what, data) is not None


def test_build_alternative_directories(idf_py: IdfPyFunc, session_work_dir: Path, test_app_copy: Path) -> None:
    logging.info('Moving BUILD_DIR_BASE out of tree')
    alt_build_dir = session_work_dir / 'alt_build'
    idf_py('-B', str(alt_build_dir), 'build')
    assert os.listdir(alt_build_dir) != [], 'No files found in new build directory!'
    default_build_dir = test_app_copy / 'build'
    if default_build_dir.exists():
        assert os.listdir(default_build_dir) == [], f'Some files were incorrectly put into the default build directory: {default_build_dir}'

    logging.info('BUILD_DIR_BASE inside default build directory')
    build_subdir_inside_build_dir = default_build_dir / 'subdirectory'
    idf_py('-B', str(build_subdir_inside_build_dir), 'build')
    assert os.listdir(build_subdir_inside_build_dir) != [], 'No files found in new build directory!'


@pytest.mark.usefixtures('test_app_copy')
def test_build_cmake_ninja() -> None:
    logging.info('Can build with Ninja (no idf.py)')
    run_cmake_and_build('-G', 'Ninja', '..')
    assert_built(BOOTLOADER_BINS + APP_BINS + PARTITION_BIN)


@pytest.mark.skipif(sys.platform == 'win32', reason="GNU Make doesn't work on Windows")
@pytest.mark.usefixtures('test_app_copy')
def test_build_cmake_makefile() -> None:
    logging.info('Can build with GNU Make (no idf.py)')
    run_cmake_and_build('-G', 'Unix Makefiles', '..')
    assert_built(BOOTLOADER_BINS + APP_BINS + PARTITION_BIN)


@pytest.mark.usefixtures('test_app_copy')
def test_build_with_generator_ninja(idf_py: IdfPyFunc) -> None:
    logging.info('idf.py can build with Ninja')
    idf_py('-G', 'Ninja', 'build')
    cmake_cache_file = Path('build', 'CMakeCache.txt')
    assert_built([cmake_cache_file])
    check_file_contains(cmake_cache_file, 'CMAKE_GENERATOR:INTERNAL=Ninja')
    assert_built(BOOTLOADER_BINS + APP_BINS + PARTITION_BIN)


@pytest.mark.skipif(sys.platform == 'win32', reason="GNU Make doesn't work on Windows")
@pytest.mark.usefixtures('test_app_copy')
def test_build_with_generator_makefile(idf_py: IdfPyFunc) -> None:
    logging.info('idf.py can build with Unix Makefiles')
    idf_py('-G', 'Unix Makefiles', 'build')
    cmake_cache_file = Path('build', 'CMakeCache.txt')
    assert_built([cmake_cache_file])
    check_file_contains(cmake_cache_file, 'CMAKE_GENERATOR:INTERNAL=Unix Makefiles')
    assert_built(BOOTLOADER_BINS + APP_BINS + PARTITION_BIN)


def test_build_with_cmake_and_idf_path_unset(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    idf_path = Path(os.environ['IDF_PATH'])
    env = get_idf_build_env(idf_path)
    env.pop('IDF_PATH')

    logging.info('Can build with IDF_PATH set via cmake cache not environment')
    replace_in_file('CMakeLists.txt', 'ENV{IDF_PATH}', '{IDF_PATH}')
    run_cmake_and_build('-G', 'Ninja', '..', '-DIDF_PATH={}'.format(idf_path), env=env)
    assert_built(BOOTLOADER_BINS + APP_BINS + PARTITION_BIN)
    idf_py('fullclean')

    logging.info('Can build with IDF_PATH unset and inferred by cmake when Kconfig needs it to be set')
    # working with already changed CMakeLists.txt
    kconfig_file = test_app_copy / 'main' / 'Kconfig.projbuild'
    kconfig_file.write_text('source "$IDF_PATH/examples/wifi/getting_started/station/main/Kconfig.projbuild"')
    run_cmake_and_build('-G', 'Ninja', '..', '-DIDF_PATH={}'.format(idf_path), env=env)
    assert_built(BOOTLOADER_BINS + APP_BINS + PARTITION_BIN)
    kconfig_file.unlink()  # remove file to not affect following sub-test
    idf_py('fullclean')

    logging.info('Can build with IDF_PATH unset and inferred by build system')
    # replacing {IDF_PATH} not ENV{IDF_PATH} since CMakeLists.txt was already changed in this test
    replace_in_file('CMakeLists.txt', '{IDF_PATH}', '{ci_idf_path}')
    run_cmake_and_build('-G', 'Ninja', '-D', 'ci_idf_path={}'.format(idf_path), '..', env=env)
    assert_built(BOOTLOADER_BINS + APP_BINS + PARTITION_BIN)
