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
    ],
    indirect=True,
)
def test_esp_flash_stress(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='esp_flash')


@pytest.mark.esp32s3
@pytest.mark.MSPI_F8R8
@pytest.mark.parametrize(
    'config',
    [
        'esp32s3_f8r8',
    ],
    indirect=True,
)
def test_esp_flash_stress_f8r8(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'esp32s3_rom_xip_psram',
    ],
    indirect=True,
)
def test_esp_flash_stress_rom_xip_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32c3
@pytest.mark.flash_suspend
@pytest.mark.parametrize(
    'config',
    [
        'esp32c3_suspend',
    ],
    indirect=True,
)
def test_flash_auto_suspend_stress(dut: Dut) -> None:
    dut.run_all_single_board_cases()
