# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_advanced_rgb_led_matrix_example(dut: Dut) -> None:
    dut.expect_exact('example: Install parallel IO TX unit')
    dut.expect_exact('example: Initialize LVGL library')
    dut.expect_exact('example: Install LVGL tick timer')
    dut.expect_exact('example: Display LVGL UI')
