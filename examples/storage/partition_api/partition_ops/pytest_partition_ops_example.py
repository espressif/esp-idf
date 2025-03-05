# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import re

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_partition_ops_example(dut: Dut) -> None:
    # ESP_ERROR_CHECK or assert will cause abort on error and "Example end" won't be received
    message_list = (
        rb'Written data: ESP-IDF Partition Operations Example \(Read, Erase, Write\)',
        rb'Read data: ESP-IDF Partition Operations Example \(Read, Erase, Write\)',
        rb'Erased data',
        rb'Example end',
    )

    for msg in message_list:
        dut.expect(re.compile(msg), timeout=20)
