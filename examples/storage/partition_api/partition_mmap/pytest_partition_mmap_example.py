# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import re

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.generic
def test_partition_mmap_example(dut: Dut) -> None:
    # ESP_ERROR_CHECK or assert will cause abort on error and "Example end" won't be received
    message_list = (rb'Written sample data to partition: ESP-IDF Partition Memory Map Example',
                    rb'Mapped partition to data memory address \S+',
                    rb'Read sample data from partition using mapped memory: ESP-IDF Partition Memory Map Example',
                    rb'Data matches',
                    rb'Unmapped partition from data memory',
                    rb'Example end')

    for msg in message_list:
        dut.expect(re.compile(msg), timeout=20)
