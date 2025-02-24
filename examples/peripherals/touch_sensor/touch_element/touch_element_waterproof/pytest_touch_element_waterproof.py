# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32s2', 'esp32s3'], indirect=['target'])
def test_touch_element_waterproof(dut: Dut) -> None:
    dut.expect_exact('Touch Element Waterproof Example: Touch Element library install')
    dut.expect_exact('Touch Element Waterproof Example: Touch Element waterproof install')
    dut.expect_exact('Touch Element Waterproof Example: Touch button install')
    dut.expect_exact('Touch Element Waterproof Example: Touch buttons create')
