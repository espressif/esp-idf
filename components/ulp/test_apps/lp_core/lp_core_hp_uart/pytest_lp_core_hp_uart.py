# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_LP_CORE_SUPPORTED == 1'), indirect=['target'])
def test_lp_core_hp_uart_print(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"lp-print can output to hp-uart"')
    dut.expect_exact('Hello, World!')
    dut.expect_exact('1234abcd')
    dut.expect_exact('56')


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_LP_CORE_SUPPORTED == 1'), indirect=['target'])
def test_lp_core_panic(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"LP-Core panic"')

    dut.expect_exact("Guru Meditation Error: LP Core panic'ed Breakpoint")
    dut.expect_exact('Core 0 register dump:')
    dut.expect_exact('ELF file SHA256:')


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_LP_CORE_SUPPORTED == 1'), indirect=['target'])
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


@pytest.mark.generic
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_lp_core_lp_rom(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"LP-Core LP-ROM"')
    dut.expect_exact('ULP: all tests passed')
    dut.expect_exact('LP ROM test passed')


_PMP_TARGETS = soc_filtered_targets('SOC_LP_CORE_HAS_PMP == 1')


@pytest.mark.generic
@idf_parametrize('target', _PMP_TARGETS, indirect=['target'])
def test_lp_core_pmp_positive(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"LP core PMP: normal operation succeeds"')
    dut.expect_exact('LP core PMP: normal operation succeeds:PASS')


@pytest.mark.generic
@idf_parametrize('target', _PMP_TARGETS, indirect=['target'])
def test_lp_core_pmp_write_to_text(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"LP core PMP: write to .text triggers Store access fault"')
    dut.expect_exact("Guru Meditation Error: LP Core panic'ed Store access fault")


@pytest.mark.generic
@idf_parametrize('target', _PMP_TARGETS, indirect=['target'])
def test_lp_core_pmp_exec_from_data(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"LP core PMP: execute from .data triggers Instruction access fault"')
    dut.expect_exact("Guru Meditation Error: LP Core panic'ed Instruction access fault")


@pytest.mark.generic
@idf_parametrize('target', _PMP_TARGETS, indirect=['target'])
def test_lp_core_pmp_access_unmapped(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"LP core PMP: access unmapped region triggers Load access fault"')
    dut.expect_exact("Guru Meditation Error: LP Core panic'ed Load access fault")
