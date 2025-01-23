# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.generic
def test_i2s_usb_example(dut: Dut) -> None:
    dut.expect(r'initial MCLK: [0-9]+ Hz')
