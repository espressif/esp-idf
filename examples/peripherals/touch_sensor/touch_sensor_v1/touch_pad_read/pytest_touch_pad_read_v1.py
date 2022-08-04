# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.generic
def test_touch_pad_read_v1(dut: Dut) -> None:
    dut.expect_exact('Touch Sensor filter mode read, the output format is:')
    dut.expect(r'T0:\[\s*\d+,\s*\d+\] T1:\[\s*\d+,\s*\d+\] T2:\[\s*\d+,\s*\d+\] T3:\[\s*\d+,\s*\d+\] T4:\[\s*\d+,\s*\d+\]'
               r' T5:\[\s*\d+,\s*\d+\] T6:\[\s*\d+,\s*\d+\] T7:\[\s*\d+,\s*\d+\] T8:\[\s*\d+,\s*\d+\] T9:\[\s*\d+,\s*\d+\]')
