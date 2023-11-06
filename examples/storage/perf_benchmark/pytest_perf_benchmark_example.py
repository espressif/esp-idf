# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0


import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'spiflash'
    ],
    indirect=True
)
def test_examples_perf_benchmark_spiflash(dut: Dut) -> None:
    # SPI flash
    dut.expect('example: Mountig WL layer...', timeout=10)
    dut.expect('example: WL layer mounted', timeout=90)
    dut.expect('example: WL layer unmounted', timeout=240)  # SPI flash has slow write speed

    dut.expect('example: Mounting FATFS partition...', timeout=10)
    dut.expect('example: FATFS mounted to', timeout=90)  # Increased timeout due to formatting
    dut.expect('example: FATFS partition unmounted', timeout=240)  # SPI flash has slow write speed

    dut.expect('example: Mounting SPIFFS partition...', timeout=10)
    dut.expect('example: SPIFFS mounted to', timeout=90)  # Increased timeout due to formatting
    dut.expect('example: SPIFFS partition unmounted', timeout=240)  # SPI flash has slow write speed


@pytest.mark.esp32
@pytest.mark.sdcard_sdmode
@pytest.mark.parametrize(
    'config',
    [
        'sdmmc_1line',
        'sdmmc_4line',
    ],
    indirect=True,
)
def test_examples_perf_benchmark_sdcard_sdmmc(dut: Dut) -> None:
    # SD card
    dut.expect('example: Mounting SD card - raw access', timeout=10)
    dut.expect('example: SD card mounted - raw access', timeout=10)
    dut.expect('example: SD card unmounted - raw access', timeout=60)

    dut.expect('example: Mounting SD card - FATFS', timeout=30)
    dut.expect('example: SD card mounted - FATFS', timeout=120)  # Increased timeout due to formatting
    dut.expect('example: SD card unmounted - FATFS', timeout=180)


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32s2
@pytest.mark.sdcard_spimode
@pytest.mark.parametrize(
    'config',
    [
        'sdspi_1line',
    ],
    indirect=True,
)
def test_examples_perf_benchmark_sdcard_spi(dut: Dut) -> None:
    # SD card
    dut.expect('example: Mounting SD card - raw access', timeout=10)
    dut.expect('example: SD card mounted - raw access', timeout=10)
    dut.expect('example: SD card unmounted - raw access', timeout=180)

    dut.expect('example: Mounting SD card - FATFS', timeout=30)
    dut.expect('example: SD card mounted - FATFS', timeout=120)  # Increased timeout due to formatting
    dut.expect('example: SD card unmounted - FATFS', timeout=180)
