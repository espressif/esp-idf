# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import os
import sys
import textwrap
from pathlib import Path
from typing import List, Union

import pytest
from test_build_system_helpers import (APP_BINS, BOOTLOADER_BINS, PARTITION_BIN, IdfPyFunc, append_to_file,
                                       file_contains, get_idf_build_env, replace_in_file, run_cmake_and_build)


def assert_built(paths: Union[List[str], List[Path]]) -> None:
    for path in paths:
        assert os.path.exists(path)


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
    assert file_contains(cmake_cache_file, 'CMAKE_GENERATOR:INTERNAL=Ninja')
    assert_built(BOOTLOADER_BINS + APP_BINS + PARTITION_BIN)


@pytest.mark.skipif(sys.platform == 'win32', reason="GNU Make doesn't work on Windows")
@pytest.mark.usefixtures('test_app_copy')
def test_build_with_generator_makefile(idf_py: IdfPyFunc) -> None:
    logging.info('idf.py can build with Unix Makefiles')
    idf_py('-G', 'Unix Makefiles', 'build')
    cmake_cache_file = Path('build', 'CMakeCache.txt')
    assert_built([cmake_cache_file])
    assert file_contains(cmake_cache_file, 'CMAKE_GENERATOR:INTERNAL=Unix Makefiles')
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


def test_build_skdconfig_phy_init_data(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('can build with phy_init_data')
    (test_app_copy / 'sdkconfig.defaults').touch()
    (test_app_copy / 'sdkconfig.defaults').write_text('CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION=y')
    idf_py('reconfigure')
    idf_py('build')
    assert_built(BOOTLOADER_BINS + APP_BINS + PARTITION_BIN + ['build/phy_init_data.bin'])


def test_build_compiler_flag_in_source_file(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Compiler flags on build command line are taken into account')
    compiler_flag = textwrap.dedent("""
                                    #ifndef USER_FLAG
                                    #error "USER_FLAG is not defined!"
                                    #endif
                                    """)
    append_to_file((test_app_copy / 'main' / 'build_test_app.c'), compiler_flag)
    idf_py('build', '-DCMAKE_C_FLAGS=-DUSER_FLAG')


@pytest.mark.usefixtures('test_app_copy')
def test_build_compiler_flags_no_overwriting(idf_py: IdfPyFunc) -> None:
    logging.info('Compiler flags cannot be overwritten')
    # If the compiler flags are overriden, the following build command will
    # cause issues at link time.
    idf_py('build', '-DCMAKE_C_FLAGS=', '-DCMAKE_CXX_FLAGS=')


def test_build_with_sdkconfig_build_abspath(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    build_path = test_app_copy / 'build_tmp'
    sdkconfig_path = build_path / 'sdkconfig'
    idf_py('-D', f'SDKCONFIG={sdkconfig_path}', '-B', str(build_path), 'build')


def test_build_cmake_executable_suffix(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('idf.py can build with CMAKE_EXECUTABLE_SUFFIX set')
    append_to_file((test_app_copy / 'CMakeLists.txt'), 'set(CMAKE_EXECUTABLE_SUFFIX_CXX ".ext")')
    ret = idf_py('build')
    assert 'Project build complete' in ret.stdout, 'Build with CMAKE_EXECUTABLE_SUFFIX set failed'
