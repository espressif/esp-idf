# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Rebuild tests with esp-idf-configdep enabled (default idf.py behavior).

esp-idf-configdep rewrites compiler .d files to depend on per-option
build/config/.../*.cdep stubs instead of sdkconfig.h. The tests in
test_rebuild.py disable configdep to exercise pure CMake/Ninja incremental
behavior. Here we keep configdep **on** and apply the same coarse-grained
checks as the classic rebuild tests (plus relaxed expectations where configdep
legitimately skips compiling TUs that do not reference changed CONFIG_* symbols).
"""

import logging
import os
from pathlib import Path

import pytest
from test_build_system_helpers import ALL_ARTIFACTS_BUILDV1
from test_build_system_helpers import ALL_ARTIFACTS_BUILDV2
from test_build_system_helpers import APP_BINS
from test_build_system_helpers import BOOTLOADER_BINS
from test_build_system_helpers import BUILD_SNAPSHOT_EXCLUDE
from test_build_system_helpers import PARTITION_BIN
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import get_snapshot
from test_build_system_helpers import rebuild_and_check
from test_build_system_helpers import replace_in_file


@pytest.mark.usefixtures('test_app_copy')
def test_rebuild_no_changes_with_configdep(idf_py: IdfPyFunc, request: pytest.FixtureRequest) -> None:
    """No-op rebuild: second build after a clean first build must not change any artifacts."""
    idf_py('build')

    logging.info('baseline snapshot after settle')
    all_build_files = get_snapshot('build/**/*', exclude_patterns=BUILD_SNAPSHOT_EXCLUDE)

    logging.info('check that all build artifacts were generated')
    all_artifacts = ALL_ARTIFACTS_BUILDV2 if request.config.getoption('buildv2', False) else ALL_ARTIFACTS_BUILDV1
    for artifact in all_artifacts:
        assert Path(artifact).exists()

    logging.info('noop build with configdep; mtimes under baseline should match')
    idf_py('build')
    all_build_files_after = get_snapshot('build/**/*', exclude_patterns=BUILD_SNAPSHOT_EXCLUDE)
    all_build_files_after.assert_same(all_build_files)


@pytest.mark.usefixtures('test_app_copy')
def test_rebuild_source_files_with_configdep(idf_py: IdfPyFunc) -> None:
    """Same selective-rebuild checks as ``test_rebuild_source_files``, with configdep on."""
    idf_path = Path(os.environ['IDF_PATH'])
    idf_py('build')

    logging.info('updating a component source file rebuilds only that component')
    component_files_patterns = [
        'build/esp-idf/esp_system/libesp_system.a',
        'build/esp-idf/esp_system/CMakeFiles/__idf_esp_system.dir/port/cpu_start.c.obj',
    ]
    other_files_patterns = [
        'build/esp-idf/lwip/liblwip.a',
        'build/esp-idf/freertos/libfreertos.a',
        *BOOTLOADER_BINS,
        *PARTITION_BIN,
    ]
    (idf_path / 'components/esp_system/port/cpu_start.c').touch()
    rebuild_and_check(idf_py, component_files_patterns, other_files_patterns)

    logging.info('changing a bootloader source file rebuilds the bootloader')
    (idf_path / 'components/bootloader/subproject/main/bootloader_start.c').touch()
    rebuild_and_check(idf_py, BOOTLOADER_BINS, APP_BINS + PARTITION_BIN)

    logging.info('changing the partitions CSV file rebuilds only the partition table')
    (idf_path / 'components/partition_table/partitions_singleapp.csv').touch()
    rebuild_and_check(idf_py, PARTITION_BIN, APP_BINS + BOOTLOADER_BINS)

    logging.info(
        'sdkconfig update: binaries and generated headers must refresh (objects are selective under configdep)'
    )
    sdkconfig_files = ['build/config/sdkconfig.h', 'build/config/sdkconfig.json']
    replace_in_file('sdkconfig', '# CONFIG_FREERTOS_UNICORE is not set', 'CONFIG_FREERTOS_UNICORE=y')
    rebuild_and_check(idf_py, APP_BINS + BOOTLOADER_BINS + sdkconfig_files, PARTITION_BIN)

    logging.info('Updating project CMakeLists.txt triggers app re-link at minimum')
    replace_in_file(
        'CMakeLists.txt',
        '# placeholder_after_include_project_cmake',
        'add_compile_options("-DUSELESS_MACRO_DOES_NOTHING=1")',
    )
    rebuild_and_check(idf_py, APP_BINS, PARTITION_BIN + BOOTLOADER_BINS + sdkconfig_files)
