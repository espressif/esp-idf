# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
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
def test_rgb_lcd_lvgl_esp32s3(dut: Dut) -> None:
    dut.expect_exact('example: Turn off LCD backlight')
    dut.expect_exact('example: Install RGB LCD panel driver')
    dut.expect_exact('example: Initialize RGB LCD panel')
    dut.expect_exact('example: Turn on LCD backlight')
    dut.expect_exact('example: Initialize LVGL library')
    dut.expect_exact('example: Install LVGL tick timer')
    dut.expect_exact('example: Create LVGL task')
    dut.expect_exact('example: Display LVGL UI')


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'single_fb_with_bb',
        'single_fb_no_bb',
        'double_fb',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_rgb_lcd_lvgl_esp32p4(dut: Dut) -> None:
    dut.expect_exact('example: Turn off LCD backlight')
    dut.expect_exact('example: Install RGB LCD panel driver')
    dut.expect_exact('example: Initialize RGB LCD panel')
    dut.expect_exact('example: Turn on LCD backlight')
    dut.expect_exact('example: Initialize LVGL library')
    dut.expect_exact('example: Install LVGL tick timer')
    dut.expect_exact('example: Create LVGL task')
    dut.expect_exact('example: Display LVGL UI')
