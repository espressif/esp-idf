# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.octal_psram
@pytest.mark.parametrize(
    'config',
    [
        'single_fb_with_bb',
        'single_fb_no_bb',
        'double_fb',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_rgb_lcd_lvgl(dut: Dut) -> None:
    dut.expect_exact('example: Turn off LCD backlight')
    dut.expect_exact('example: Install RGB LCD panel driver')
    dut.expect_exact('example: Register event callbacks')
    dut.expect_exact('example: Initialize RGB LCD panel')
    dut.expect_exact('example: Turn on LCD backlight')
    dut.expect_exact('example: Initialize LVGL library')
    dut.expect_exact('example: Register display driver to LVGL')
    dut.expect_exact('example: Install LVGL tick timer')
    dut.expect_exact('example: Display LVGL Scatter Chart')
