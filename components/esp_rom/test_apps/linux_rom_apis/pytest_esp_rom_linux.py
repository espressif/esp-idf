# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@idf_parametrize('target', ['linux'], indirect=['target'])
def test_esp_rom_linux(dut: Dut) -> None:
    dut.expect_exact('All tests passed', timeout=5)
