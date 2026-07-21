# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import re

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_examples_spiffs(dut: Dut) -> None:
    message_list = (
        rb'example: Initializing SPIFFS',
        rb'example: Partition size: total: \d+, used: \d+',
        rb'example: Opening file',
        rb'example: File written',
        rb'example: Renaming file',
        rb'example: Reading file',
        rb'example: Read from file: \'Hello World!\'',
        rb'example: SPIFFS unmounted',
    )

    # Startup is fail-fast (Initializing SPIFFS appears quickly).
    # First-boot format of ~960KB SPIFFS can exceed 120s on esp32c3 CI UART runners,
    # so allow up to 240s for format/remount; remaining expects use 120s.
    dut.expect(re.compile(message_list[0]), timeout=30)

    spiffs_format = rb'SPIFFS: mount failed, -10025\. formatting\.\.\.'
    format_or_partition = re.compile(rb'(?:' + spiffs_format + rb'|' + message_list[1] + rb')')
    if dut.expect(format_or_partition, timeout=240) == 0:
        dut.expect(re.compile(message_list[1]), timeout=240)

    for msg in message_list[2:]:
        dut.expect(re.compile(msg), timeout=120)
