# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.sdcard_sdmode
@pytest.mark.parametrize(
    'config',
    [
        'default',
        'release',
    ],
)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_fatfs_sdcard_generic_sdmmc(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='sdmmc', timeout=180)


@pytest.mark.temp_skip_ci(targets=['esp32'], reason='IDFCI-2058, temporary lack runner')
@pytest.mark.sdcard_spimode
@pytest.mark.parametrize(
    'config',
    [
        'default',
        'release',
    ],
)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_fatfs_sdcard_generic_sdspi(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='sdspi', timeout=180)


@pytest.mark.sdcard_sdmode
@pytest.mark.psram
@pytest.mark.parametrize(
    'config',
    [
        'psram',
    ],
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_fatfs_sdcard_psram_sdmmc(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='sdmmc', timeout=180)


@pytest.mark.temp_skip_ci(targets=['esp32'], reason='IDFCI-2058, temporary lack runner')
@pytest.mark.sdcard_spimode
@pytest.mark.psram
@pytest.mark.parametrize(
    'config',
    [
        'psram',
    ],
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_fatfs_sdcard_psram_sdspi(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='sdspi', timeout=180)
