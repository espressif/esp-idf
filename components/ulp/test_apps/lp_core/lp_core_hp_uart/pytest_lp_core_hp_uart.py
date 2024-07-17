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
