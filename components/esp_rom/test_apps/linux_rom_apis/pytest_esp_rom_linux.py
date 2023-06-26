# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.linux
@pytest.mark.host_test
def test_esp_rom_linux(dut: Dut) -> None:
    dut.expect_exact('All tests passed', timeout=5)
