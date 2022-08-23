# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.generic
def test_touch_pad_interrupt_v1(dut: Dut) -> None:
    dut.expect_exact('Touch pad: Initializing touch pad')
    dut.expect(r'test init: touch pad \[\d+\] val is \d+')
    dut.expect_exact('Touch pad: Waiting for any pad being touched...')
    dut.expect_exact('Touch pad: Change mode...filter mode')
    dut.expect_exact('Touch pad: Waiting for any pad being touched...')
