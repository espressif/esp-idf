# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_touch_pad_read_v2(dut: Dut) -> None:
    dut.expect_exact('touch read: Denoise function init')
    dut.expect(r'T1: \[\d+\] T2: \[\d+\] T3: \[\d+\] T4: \[\d+\] T5: \[\d+\] T6: \[\d+\] T7: \[\d+\] T8: \[\d+\] T9: \[\d+\]'
               r' T10: \[\d+\] T11: \[\d+\] T12: \[\d+\] T13: \[\d+\] T14: \[\d+\]')
