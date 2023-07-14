# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import re
from pathlib import Path
from typing import Optional

import pytest
from test_build_system_helpers import IdfPyFunc, file_contains


def idf_version_from_cmake() -> Optional[str]:
    version_path = os.path.join(os.environ['IDF_PATH'], 'tools/cmake/version.cmake')
    regex = re.compile(r'^\s*set\s*\(\s*IDF_VERSION_([A-Z]{5})\s+(\d+)')
    ver = {}
    try:
        with open(version_path) as f:
            for line in f:
                m = regex.match(line)

                if m:
                    ver[m.group(1)] = m.group(2)

        return '%s.%s.%s' % (ver['MAJOR'], ver['MINOR'], ver['PATCH'])
    except (KeyError, OSError):
        pytest.fail('Cannot find ESP-IDF version in tools/cmake/version.cmake')
        return None


def test_sdkconfig_contains_all_files(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('sdkconfig should have contents of all files: sdkconfig, sdkconfig.defaults, sdkconfig.defaults.IDF_TARGET')
    (test_app_copy / 'sdkconfig').write_text('CONFIG_PARTITION_TABLE_TWO_OTA=y')
    (test_app_copy / 'sdkconfig.defaults').write_text('CONFIG_PARTITION_TABLE_OFFSET=0x10000')
    (test_app_copy / 'sdkconfig.defaults.esp32').write_text('CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ_240=y')
    idf_py('reconfigure')
    assert all([file_contains((test_app_copy / 'sdkconfig'), x) for x in ['CONFIG_PARTITION_TABLE_TWO_OTA=y',
                                                                          'CONFIG_PARTITION_TABLE_OFFSET=0x10000',
                                                                          'CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ_240=y']])


def test_sdkconfig_multiple_default_files(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('should be able to specify multiple sdkconfig default files')
    (test_app_copy / 'sdkconfig.defaults1').write_text('CONFIG_PARTITION_TABLE_OFFSET=0x10000')
    (test_app_copy / 'sdkconfig.defaults2').write_text('CONFIG_PARTITION_TABLE_TWO_OTA=y')
    idf_py('-DSDKCONFIG_DEFAULTS=sdkconfig.defaults1;sdkconfig.defaults2', 'reconfigure')
    assert all([file_contains((test_app_copy / 'sdkconfig'), x) for x in ['CONFIG_PARTITION_TABLE_TWO_OTA=y',
                                                                          'CONFIG_PARTITION_TABLE_OFFSET=0x10000']])


def test_keep_idf_init_version(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('sdkconfig should contain initial IDF version, even after regenerating the config')
    (test_app_copy / 'sdkconfig').write_text('CONFIG_IDF_INIT_VERSION="1.2.3"')
    idf_py('reconfigure')
    assert file_contains((test_app_copy / 'sdkconfig'), 'CONFIG_IDF_INIT_VERSION="1.2.3"')


def test_empty_idf_init_version(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('sdkconfig should add current IDF version as initial IDF version')
    (test_app_copy / 'sdkconfig').write_text('CONFIG_IDF_INIT_VERSION=""')
    idf_py('reconfigure')
    version = idf_version_from_cmake()
    assert file_contains((test_app_copy / 'sdkconfig'), f'CONFIG_IDF_INIT_VERSION="{version}"')
