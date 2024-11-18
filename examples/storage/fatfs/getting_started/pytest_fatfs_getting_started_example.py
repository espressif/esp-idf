# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.generic
def test_examples_fatfs_getting_started(dut: Dut) -> None:
    dut.expect('example: Mounting FAT filesystem', timeout=90)
    dut.expect('example: Filesystem mounted', timeout=90)
    dut.expect('example: Opening file', timeout=90)
    dut.expect('example: File written', timeout=90)
    dut.expect('example: Reading file', timeout=90)
    dut.expect('example: Read from file: \'Hello World!\'', timeout=90)
    dut.expect('example: Unmounting FAT filesystem', timeout=90)
    dut.expect('example: Done', timeout=90)
