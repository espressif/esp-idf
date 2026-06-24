# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'image_in_bin',
        'image_in_fs',
    ],
    indirect=True,
)
@idf_parametrize('target', soc_filtered_targets('SOC_LCD_I80_SUPPORTED == 1'), indirect=['target'])
def test_i80_lcd_lvgl(dut: Dut) -> None:
    dut.expect_exact('Calling app_main()')
    dut.expect_exact('example: Initialize Intel 8080 bus')
    dut.expect_exact('example: Initialize LVGL library')
    dut.expect_exact('example: Install LVGL tick timer')
    dut.expect_exact('example: Create LVGL task')
    dut.expect_exact('example: Starting LVGL task')
    dut.expect_exact('example: Display LVGL animation')
