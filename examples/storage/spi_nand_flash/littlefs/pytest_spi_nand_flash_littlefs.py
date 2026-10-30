# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.spi_nand_flash
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_spi_nand_flash_littlefs(dut: Dut) -> None:
    dut.expect_exact('Opening file', timeout=60)
    dut.expect_exact('File written')
    dut.expect_exact('Reading file')
    dut.expect("Read from file: 'Written using ESP-IDF")
    dut.expect_exact('Done')
    dut.expect_exact('Returned from app_main')
