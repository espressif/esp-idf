# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_touch_pad_interrupt_v2(dut: Dut) -> None:
    dut.expect_exact('Touch pad: Initializing touch pad')
    dut.expect_exact('Touch pad: Denoise function init')
    dut.expect_exact('Touch pad: touch pad waterproof init')
    dut.expect_exact('Touch pad: touch pad filter init')
    dut.expect(r'touch pad \[\d+\] base \d+, thresh \d+')
