# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_touch_element_waterproof(dut: Dut) -> None:
    dut.expect_exact('Touch Element Waterproof Example: Touch Element library install')
    dut.expect_exact('Touch Element Waterproof Example: Touch Element waterproof install')
    dut.expect_exact('Touch Element Waterproof Example: Touch button install')
    dut.expect_exact('Touch Element Waterproof Example: Touch buttons create')
