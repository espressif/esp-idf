# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
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
def test_esp_flash(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='esp_flash')


@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c2
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'rom_impl',
    ],
    indirect=True,
)
def test_esp_flash_rom(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='esp_flash')


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c2
@pytest.mark.flash_multi
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
def test_esp_flash_multi(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='esp_flash_multi', timeout=120)


@pytest.mark.esp32c2
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
def test_esp_flash_26mhz_c2(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='esp_flash')
