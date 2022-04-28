# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os
from datetime import datetime
from pathlib import Path
from typing import Optional

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_fatfsgen(env: ttfw_idf.TinyFW.Env, _: Optional[list]) -> None:
    tag = 'fatfsgen'
    test_path = 'examples/storage/fatfsgen'
    timeout = 20
    filename_ln = 'sublongnames/testlongfilenames.txt'
    filename_sn = 'sub/test.txt'
    dir_ln = os.path.join(os.path.dirname(__file__), 'fatfs_long_name_image')
    dir_sn = os.path.join(os.path.dirname(__file__), 'fatfs_image')
    Path(os.path.join(dir_ln, filename_ln)).touch()
    Path(os.path.join(dir_sn, filename_sn)).touch()
    date_modified = datetime.today().strftime('%Y-%m-%d')
    date_default = '1980-01-01'

    for test_name in ['test_read_write_partition_gen', 'test_read_write_partition_gen_default_dt']:
        filename = filename_sn
        filename_expected = f'/spiflash/{filename}'
        date_ = date_default if test_name == 'test_read_write_partition_gen_default_dt' else date_modified
        dut = env.get_dut(tag, test_path, app_config_name=test_name)
        dut.start_app()
        dut.expect_all('example: Mounting FAT filesystem',
                       'example: Opening file',
                       'example: File written',
                       'example: Reading file',
                       'example: Read from file: \'This is written by the device\'',
                       'example: Reading file',
                       f'The file \'{filename_expected}\' was modified at date: {date_}',
                       'example: Read from file: \'This is generated on the host\'',
                       'example: Unmounting FAT filesystem',
                       'example: Done',
                       timeout=timeout)
        env.close_dut(dut.name)

    for test_name in ['test_read_only_partition_gen', 'test_read_only_partition_gen_default_dt']:
        filename = filename_sn
        filename_expected = f'/spiflash/{filename}'
        date_ = date_default if test_name == 'test_read_only_partition_gen_default_dt' else date_modified
        dut = env.get_dut(tag, test_path, app_config_name=test_name)
        dut.start_app()
        dut.expect_all('example: Mounting FAT filesystem',
                       'example: Reading file',
                       f'The file \'{filename_expected}\' was modified at date: {date_}',
                       'example: Read from file: \'this is test\'',
                       'example: Unmounting FAT filesystem',
                       'example: Done',
                       timeout=timeout)
        env.close_dut(dut.name)

    for test_name in ['test_read_write_partition_gen_ln', 'test_read_write_partition_gen_ln_default_dt']:
        filename = filename_ln
        filename_expected = f'/spiflash/{filename}'
        date_ = date_default if test_name == 'test_read_write_partition_gen_ln_default_dt' else date_modified
        dut = env.get_dut(tag, test_path, app_config_name=test_name)
        dut.start_app()
        dut.expect_all('example: Mounting FAT filesystem',
                       'example: Opening file',
                       'example: File written',
                       'example: Reading file',
                       'example: Read from file: \'This is written by the device\'',
                       'example: Reading file',
                       f'The file \'{filename_expected}\' was modified at date: {date_}',
                       'example: Read from file: \'This is generated on the host; long name it has\'',
                       'example: Unmounting FAT filesystem',
                       'example: Done',
                       timeout=timeout)
        env.close_dut(dut.name)

    for test_name in ['test_read_only_partition_gen_ln', 'test_read_only_partition_gen_ln_default_dt']:
        filename = filename_ln
        filename_expected = f'/spiflash/{filename}'
        date_ = date_default if test_name == 'test_read_only_partition_gen_ln_default_dt' else date_modified
        dut = env.get_dut(tag, test_path, app_config_name=test_name)
        dut.start_app()
        dut.expect_all('example: Mounting FAT filesystem',
                       'example: Reading file',
                       f'The file \'{filename_expected}\' was modified at date: {date_}',
                       'example: Read from file: \'this is test; long name it has\'',
                       'example: Unmounting FAT filesystem',
                       'example: Done',
                       timeout=timeout)
        env.close_dut(dut.name)


if __name__ == '__main__':
    test_examples_fatfsgen()
