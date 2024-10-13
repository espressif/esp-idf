# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32c6
@pytest.mark.generic
def test_simple_rgb_led_matrix_example(dut: Dut) -> None:
    dut.expect_exact('example: Install fast GPIO bundle for line address control')
    dut.expect_exact('example: Install parallel IO TX unit')
    dut.expect_exact('example: Initialize LVGL library')
    dut.expect_exact('example: Register display driver to LVGL')
    dut.expect_exact('example: Install LVGL tick timer')
    dut.expect_exact('example: Display LVGL UI')
