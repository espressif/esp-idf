# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import logging
import re

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.sdcard_sdmode
@idf_parametrize('target', ['esp32', 'esp32p4'], indirect=['target'])
def test_examples_sd_card_sdmmc(dut: Dut) -> None:
    dut.expect('example: Initializing SD card', timeout=20)
    dut.expect('example: Using SDMMC peripheral', timeout=10)

    # Provide enough time for possible SD card formatting
    dut.expect('Filesystem mounted', timeout=60)

    # These lines are matched separately because of ASCII color codes in the output
    name = dut.expect(re.compile(rb'Name: (\w+)\r'), timeout=10).group(1).decode()
    _type = dut.expect(re.compile(rb'Type: (\S+)'), timeout=10).group(1).decode()
    speed = dut.expect(re.compile(rb'Speed: (\S+)'), timeout=10).group(1).decode()
    size = dut.expect(re.compile(rb'Size: (\S+)'), timeout=10).group(1).decode()

    logging.info(f'Card {name} {_type} {speed}MHz {size} found')

    message_list1 = (
        'Opening file /sdcard/hello.txt',
        'File written',
        'Renaming file /sdcard/hello.txt to /sdcard/foo.txt',
        'Reading file /sdcard/foo.txt',
        f"Read from file: 'Hello {name}!'",
    )
    sd_card_format = re.compile(str.encode('formatting drive, allocation unit size=\\S+'))
    after_card_format = "file doesn't exist, formatting done"
    message_list2 = (
        'Opening file /sdcard/nihao.txt',
        'File written',
        'Reading file /sdcard/nihao.txt',
        f"Read from file: 'Nihao {name}!'",
        'Card unmounted',
    )

    for msg in message_list1:
        dut.expect_exact(msg, timeout=30)
    dut.expect(sd_card_format, timeout=10)
    dut.expect_exact(after_card_format, timeout=180)  # Provide enough time for SD card FATFS format operation
    for msg in message_list2:
        dut.expect_exact(msg, timeout=30)
