# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0


import re

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
def test_wear_levelling_example(dut: Dut) -> None:

    message_list = ('example: Mounting FAT filesystem',
                    'example: Opening file',
                    'example: File written',
                    'example: Reading file',
                    re.compile(str.encode('example: Read from file: \'hello world, from ESP-IDF \\S+\'')),
                    re.compile(str.encode('vfs_fat_spiflash: Formatting FATFS partition, allocation unit size=\\S+')),
                    'example: file doesnt exist, format done',
                    'example: Opening file',
                    'example: File written',
                    'example: Reading file',
                    re.compile(str.encode('example: Read from file: \'nihao shijie, from ESP-IDF \\S+\'')),
                    'example: Unmounting FAT filesystem',
                    'example: Done')

    for msg in message_list:
        dut.expect(msg, timeout=20)
