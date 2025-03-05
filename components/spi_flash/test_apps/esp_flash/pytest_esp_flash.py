# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
        'flash_qio',
        'verify',
        'special',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_esp_flash(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='esp_flash')


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'rom_impl',
    ],
    indirect=True,
)
@idf_parametrize(
    'target', ['esp32s3', 'esp32c3', 'esp32c2', 'esp32c6', 'esp32h2', 'esp32c5', 'esp32c61'], indirect=['target']
)
def test_esp_flash_rom(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='esp_flash')


@pytest.mark.flash_multi
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c2'], indirect=['target'])
def test_esp_flash_multi(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='esp_flash_multi', timeout=120)


@pytest.mark.generic
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'config, baud',
    [
        ('esp32c2_xtal26m', '74880'),
        ('esp32c2_xtal26m_rom', '74880'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_esp_flash_26mhz_c2(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='esp_flash')
