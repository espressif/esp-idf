# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_examples_fatfs_bdl_wl(dut: Dut) -> None:
    # CI boards reuse the 'storage' FAT partition across tests; stale WL/FAT
    # metadata can mount but fail on the first write. So first erase the partition
    dut.serial.erase_partition('storage')
    dut.serial.hard_reset()

    dut.expect('example: Mounting FAT filesystem via BDL', timeout=90)
    dut.expect('example: Filesystem mounted', timeout=90)
    dut.expect('example: Opening file', timeout=90)
    dut.expect('example: File written', timeout=90)
    dut.expect('example: Reading file', timeout=90)
    dut.expect("example: Read from file: 'Hello from FatFS over BDL!'", timeout=90)
    dut.expect('example: Unmounting FAT filesystem', timeout=90)
    dut.expect('example: Done', timeout=90)
