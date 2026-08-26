# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.octal_psram
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_rgb_lcd_esp32s3(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_LCD_RGB_SUPPORTED == 1 and IDF_TARGET not in ["esp32s3"]'),
    indirect=['target'],
)
def test_rgb_lcd(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.flash_encryption_f4r8
@pytest.mark.parametrize(
    'config',
    [
        'flash_enc',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_rgb_lcd_with_flash_encryption_esp32s3_f4r8(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.flash_encryption
@pytest.mark.parametrize(
    'config',
    [
        'flash_enc',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    soc_filtered_targets(
        'SOC_LCD_RGB_SUPPORTED == 1 and SOC_FLASH_ENC_SUPPORTED == 1 and IDF_TARGET not in ["esp32s3"]'
    ),
    indirect=['target'],
)
def test_rgb_lcd_with_flash_encryption(dut: Dut) -> None:
    dut.run_all_single_board_cases()
