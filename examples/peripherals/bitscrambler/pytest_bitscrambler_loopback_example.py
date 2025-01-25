# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32p4
@pytest.mark.esp32c5
@pytest.mark.generic
def test_bitscrambler_loopback_example(dut: Dut) -> None:
    dut.expect_exact('BitScrambler example main', timeout=5)
    dut.expect_exact('BitScrambler program complete. Input 40, output 40 bytes')

    expected_lines = [
        'FF 00 00 00 00 00 00 00',
        '80 80 80 80 80 80 80 80',
        '01 02 04 08 10 20 40 80',
        '00 FF 00 FF 00 FF 00 FF',
        'FF 00 FF 00 FF 00 FF 00',
    ]
    for line in expected_lines:
        dut.expect_exact(line)
