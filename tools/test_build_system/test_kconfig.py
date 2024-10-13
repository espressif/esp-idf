# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import shutil
import textwrap
from contextlib import contextmanager
from pathlib import Path
from typing import Iterator

import pytest
from test_build_system_helpers import append_to_file
from test_build_system_helpers import file_contains
from test_build_system_helpers import IdfPyFunc


@contextmanager
def backup_required_files(test_app_copy: Path) -> Iterator[None]:
    idf_path = Path(os.environ['IDF_PATH'])

    sdk_rename_backup = (idf_path / 'sdkconfig.rename').read_text()
    kconfig_backup = (idf_path / 'Kconfig').read_text()
    try:
        yield
    finally:
        (idf_path / 'sdkconfig.rename').write_text(sdk_rename_backup)
        (idf_path / 'Kconfig').write_text(kconfig_backup)
        shutil.rmtree(test_app_copy / 'build', ignore_errors=True)
        if (test_app_copy / 'sdkconfig').exists():
            (test_app_copy / 'sdkconfig').unlink()


# For this and the following test function, there are actually two logical
# tests in one test function. It would be better to have every check in a separate
# test case, but that would mean doing idf_copy each time, and copying takes most of the time
@pytest.mark.usefixtures('idf_copy')
def test_kconfig_deprecated_options(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    idf_path = Path(os.environ['IDF_PATH'])

    with backup_required_files(test_app_copy):
        logging.info('Handling deprecated Kconfig options')
        (idf_path / 'sdkconfig.rename').write_text('')
        idf_py('reconfigure')
        append_to_file((test_app_copy / 'sdkconfig'), 'CONFIG_TEST_OLD_OPTION=y')
        (idf_path / 'sdkconfig.rename').write_text('CONFIG_TEST_OLD_OPTION CONFIG_TEST_NEW_OPTION')
        append_to_file((idf_path / 'Kconfig'), textwrap.dedent("""
                                                               menu "test"
                                                                   config TEST_NEW_OPTION
                                                                       bool "test"
                                                                       default "n"
                                                                       help
                                                                           TEST_NEW_OPTION description
                                                               endmenu
                                                               """))
        idf_py('reconfigure')
        assert all([file_contains((test_app_copy / 'sdkconfig'), x) for x in ['CONFIG_TEST_OLD_OPTION=y',
                                                                              'CONFIG_TEST_NEW_OPTION=y']])
        assert all([file_contains((test_app_copy / 'build' / 'config' / 'sdkconfig.h'), x) for x in ['#define CONFIG_TEST_NEW_OPTION 1',
                                                                                                     '#define CONFIG_TEST_OLD_OPTION CONFIG_TEST_NEW_OPTION']])
        assert all([file_contains((test_app_copy / 'build' / 'config' / 'sdkconfig.cmake'), x) for x in ['set(CONFIG_TEST_OLD_OPTION "y")',
                                                                                                         'set(CONFIG_TEST_NEW_OPTION "y")']])

    logging.info('Handling deprecated Kconfig options in sdkconfig.defaults')
    (test_app_copy / 'sdkconfig.defaults').write_text('CONFIG_TEST_OLD_OPTION=7')
    (idf_path / 'sdkconfig.rename').write_text('CONFIG_TEST_OLD_OPTION CONFIG_TEST_NEW_OPTION')
    append_to_file((idf_path / 'Kconfig'), textwrap.dedent("""
                                                           menu "test"
                                                               config TEST_NEW_OPTION
                                                                   int "TEST_NEW_OPTION"
                                                                   range 0 10
                                                                   default 5
                                                                   help
                                                                       TEST_NEW_OPTION description
                                                           endmenu
                                                           """))
    idf_py('reconfigure')
    assert all([file_contains((test_app_copy / 'sdkconfig'), x) for x in ['CONFIG_TEST_OLD_OPTION=7',
                                                                          'CONFIG_TEST_NEW_OPTION=7']])


@pytest.mark.usefixtures('idf_copy')
@pytest.mark.idf_copy('test_kconfig_various_options')
def test_kconfig_multiple_and_target_specific_options(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    idf_path = Path(os.environ['IDF_PATH'])

    with backup_required_files(test_app_copy):
        logging.info('Can have multiple deprecated Kconfig options map to a single new option')
        (idf_path / 'sdkconfig.rename').write_text('')
        idf_py('reconfigure')
        append_to_file((test_app_copy / 'sdkconfig'), 'CONFIG_TEST_NEW_OPTION=y')
        append_to_file((idf_path / 'sdkconfig.rename'), '\n'.join(['CONFIG_TEST_OLD_OPTION_1 CONFIG_TEST_NEW_OPTION',
                                                                   'CONFIG_TEST_OLD_OPTION_2 CONFIG_TEST_NEW_OPTION']))
        append_to_file((idf_path / 'Kconfig'), textwrap.dedent("""
                                                               menu "test"
                                                               config TEST_NEW_OPTION
                                                                   bool "test"
                                                                   default "n"
                                                                   help
                                                                       TEST_NEW_OPTION description
                                                               endmenu
                                                               """))
        idf_py('reconfigure')
        assert all([file_contains((test_app_copy / 'sdkconfig'), x) for x in ['CONFIG_TEST_OLD_OPTION_1=y',
                                                                              'CONFIG_TEST_OLD_OPTION_2=y']])
        assert all([file_contains((test_app_copy / 'build' / 'config' / 'sdkconfig.h'), x) for x in ['#define CONFIG_TEST_OLD_OPTION_1 CONFIG_TEST_NEW_OPTION',
                                                                                                     '#define CONFIG_TEST_OLD_OPTION_2 CONFIG_TEST_NEW_OPTION'
                                                                                                     ]])
        assert all([file_contains((test_app_copy / 'build' / 'config' / 'sdkconfig.cmake'), x) for x in ['set(CONFIG_TEST_OLD_OPTION_1 "y")',
                                                                                                         'set(CONFIG_TEST_OLD_OPTION_2 "y")']])

    logging.info('Can have target specific deprecated Kconfig options')
    (test_app_copy / 'sdkconfig').write_text('CONFIG_TEST_OLD_OPTION=y')
    append_to_file((idf_path / 'components' / 'esp_system' / 'sdkconfig.rename.esp32s2'), 'CONFIG_TEST_OLD_OPTION CONFIG_TEST_NEW_OPTION')
    append_to_file((idf_path / 'Kconfig'), textwrap.dedent("""
                                                           menu "test"
                                                           config TEST_NEW_OPTION
                                                               bool "TEST_NEW_OPTION"
                                                               default y
                                                               help
                                                                   TEST_NEW_OPTION description
                                                           endmenu
                                                           """))
    idf_py('set-target', 'esp32')
    assert not file_contains((test_app_copy / 'sdkconfig'), 'CONFIG_TEST_OLD_OPTION=y')
    assert file_contains((test_app_copy / 'sdkconfig'), 'CONFIG_TEST_NEW_OPTION=y')
    (test_app_copy / 'sdkconfig').unlink()
    idf_py('set-target', 'esp32s2')
    assert all([file_contains((test_app_copy / 'sdkconfig'), x) for x in ['CONFIG_TEST_NEW_OPTION=y',
                                                                          'CONFIG_TEST_OLD_OPTION=y']])
