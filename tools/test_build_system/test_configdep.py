# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
from pathlib import Path

import pytest
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import append_to_file
from test_build_system_helpers import bin_file_contains
from test_build_system_helpers import get_snapshot
from test_build_system_helpers import replace_in_file

CONFIGDEP_TEST_APP = 'tools/test_build_system/configdep_test_app'

_MAIN_OBJ_DIR = ('build', 'esp-idf', 'main', 'CMakeFiles', '__idf_main.dir')
MAIN_OBJ = os.path.join(*_MAIN_OBJ_DIR, 'configdep_main.c.obj')
UNRELATED_OBJ = os.path.join(*_MAIN_OBJ_DIR, 'unrelated.c.obj')
CONFIGDEP_ELF = Path(os.path.join('build', 'configdep_test_app.elf'))


@pytest.mark.test_app_copy(CONFIGDEP_TEST_APP)
@pytest.mark.usefixtures('test_app_copy')
def test_configdep_selective_rebuild(idf_py: IdfPyFunc) -> None:
    """Verify selective rebuild when a Kconfig option toggles.

    ``esp-idf-configdep`` scans each translation unit's source for ``CONFIG_*``
    tokens and appends matching ``.cdep`` stubs to the compiler depfile. Avoid
    spelling projbuild-only symbols (even in comments) in ``unrelated.c``, or the
    scanner adds a spurious ``option.cdep`` edge and Ninja rebuilds that ``.obj``.
    """
    logging.info('initial build with configdep enabled (default)')
    idf_py('build')

    logging.info('toggle CONFIG_TEST_CONFIGDEP_OPTION from y to n')
    replace_in_file('sdkconfig', 'CONFIG_TEST_CONFIGDEP_OPTION=y', '# CONFIG_TEST_CONFIGDEP_OPTION is not set')

    snapshot_main = get_snapshot(MAIN_OBJ)
    snapshot_unrelated = get_snapshot(UNRELATED_OBJ)

    logging.info('rebuild after config change')
    idf_py('build')

    logging.info('configdep_main.c.obj must be rebuilt (references changed option)')
    snapshot_main.assert_different(get_snapshot(MAIN_OBJ))

    logging.info('unrelated.c.obj must not be rebuilt (no CONFIG_* token for that option in source)')
    snapshot_unrelated.assert_same(get_snapshot(UNRELATED_OBJ))


@pytest.mark.test_app_copy(CONFIGDEP_TEST_APP)
@pytest.mark.usefixtures('test_app_copy')
def test_configdep_no_rebuild_without_change(idf_py: IdfPyFunc) -> None:
    logging.info('initial build with configdep enabled (default)')
    idf_py('build')

    snapshot_main = get_snapshot(MAIN_OBJ)
    snapshot_unrelated = get_snapshot(UNRELATED_OBJ)

    logging.info('rebuild with no config changes')
    idf_py('build')

    logging.info('neither object file should be rebuilt')
    snapshot_main.assert_same(get_snapshot(MAIN_OBJ))
    snapshot_unrelated.assert_same(get_snapshot(UNRELATED_OBJ))


@pytest.mark.test_app_copy(CONFIGDEP_TEST_APP)
@pytest.mark.usefixtures('test_app_copy')
def test_configdep_elf_strings_track_sdkconfig_toggle(idf_py: IdfPyFunc) -> None:
    """After rebuild, the firmware string matches the final Kconfig (host-side ELF check)."""
    idf_py('build')
    assert CONFIGDEP_ELF.is_file()
    assert bin_file_contains(CONFIGDEP_ELF, b'CONFIGDEP_OPTION: ENABLED')
    assert not bin_file_contains(CONFIGDEP_ELF, b'CONFIGDEP_OPTION: DISABLED')

    append_to_file('sdkconfig', '\n# CONFIG_TEST_CONFIGDEP_OPTION is not set\n')
    idf_py('build')
    assert bin_file_contains(CONFIGDEP_ELF, b'CONFIGDEP_OPTION: DISABLED')
    assert not bin_file_contains(CONFIGDEP_ELF, b'CONFIGDEP_OPTION: ENABLED')
