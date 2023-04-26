# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0


import logging
import re

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s3
@pytest.mark.emmc
@pytest.mark.parametrize(
    'config',
    [
        '1line',
        '4line',
        '8line',
    ],
    indirect=True,
)
def test_examples_sd_card_sdmmc(dut: Dut) -> None:
    dut.expect('example: Initializing eMMC', timeout=20)
    dut.expect('example: Using SDMMC peripheral', timeout=10)

    # Provide enough time for possible SD card formatting
    dut.expect('Filesystem mounted', timeout=60)

    # These lines are matched separately because of ASCII color codes in the output
    name = dut.expect(re.compile(rb'Name: (\w+)\r'), timeout=10).group(1).decode()
    _type = dut.expect(re.compile(rb'Type: (\S+)'), timeout=10).group(1).decode()
    speed = dut.expect(re.compile(rb'Speed: (\S+)'), timeout=10).group(1).decode()
    size = dut.expect(re.compile(rb'Size: (\S+)'), timeout=10).group(1).decode()

    logging.info('Card {} {} {}MHz {} found'.format(name, _type, speed, size))

    message_list = ('Opening file /eMMC/hello.txt',
                    'File written',
                    'Renaming file /eMMC/hello.txt to /eMMC/foo.txt',
                    'Reading file /eMMC/foo.txt',
                    "Read from file: 'Hello {}!'".format(name),
                    'Card unmounted')

    for msg in message_list:
        dut.expect(msg, timeout=10)
