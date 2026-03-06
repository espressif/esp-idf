# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
# These tests check whether the build system rebuilds some files or not
# depending on the changes to the project. Configdep is disabled here so
# mtime assertions reflect CMake/Ninja only; see test_rebuild_configdep.py for
# the same checks with configdep enabled (after settle builds).
import logging
import os
import subprocess
from pathlib import Path
from typing import cast

import pytest
from test_build_system_helpers import ALL_ARTIFACTS_BUILDV1
from test_build_system_helpers import ALL_ARTIFACTS_BUILDV2
from test_build_system_helpers import APP_BINS
from test_build_system_helpers import BOOTLOADER_BINS
from test_build_system_helpers import BUILD_SNAPSHOT_EXCLUDE
from test_build_system_helpers import PARTITION_BIN
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import get_idf_build_env
from test_build_system_helpers import get_snapshot
from test_build_system_helpers import rebuild_and_check
from test_build_system_helpers import replace_in_file
from test_build_system_helpers import run_idf_py


@pytest.fixture
def idf_py(monkeypatch: pytest.MonkeyPatch) -> IdfPyFunc:
    """Use ``idf.py`` with esp-idf-configdep disabled for this module.

    These tests assert Ninja rebuild granularity via file mtimes. The
    ``esp-idf-configdep`` wrapper can create per-option ``.cdep`` stubs after the
    real compiler has written ``.obj`` files, so on the next Ninja run inputs can
    look newer than outputs and trigger spurious rebuilds. That behavior is
    covered separately in ``test_rebuild_configdep.py`` with a settle-build
    pattern. Here we exercise the core CMake/Ninja dependency graph only.
    """
    monkeypatch.setenv('IDF_CONFIGDEP_ENABLE', '0')
    env = get_idf_build_env(os.environ['IDF_PATH'])

    def result(*args: str, check: bool = True, input_str: str | None = None) -> subprocess.CompletedProcess[str]:
        return cast(
            subprocess.CompletedProcess[str],
            run_idf_py(*args, env=env, workdir=os.getcwd(), check=check, input_str=input_str),
        )

    return result


@pytest.mark.usefixtures('test_app_copy')
def test_rebuild_no_changes(idf_py: IdfPyFunc, request: pytest.FixtureRequest) -> None:
    logging.info('initial build')
    idf_py('build')
    logging.info('get the first snapshot')
    # excluding the 'log' subdirectory here since it changes after every build
    all_build_files = get_snapshot('build/**/*', exclude_patterns=BUILD_SNAPSHOT_EXCLUDE)

    logging.info('check that all build artifacts were generated')
    all_artifacts = ALL_ARTIFACTS_BUILDV2 if request.config.getoption('buildv2', False) else ALL_ARTIFACTS_BUILDV1
    for artifact in all_artifacts:
        assert Path(artifact).exists()

    logging.info('build again with no changes')
    idf_py('build')
    # if there are no changes, nothing gets rebuilt
    all_build_files_after_rebuild = get_snapshot('build/**/*', exclude_patterns=BUILD_SNAPSHOT_EXCLUDE)
    all_build_files_after_rebuild.assert_same(all_build_files)


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

    logging.info('sdkconfig update triggers full recompile')
    # pick on .c, .cpp, .S file which includes sdkconfig.h:
    obj_files = [
        'build/esp-idf/esp_libc/CMakeFiles/__idf_esp_libc.dir/newlib_init.c.obj',
        'build/esp-idf/nvs_flash/CMakeFiles/__idf_nvs_flash.dir/src/nvs_api.cpp.obj',
        'build/esp-idf/esp_system/CMakeFiles/__idf_esp_system.dir/port/arch/xtensa/panic_handler_asm.S.obj',
    ]
    sdkconfig_files = ['build/config/sdkconfig.h', 'build/config/sdkconfig.json']
    replace_in_file('sdkconfig', '# CONFIG_FREERTOS_UNICORE is not set', 'CONFIG_FREERTOS_UNICORE=y')
    rebuild_and_check(idf_py, APP_BINS + BOOTLOADER_BINS + obj_files + sdkconfig_files, PARTITION_BIN)

    logging.info('Updating project CMakeLists.txt triggers app recompile')
    replace_in_file(
        'CMakeLists.txt',
        '# placeholder_after_include_project_cmake',
        'add_compile_options("-DUSELESS_MACRO_DOES_NOTHING=1")',
    )
    rebuild_and_check(idf_py, APP_BINS + obj_files, PARTITION_BIN + BOOTLOADER_BINS + sdkconfig_files)


@pytest.mark.usefixtures('test_app_copy')
def test_rebuild_linker(idf_py: IdfPyFunc) -> None:
    idf_path = Path(os.environ['IDF_PATH'])
    logging.info('initial build')
    idf_py('build')

    logging.info('Updating rom ld file should re-link app and bootloader')
    (idf_path / 'components/esp_rom/esp32/ld/esp32.rom.ld').touch()
    rebuild_and_check(idf_py, APP_BINS + BOOTLOADER_BINS, PARTITION_BIN)

    logging.info('Updating app-only sections ld file should only re-link the app')
    (idf_path / 'components/esp_system/ld/esp32/sections.ld.in').touch()
    rebuild_and_check(idf_py, APP_BINS, BOOTLOADER_BINS + PARTITION_BIN)

    logging.info('Updating app-only memory ld file should only re-link the app')
    (idf_path / 'components/esp_system/ld/esp32/memory.ld.in').touch()
    rebuild_and_check(idf_py, APP_BINS, BOOTLOADER_BINS + PARTITION_BIN)

    logging.info('Updating fragment file should only re-link the app')
    (idf_path / 'components/esp_common/common.lf').touch()
    rebuild_and_check(idf_py, APP_BINS, BOOTLOADER_BINS + PARTITION_BIN)


def test_rebuild_ldgen_fingerprint(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    """Verify the ldgen fingerprint skip path: when section names and other
    inputs are unchanged, ldgen exits early without regenerating sections.ld
    and prints an informational message.
    """
    skip_msg = 'Skipping linker script generation, section names unchanged'
    app_c = test_app_copy / 'main' / 'build_test_app.c'

    # Seed app_main with a printf so the first build establishes the string
    # literal and its .rodata section. Later we only change the arithmetic
    # constant, which modifies .text.app_main contents but keeps section
    # names unchanged — exactly the case the fingerprint should optimize.
    replace_in_file(app_c, '// placeholder_inside_main', 'printf("value = %d\\n", 1 + 2);')

    logging.info('initial build')
    idf_py('build')

    logging.info(
        'changing the printed calculation modifies .text.app_main but keeps all section names - fingerprint should hit'
    )
    replace_in_file(app_c, '1 + 2', '3 + 4')
    result = idf_py('build')
    assert skip_msg in result.stdout, f'expected {skip_msg!r} in build output'

    logging.info('touching a fragment file invalidates the fingerprint')
    idf_path = Path(os.environ['IDF_PATH'])
    (idf_path / 'components/esp_common/common.lf').touch()
    result = idf_py('build')
    assert skip_msg not in result.stdout, f'unexpected {skip_msg!r} in build output after fragment change'


def test_rebuild_ldgen_lf_cache(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    """Verify the ldgen lf cache: when section names change but fragment files
    don't, parsed FragmentFile objects are loaded from cache rather than
    re-parsed, and an informational message is printed.
    """
    cache_msg = 'Skipping linker fragment parsing, fragment files unchanged'

    logging.info('initial build')
    idf_py('build')

    logging.info(
        'adding a new function changes section names but leaves fragment files unchanged - lf cache should hit'
    )
    replace_in_file(
        test_app_copy / 'main' / 'build_test_app.c',
        '// placeholder_before_main',
        'void test_ldgen_lf_cache_extra_func(void) {}',
    )
    result = idf_py('build')
    assert cache_msg in result.stdout, f'expected {cache_msg!r} in build output'

    logging.info('touching a fragment file invalidates the lf cache')
    idf_path = Path(os.environ['IDF_PATH'])
    (idf_path / 'components/esp_common/common.lf').touch()
    result = idf_py('build')
    assert cache_msg not in result.stdout, f'unexpected {cache_msg!r} in build output after fragment change'


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
