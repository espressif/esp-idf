# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
from time import sleep

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.generic
def test_console_advanced(dut: Dut) -> None:
    sleep(2)  # Some time for the OS to enumerate our USB device

    # Wait until the console prompt appears
    dut.expect(dut.target + '> ')

    # Write CLI command "version"
    dut.write('version')
    sleep(0.5)

    # Check if following strings are present in the "version" command output
    dut.expect('IDF Version')
    dut.expect('Chip info')
