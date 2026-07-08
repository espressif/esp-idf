# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
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
    'target',
    ['esp32s3', 'esp32c3', 'esp32c2', 'esp32c6', 'esp32h2', 'esp32h21', 'esp32c5', 'esp32c61'],
    indirect=['target'],
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
        ('c2_xtal26m', '74880'),
        ('c2_xtal26m_rom', '74880'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_esp_flash_26mhz_c2(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='esp_flash')


@pytest.mark.parametrize(
    'config',
    [
        '32m_120m_qio_map',
        '32m_120m_dio_map',
        '32m_120m_qio',
        '32m_120m_dio',
    ],
    indirect=True,
)
@idf_parametrize(
    'target,markers',
    [
        ('esp32p4', (pytest.mark.flash_32m,)),
        ('esp32c5', (pytest.mark.flash_32m,)),
        ('esp32c61', (pytest.mark.flash_32m,)),
        ('esp32s31', (pytest.mark.flash_32m,)),
    ],
    indirect=['target'],
)
def test_esp_flash_32m(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='esp_flash')
