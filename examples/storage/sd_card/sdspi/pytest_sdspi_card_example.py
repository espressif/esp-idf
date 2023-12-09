# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0


import logging
import re

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3  # no runner available at the moment
@pytest.mark.sdcard_spimode
def test_examples_sd_card_sdspi(dut: Dut) -> None:
    dut.expect('example: Initializing SD card', timeout=20)
    dut.expect('example: Using SPI peripheral', timeout=20)

    # Provide enough time for possible SD card formatting
    dut.expect('Filesystem mounted', timeout=60)

    # These lines are matched separately because of ASCII color codes in the output
    name = dut.expect(re.compile(rb'Name: (\w+)\r'), timeout=20).group(1).decode()
    _type = dut.expect(re.compile(rb'Type: (\S+)'), timeout=20).group(1).decode()
    speed = dut.expect(re.compile(rb'Speed: (\S+)'), timeout=20).group(1).decode()
    size = dut.expect(re.compile(rb'Size: (\S+)'), timeout=20).group(1).decode()

    logging.info('Card {} {} {}MHz {} found'.format(name, _type, speed, size))

    message_list1 = ('Opening file /sdcard/hello.txt',
                     'File written',
                     'Renaming file /sdcard/hello.txt to /sdcard/foo.txt',
                     'Reading file /sdcard/foo.txt',
                     "Read from file: 'Hello {}!'".format(name))
    sd_card_format = re.compile(str.encode('Formatting card, allocation unit size=\\S+'))
    message_list2 = ('file doesnt exist, format done',
                     'Opening file /sdcard/nihao.txt',
                     'File written',
                     'Reading file /sdcard/nihao.txt',
                     "Read from file: 'Nihao {}!'".format(name),
                     'Card unmounted')

    for msg in message_list1:
        dut.expect_exact(msg, timeout=30)
    dut.expect(sd_card_format, timeout=180)  # Provide enough time for SD card FATFS format operation
    for msg in message_list2:
        dut.expect_exact(msg, timeout=30)
