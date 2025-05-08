# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.nightly_run
@pytest.mark.parametrize(
    'config',
    [
        'no_poisoning',
        'light_poisoning',
        'comprehensive_poisoning',
    ],
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_heap_poisoning(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.parametrize(
    'config, embedded_services',
    [('no_poisoning', 'idf,qemu'), ('light_poisoning', 'idf,qemu'), ('comprehensive_poisoning', 'idf,qemu')],
)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_heap_poisoning_qemu(dut: Dut) -> None:
    for case in dut.test_menu:
        if 'qemu-ignore' not in case.groups and not case.is_ignored and case.type == 'normal':
            dut._run_normal_case(case)


@pytest.mark.generic
@pytest.mark.parametrize('config', ['in_flash'])
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_heap_in_flash(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'target',
    [
        'esp32',
        'esp32s2',
        'esp32s3',
    ],
)
@pytest.mark.parametrize('config', ['psram', 'psram_all_ext'])
def test_heap(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='psram')


@pytest.mark.generic
@pytest.mark.parametrize(
    'target',
    [
        'esp32',
    ],
)
@pytest.mark.parametrize('config', ['misc_options'])
def test_heap_misc_options(dut: Dut) -> None:
    dut.run_all_single_board_cases(name=['IRAM_8BIT capability test', 'test allocation and free function hooks'])

    dut.expect_exact("Enter next test, or 'enter' to see menu")
    dut.write('"When enabled, allocation operation failure generates an abort"')
    dut.expect('Backtrace: ')


@pytest.mark.generic
@idf_parametrize(
    'config,target',
    [
        ('heap_trace_esp32', 'esp32'),
        ('heap_trace_hashmap_esp32', 'esp32'),
        ('heap_trace_esp32c3', 'esp32c3'),
        ('heap_trace_hashmap_esp32c3', 'esp32c3'),
    ],
    indirect=['config', 'target'],
)
def test_heap_trace_dump(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='trace-dump&internal')
    dut.run_all_single_board_cases(group='trace-dump&external')
    dut.run_all_single_board_cases(group='trace-dump&all')
    dut.run_all_single_board_cases(group='heap-trace')


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32c61'], reason='support TBD')  # TODO [ESP32C61] IDF-9858 IDF-10989
@pytest.mark.parametrize('config', ['mem_prot'])
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_memory_protection(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='heap&mem_prot', timeout=300)
