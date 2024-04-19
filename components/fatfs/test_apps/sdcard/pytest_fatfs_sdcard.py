# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.sdcard_sdmode
@pytest.mark.parametrize(
    'config',
    [
        'default',
        'release',
    ]
)
def test_fatfs_sdcard_generic_sdmmc(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='sdmmc', timeout=180)


@pytest.mark.esp32
@pytest.mark.temp_skip_ci(targets=['esp32'], reason='IDFCI-2058, temporary lack runner')
@pytest.mark.esp32c3
@pytest.mark.sdcard_spimode
@pytest.mark.parametrize(
    'config',
    [
        'default',
        'release',
    ]
)
def test_fatfs_sdcard_generic_sdspi(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='sdspi', timeout=180)


@pytest.mark.esp32
@pytest.mark.sdcard_sdmode
@pytest.mark.psram
@pytest.mark.parametrize(
    'config',
    [
        'psram',
    ]
)
def test_fatfs_sdcard_psram_sdmmc(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='sdmmc', timeout=180)


@pytest.mark.esp32
@pytest.mark.temp_skip_ci(targets=['esp32'], reason='IDFCI-2058, temporary lack runner')
@pytest.mark.sdcard_spimode
@pytest.mark.psram
@pytest.mark.parametrize(
    'config',
    [
        'psram',
    ]
)
def test_fatfs_sdcard_psram_sdspi(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='sdspi', timeout=180)
