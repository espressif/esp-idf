# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import re

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.generic
def test_examples_spiffs(dut: Dut) -> None:
    message_list = (rb'example: Initializing SPIFFS',
                    rb'example: Partition size: total: \d+, used: \d+',
                    rb'example: Opening file',
                    rb'example: File written',
                    rb'example: Renaming file',
                    rb'example: Reading file',
                    rb'example: Read from file: \'Hello World!\'',
                    rb'example: SPIFFS unmounted')

    for msg in message_list:
        dut.expect(re.compile(msg), timeout=60)
