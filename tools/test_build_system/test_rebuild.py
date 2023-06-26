# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
# These tests check whether the build system rebuilds some files or not
# depending on the changes to the project.
import logging
import os
from pathlib import Path
from typing import List, Union

import pytest
from test_build_system_helpers import (ALL_ARTIFACTS, APP_BINS, BOOTLOADER_BINS, PARTITION_BIN, IdfPyFunc,
                                       get_snapshot, replace_in_file)


@pytest.mark.usefixtures('test_app_copy')
def test_rebuild_no_changes(idf_py: IdfPyFunc) -> None:
    logging.info('initial build')
    idf_py('build')
    logging.info('get the first snapshot')
    # excluding the 'log' subdirectory here since it changes after every build
    all_build_files = get_snapshot('build/**/*', exclude_patterns='build/log/*')

    logging.info('check that all build artifacts were generated')
    for artifact in ALL_ARTIFACTS:
        assert Path(artifact).exists()

    logging.info('build again with no changes')
    idf_py('build')
    # if there are no changes, nothing gets rebuilt
    all_build_files_after_rebuild = get_snapshot('build/**/*', exclude_patterns='build/log/*')
    all_build_files_after_rebuild.assert_same(all_build_files)


def rebuild_and_check(idf_py: IdfPyFunc,
                      should_be_rebuilt: Union[str, List[str]],
                      should_not_be_rebuilt: Union[str, List[str]]) -> None:
    """
    Helper function for the test cases below.
    Asserts that the files matching 'should_be_rebuilt' patterns are rebuilt
    and files matching 'should_not_be_rebuilt' patterns aren't, after
    touching (updating the mtime) of the given 'file_to_touch' and rebuilding.
    """
    snapshot_should_be_rebuilt = get_snapshot(should_be_rebuilt)
    snapshot_should_not_be_rebuilt = get_snapshot(should_not_be_rebuilt)
    idf_py('build')
    snapshot_should_be_rebuilt.assert_different(get_snapshot(should_be_rebuilt))
    snapshot_should_not_be_rebuilt.assert_same(get_snapshot(should_not_be_rebuilt))


# For this and the following test function, there are actually multiple logical
# tests in one test function. It would be better to have every check in a separate
# test case, but that would mean doing a full clean build each time. Having a few
# related checks per test function looks like a reasonable compromise.
# If the test function grows too big, try splitting it into two or more functions.
@pytest.mark.usefixtures('test_app_copy')
def test_rebuild_source_files(idf_py: IdfPyFunc) -> None:
    idf_path = Path(os.environ['IDF_PATH'])
    logging.info('initial build')
    idf_py('build')

    logging.info('updating a component source file rebuilds only that component')
    component_files_patterns = [
        'build/esp-idf/esp_system/libesp_system.a',
        'build/esp-idf/esp_system/CMakeFiles/__idf_esp_system.dir/port/cpu_start.c.obj']
    other_files_patterns = [
        'build/esp-idf/lwip/liblwip.a',
        'build/esp-idf/freertos/libfreertos.a',
        *BOOTLOADER_BINS,
        *PARTITION_BIN]
    (idf_path / 'components/esp_system/port/cpu_start.c').touch()
    rebuild_and_check(idf_py,
                      component_files_patterns, other_files_patterns)

    logging.info('changing a bootloader source file rebuilds the bootloader')
    (idf_path / 'components/bootloader/subproject/main/bootloader_start.c').touch()
    rebuild_and_check(idf_py,
                      BOOTLOADER_BINS, APP_BINS + PARTITION_BIN)

    logging.info('changing the partitions CSV file rebuilds only the partition table')
    (idf_path / 'components/partition_table/partitions_singleapp.csv').touch()
    rebuild_and_check(idf_py,
                      PARTITION_BIN, APP_BINS + BOOTLOADER_BINS)

    logging.info('sdkconfig update triggers full recompile')
    # pick on .c, .cpp, .S file which includes sdkconfig.h:
    obj_files = [
        'build/esp-idf/newlib/CMakeFiles/__idf_newlib.dir/newlib_init.c.obj',
        'build/esp-idf/nvs_flash/CMakeFiles/__idf_nvs_flash.dir/src/nvs_api.cpp.obj'
        'build/esp-idf/esp_system/CMakeFiles/__idf_esp_system.dir/port/arch/xtensa/panic_handler_asm.S.obj'
    ]
    sdkconfig_files = [
        'build/config/sdkconfig.h',
        'build/config/sdkconfig.json'
    ]
    replace_in_file('sdkconfig', '# CONFIG_FREERTOS_UNICORE is not set', 'CONFIG_FREERTOS_UNICORE=y')
    rebuild_and_check(idf_py, APP_BINS + BOOTLOADER_BINS + obj_files + sdkconfig_files, PARTITION_BIN)

    logging.info('Updating project CMakeLists.txt triggers app recompile')
    replace_in_file('CMakeLists.txt',
                    '# placeholder_after_include_project_cmake',
                    'add_compile_options("-DUSELESS_MACRO_DOES_NOTHING=1")')
    rebuild_and_check(idf_py, APP_BINS + obj_files, PARTITION_BIN + BOOTLOADER_BINS + sdkconfig_files)


@pytest.mark.usefixtures('test_app_copy')
def test_rebuild_linker(idf_py: IdfPyFunc) -> None:
    idf_path = Path(os.environ['IDF_PATH'])
    logging.info('initial build')
    idf_py('build')

    logging.info('Updating rom ld file should re-link app and bootloader')
    (idf_path / 'components/esp_rom/esp32/ld/esp32.rom.ld').touch()
    rebuild_and_check(idf_py,
                      APP_BINS + BOOTLOADER_BINS, PARTITION_BIN)

    logging.info('Updating app-only sections ld file should only re-link the app')
    (idf_path / 'components/esp_system/ld/esp32/sections.ld.in').touch()
    rebuild_and_check(idf_py,
                      APP_BINS, BOOTLOADER_BINS + PARTITION_BIN)

    logging.info('Updating app-only memory ld file should only re-link the app')
    (idf_path / 'components/esp_system/ld/esp32/memory.ld.in').touch()
    rebuild_and_check(idf_py,
                      APP_BINS, BOOTLOADER_BINS + PARTITION_BIN)

    logging.info('Updating fragment file should only re-link the app')
    (idf_path / 'components/esp_common/common.lf').touch()
    rebuild_and_check(idf_py,
                      APP_BINS, BOOTLOADER_BINS + PARTITION_BIN)


@pytest.mark.usefixtures('idf_copy')
def test_rebuild_version_change(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    idf_path = Path(os.environ['IDF_PATH'])

    logging.info('Creating version files')
    version_idf_file = idf_path / 'version.txt'
    version_idf_file.write_text('IDF_VER_0123456789_0123456789_0123456789')

    version_tmp_file = test_app_copy / 'version.txt'
    version_tmp_file.write_text('project-version-1.0')

    logging.info('Build first version')
    idf_py('build')

    logging.info('Changing app version')
    version_tmp_file.write_text('project-version-2.0(012345678901234567890123456789)')

    logging.info('rebuild should update only app files, not bootloader')
    rebuild_and_check(idf_py, APP_BINS, BOOTLOADER_BINS)
    logging.info('new rebuild should not change anything')
    rebuild_and_check(idf_py, [], APP_BINS + BOOTLOADER_BINS)
