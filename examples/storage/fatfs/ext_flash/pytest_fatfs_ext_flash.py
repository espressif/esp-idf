# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.external_flash
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_ext_flash_fatfs(dut: Dut) -> None:
    message_list = (
        'Initialized external Flash',
        "partition 'nvs'",
        "partition 'storage'",
        'File written',
        "Read from file: 'Written using ESP-IDF",
    )

    for msg in message_list:
        dut.expect(msg, timeout=20)
