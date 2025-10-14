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
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_esp_flash_stress(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='esp_flash')


@pytest.mark.MSPI_F8R8
@pytest.mark.parametrize(
    'config',
    [
        'esp32s3_f8r8',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_esp_flash_stress_f8r8(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'esp32s3_rom_xip_psram',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_esp_flash_stress_rom_xip_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.flash_suspend
@pytest.mark.parametrize(
    'config',
    [
        'esp32c3_suspend',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_flash_auto_suspend_stress(dut: Dut) -> None:
    dut.run_all_single_board_cases()
