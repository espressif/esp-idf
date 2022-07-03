# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.generic
def test_i2s_adc_dac_example(dut: Dut) -> None:
    dut.expect('partiton addr: 0x([0-9a-fA-F]+); size: ([0-9]+); label: storage', timeout=30)
    dut.expect_exact('Erasing flash', timeout=30)
    dut.expect('partiton addr: 0x([0-9a-fA-F]+); size: ([0-9]+); label: storage', timeout=30)
    dut.expect('Erase size: ([0-9]+) Bytes', timeout=30)
    dut.expect('I \\(([0-9]+)\\) ad/da: ([0-9]+) mV', timeout=30)
