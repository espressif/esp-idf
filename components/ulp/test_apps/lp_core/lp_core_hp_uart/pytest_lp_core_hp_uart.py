# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32c5
@pytest.mark.esp32c6
@pytest.mark.esp32p4
@pytest.mark.generic
def test_lp_core_hp_uart_print(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"lp-print can output to hp-uart"')
    dut.expect_exact('Hello, World!')
    dut.expect_exact('1234abcd')
    dut.expect_exact('56')


@pytest.mark.esp32c5
@pytest.mark.esp32c6
@pytest.mark.esp32p4
@pytest.mark.generic
def test_lp_core_panic(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"LP-Core panic"')

    dut.expect_exact("Guru Meditation Error: LP Core panic'ed Breakpoint")
    dut.expect_exact('Core 0 register dump:')
    dut.expect_exact('ELF file SHA256:')


@pytest.mark.esp32c5
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


@pytest.mark.esp32p4
@pytest.mark.generic
def test_lp_core_lp_rom(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"LP-Core LP-ROM"')
    dut.expect_exact('ULP: all tests passed')
    dut.expect_exact('LP ROM test passed')
