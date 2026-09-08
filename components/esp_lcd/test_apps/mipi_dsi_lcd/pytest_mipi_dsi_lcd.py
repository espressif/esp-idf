# SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'cache_safe',
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_dsi_lcd(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config, skip_autoflash',
    [
        ('virt_flash_enc', 'y'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_dsi_lcd_with_virt_flash_enc(dut: Dut) -> None:
    print(' - Erase flash')
    dut.serial.erase_flash()

    print(' - Start app (flash partition_table and app)')
    dut.serial.write_flash_no_enc()
    dut.expect('Loading virtual efuse blocks from real efuses')
    dut.expect('Checking flash encryption...')
    dut.expect('Generating new flash encryption key...')

    dut.run_all_single_board_cases()
