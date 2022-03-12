# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
from typing import Optional

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_fatfsgen(env: ttfw_idf.TinyFW.Env, _: Optional[list]) -> None:
    dut = env.get_dut('fatfsgen', 'examples/storage/fatfsgen', app_config_name='test_read_write_partition_gen')
    dut.start_app()
    dut.expect_all('example: Mounting FAT filesystem',
                   'example: Opening file',
                   'example: File written',
                   'example: Reading file',
                   'example: Read from file: \'This is written by the device\'',
                   'example: Reading file',
                   'example: Read from file: \'This is generated on the host\'',
                   'example: Unmounting FAT filesystem',
                   'example: Done',
                   timeout=20)
    env.close_dut(dut.name)

    dut = env.get_dut('fatfsgen', 'examples/storage/fatfsgen', app_config_name='test_read_only_partition_gen')
    dut.start_app()
    dut.expect_all('example: Mounting FAT filesystem',
                   'example: Reading file',
                   'example: Read from file: \'this is test\'',
                   'example: Unmounting FAT filesystem',
                   'example: Done',
                   timeout=20)
    env.close_dut(dut.name)

    dut = env.get_dut('fatfsgen', 'examples/storage/fatfsgen', app_config_name='test_read_write_partition_gen_ln')
    dut.start_app()
    dut.expect_all('example: Mounting FAT filesystem',
                   'example: Opening file',
                   'example: File written',
                   'example: Reading file',
                   'example: Read from file: \'This is written by the device\'',
                   'example: Reading file',
                   'example: Read from file: \'This is generated on the host; long name it has\'',
                   'example: Unmounting FAT filesystem',
                   'example: Done',
                   timeout=20)
    env.close_dut(dut.name)

    dut = env.get_dut('fatfsgen', 'examples/storage/fatfsgen', app_config_name='test_read_only_partition_gen_ln')
    dut.start_app()
    dut.expect_all('example: Mounting FAT filesystem',
                   'example: Reading file',
                   'example: Read from file: \'this is test; long name it has\'',
                   'example: Unmounting FAT filesystem',
                   'example: Done',
                   timeout=20)
    env.close_dut(dut.name)


if __name__ == '__main__':
    test_examples_fatfsgen()
