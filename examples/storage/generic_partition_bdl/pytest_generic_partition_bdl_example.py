# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


def _expect_partition_bdl_flow(dut: Dut) -> None:
    # The partitions are enumerated in on-disk order: LittleFS (MBR entry 0) comes
    # before FATFS (MBR entry 1), so the mount/read-back logs appear in that order.
    dut.expect('example: Writing MBR partition table to the whole disk', timeout=90)
    dut.expect('example: Reading MBR partition table back', timeout=90)
    dut.expect('example: Mounting LittleFS on the LittleFS partition BDL', timeout=90)
    dut.expect(
        "example: Read back from /littlefs/hello.txt: 'Hello from LittleFS over a generic-partition BDL!'",
        timeout=90,
    )
    dut.expect('example: Mounting FATFS on the FAT partition BDL', timeout=90)
    dut.expect("example: Read back from /fat/hello.txt: 'Hello from FATFS over a generic-partition BDL!'", timeout=90)
    dut.expect('example: Done', timeout=90)


@pytest.mark.generic
@pytest.mark.parametrize('config', ['spiflash'], indirect=True)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_examples_generic_partition_bdl_spiflash(dut: Dut) -> None:
    _expect_partition_bdl_flow(dut)


# The SPI-flash configuration needs no external peripherals, so it can also run
# under QEMU (which emulates SPI flash but has no SD host). The SD card configs
# below require real hardware and therefore have no QEMU variant.
@pytest.mark.qemu
@pytest.mark.parametrize('config', ['spiflash'], indirect=True)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_examples_generic_partition_bdl_spiflash_qemu(dut: Dut) -> None:
    _expect_partition_bdl_flow(dut)


@pytest.mark.sdcard_sdmode
@pytest.mark.parametrize('config', ['sdmmc'], indirect=True)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_examples_generic_partition_bdl_sdmmc(dut: Dut) -> None:
    _expect_partition_bdl_flow(dut)


@pytest.mark.sdcard_spimode
@pytest.mark.parametrize('config', ['sdspi'], indirect=True)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_examples_generic_partition_bdl_sdspi(dut: Dut) -> None:
    _expect_partition_bdl_flow(dut)
