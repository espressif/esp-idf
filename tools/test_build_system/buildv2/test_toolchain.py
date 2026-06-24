# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import shutil
from pathlib import Path

import pytest
from test_build_system_helpers import EnvDict
from test_build_system_helpers import file_contains
from test_build_system_helpers import run_idf_py


@pytest.mark.usefixtures('test_app_copy')
def test_idf_toolchain_gcc_propagation(default_idf_env: EnvDict) -> None:
    """IDF_TOOLCHAIN=gcc is propagated to the CMake cache and to sdkconfig."""
    logging.info('Testing IDF_TOOLCHAIN=gcc propagation')

    default_idf_env.pop('IDF_TOOLCHAIN', None)
    run_idf_py('reconfigure', env=default_idf_env)

    assert file_contains('build/CMakeCache.txt', 'IDF_TOOLCHAIN:STRING=gcc'), (
        'IDF_TOOLCHAIN CMake cache variable must be "gcc" when building with the default toolchain'
    )

    sdkconfig_h = Path('build/config/sdkconfig.h').read_text()
    assert '#define CONFIG_IDF_TOOLCHAIN "gcc"' in sdkconfig_h, (
        'CONFIG_IDF_TOOLCHAIN must be "gcc" in sdkconfig.h for the default toolchain'
    )
    assert '#define CONFIG_IDF_TOOLCHAIN_GCC 1' in sdkconfig_h, (
        'CONFIG_IDF_TOOLCHAIN_GCC must be set in sdkconfig.h for the default toolchain'
    )


@pytest.mark.usefixtures('test_app_copy')
def test_idf_toolchain_clang_propagation(default_idf_env: EnvDict) -> None:
    """IDF_TOOLCHAIN=clang is propagated to the CMake cache and to sdkconfig,
    and selects a libc implementation that is compatible with clang."""
    clang_path = shutil.which('clang')
    if clang_path is None or 'esp-clang' not in str(Path(clang_path).resolve()):
        pytest.skip('Espressif clang (esp-clang) not available in PATH')

    logging.info('Testing IDF_TOOLCHAIN=clang propagation')

    default_idf_env['IDF_TOOLCHAIN'] = 'clang'
    run_idf_py('reconfigure', env=default_idf_env)

    assert file_contains('build/CMakeCache.txt', 'IDF_TOOLCHAIN:STRING=clang'), (
        'IDF_TOOLCHAIN CMake cache variable must be "clang" when IDF_TOOLCHAIN=clang is set'
    )

    sdkconfig_h = Path('build/config/sdkconfig.h').read_text()
    assert '#define CONFIG_IDF_TOOLCHAIN "clang"' in sdkconfig_h, (
        'CONFIG_IDF_TOOLCHAIN must be "clang" in sdkconfig.h when IDF_TOOLCHAIN=clang is set'
    )
    assert '#define CONFIG_IDF_TOOLCHAIN_CLANG 1' in sdkconfig_h, (
        'CONFIG_IDF_TOOLCHAIN_CLANG must be set in sdkconfig.h when IDF_TOOLCHAIN=clang is set'
    )
    assert '#define CONFIG_LIBC_NEWLIB 1' in sdkconfig_h, 'CONFIG_LIBC_NEWLIB must be selected for clang builds'
