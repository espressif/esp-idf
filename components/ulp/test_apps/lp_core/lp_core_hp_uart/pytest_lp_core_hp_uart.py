# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32c6
@pytest.mark.esp32p4
@pytest.mark.generic
def test_lp_core_hp_uart_print(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"lp-print can output to hp-uart"')
    dut.expect_exact('Hello, World!')


@pytest.mark.esp32c6
@pytest.mark.esp32p4
@pytest.mark.generic
def test_lp_core_shared_mem(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"LP-Core Shared-mem"')

    result = dut.expect(r'HP shared memory address: (0x[0-9a-fA-F]+)')
    hp_addr = result[1]

    result = dut.expect(r'ULP shared memory address: (0x[0-9a-fA-F]+)')
    ulp_addr = result[1]

    assert ulp_addr == hp_addr

    dut.expect_exact('ULP shared memory test passed')
    dut.expect_exact('HP shared memory test passed')
