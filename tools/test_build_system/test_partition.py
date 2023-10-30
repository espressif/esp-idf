# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import os
import re
import shutil
from pathlib import Path

import pytest
from test_build_system_helpers import EnvDict, IdfPyFunc, append_to_file, replace_in_file


@pytest.mark.usefixtures('test_app_copy')
def test_partition_table(idf_py: IdfPyFunc) -> None:
    logging.info('Displays partition table when executing target partition_table')
    ret = idf_py('partition-table')
    assert re.search('# ESP-IDF.+Partition Table', ret.stdout)


def test_partitions_dont_fit_in_flash(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info("Build fails if partitions don't fit in flash")
    append_to_file(test_app_copy / 'sdkconfig', 'CONFIG_ESPTOOLPY_FLASHSIZE_1MB=y')
    ret = idf_py('build', check=False)
    assert ret.returncode == 2
    assert 'does not fit in configured flash size 1MB' in ret.stdout


def test_partition_nearly_full_warning(idf_py: IdfPyFunc, test_app_copy: Path, default_idf_env: EnvDict) -> None:
    logging.info('Warning is given if smallest partition is nearly full')
    ret = idf_py('build')
    # Build a first time to get the binary size and to check that no warning is issued.
    assert 'partition is nearly full' not in ret.stdout, 'Warning for nearly full smallest partition was given when the condition is not fulfilled'
    # Get the size of the binary, in KB. Convert it to next multiple of 4.
    # The goal is to create an app partition which is slightly bigger than the binary itself
    updated_file_size = int((os.stat(test_app_copy / 'build' / 'build_test_app.bin').st_size + 4095) / 4096) * 4
    idf_path = Path(default_idf_env['IDF_PATH'])
    shutil.copy2(idf_path / 'components' / 'partition_table' / 'partitions_singleapp.csv', test_app_copy / 'partitions.csv')
    replace_in_file(test_app_copy / 'partitions.csv',
                    'factory,  app,  factory, ,        1M',
                    f'factory,  app,  factory, ,        {updated_file_size}K')
    (test_app_copy / 'sdkconfig').write_text('\n'.join(['CONFIG_PARTITION_TABLE_CUSTOM=y', 'CONFIG_FREERTOS_SMP=n']))
    ret = idf_py('build', check=False)
    assert 'partition is nearly full' in ret.stdout
