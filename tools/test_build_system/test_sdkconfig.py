# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
from pathlib import Path

from test_build_system_helpers import IdfPyFunc, file_contains


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
