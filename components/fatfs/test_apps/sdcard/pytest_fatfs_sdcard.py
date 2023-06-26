# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.sdcard_sdmode
@pytest.mark.parametrize(
    'config',
    [
        'default',
        'release',
    ]
)
def test_fatfs_sdcard_generic_sdmmc(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('')
    dut.expect_exact('Enter test for running.')
    dut.write('[sdmmc]')
    dut.expect_unity_test_output(timeout=180)


@pytest.mark.esp32
@pytest.mark.esp32s2
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
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('')
    dut.expect_exact('Enter test for running.')
    dut.write('[sdspi]')
    dut.expect_unity_test_output(timeout=180)


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
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('')
    dut.expect_exact('Enter test for running.')
    dut.write('[sdmmc]')
    dut.expect_unity_test_output(timeout=180)


@pytest.mark.esp32
@pytest.mark.sdcard_spimode
@pytest.mark.psram
@pytest.mark.parametrize(
    'config',
    [
        'psram',
    ]
)
def test_fatfs_sdcard_psram_sdspi(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('')
    dut.expect_exact('Enter test for running.')
    dut.write('[sdspi]')
    dut.expect_unity_test_output(timeout=180)
