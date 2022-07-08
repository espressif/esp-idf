# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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
                    re.compile(str.encode('example: Read from file: \'written using ESP-IDF \\S+\'')),
                    'example: Unmounting FAT filesystem',
                    'example: Done')

    for msg in message_list:
        dut.expect(msg, timeout=20)
