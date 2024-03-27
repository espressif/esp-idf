# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.supported_targets
@pytest.mark.nightly_run
@pytest.mark.parametrize(
    'config',
    [
        'no_poisoning',
        'light_poisoning',
        'comprehensive_poisoning',
    ]
)
def test_heap_poisoning(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.parametrize(
    'config, embedded_services',
    [
        ('no_poisoning', 'idf,qemu'),
        ('light_poisoning', 'idf,qemu'),
        ('comprehensive_poisoning', 'idf,qemu')
    ]
)
def test_heap_poisoning_qemu(dut: Dut) -> None:
    for case in dut.test_menu:
        if 'qemu-ignore' not in case.groups and not case.is_ignored and case.type == 'normal':
            dut._run_normal_case(case)


@pytest.mark.generic
@pytest.mark.supported_targets
@pytest.mark.parametrize(
    'config',
    [
        'in_flash'
    ]
)
def test_heap_in_flash(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.parametrize(
    'config',
    [
        'psram',
        'psram_all_ext'
    ]
)
def test_heap(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.parametrize(
    'config',
    [
        'misc_options'
    ]
)
def test_heap_misc_options(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"IRAM_8BIT capability test"')
    dut.expect_unity_test_output()

    dut.expect_exact("Enter next test, or 'enter' to see menu")
    dut.write('"test allocation and free function hooks"')
    dut.expect_unity_test_output()

    dut.expect_exact("Enter next test, or 'enter' to see menu")
    dut.write('"When enabled, allocation operation failure generates an abort"')
    dut.expect('Backtrace: ')


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.parametrize(
    'config',
    [
        'heap_trace',
        'heap_trace_hashmap'
    ]
)
def test_heap_trace_dump(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[trace-dump][internal]')
    dut.expect('Internal')

    dut.expect_exact('Enter next test, or \'enter\' to see menu')
    dut.write('[trace-dump][external]')
    dut.expect('PSRAM')

    dut.expect_exact('Enter next test, or \'enter\' to see menu')
    dut.write('[trace-dump][all]')
    dut.expect('Internal')
    dut.expect('PSRAM')

    dut.expect_exact('Enter next test, or \'enter\' to see menu')
    dut.write('[heap-trace]')
    dut.expect_unity_test_output(timeout=100)


@pytest.mark.generic
@pytest.mark.supported_targets
@pytest.mark.parametrize(
    'config',
    [
        'mem_prot'
    ]
)
def test_memory_protection(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[heap][mem_prot]')
    dut.expect_unity_test_output(timeout=300)
