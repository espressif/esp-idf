# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import re
from pprint import pformat
from typing import List, Optional

import pytest

from conftest import PanicTestDut

CONFIGS = [
    pytest.param('coredump_flash_bin_crc', marks=[pytest.mark.esp32, pytest.mark.esp32s2]),
    pytest.param('coredump_flash_elf_sha', marks=[pytest.mark.esp32]),  # sha256 only supported on esp32
    pytest.param('coredump_uart_bin_crc', marks=[pytest.mark.esp32, pytest.mark.esp32s2]),
    pytest.param('coredump_uart_elf_crc', marks=[pytest.mark.esp32, pytest.mark.esp32s2]),
    pytest.param('gdbstub', marks=[pytest.mark.esp32, pytest.mark.esp32s2]),
    pytest.param('panic', marks=[pytest.mark.esp32, pytest.mark.esp32s2]),
]

# An ESP32-only config, used for tests requiring two cores
CONFIGS_ESP32 = [
    pytest.param('coredump_flash_bin_crc', marks=[pytest.mark.esp32]),
    pytest.param('coredump_flash_elf_sha', marks=[pytest.mark.esp32]),
    pytest.param('coredump_uart_bin_crc', marks=[pytest.mark.esp32]),
    pytest.param('coredump_uart_elf_crc', marks=[pytest.mark.esp32]),
    pytest.param('gdbstub', marks=[pytest.mark.esp32]),
    pytest.param('panic', marks=[pytest.mark.esp32]),
]


def get_default_backtrace(config: str) -> List[str]:
    return [config, 'app_main', 'main_task', 'vPortTaskWrapper']


def common_test(dut: PanicTestDut, config: str, expected_backtrace: Optional[List[str]] = None) -> None:
    if 'gdbstub' in config:
        dut.expect_exact('Entering gdb stub now.')
        dut.start_gdb()
        frames = dut.gdb_backtrace()
        # Make sure frames and the expected_backtrace have the same size, else, an exception will occur
        if expected_backtrace is not None:
            size = min(len(frames), len(expected_backtrace))
            frames = frames[0:size]
            expected_backtrace = expected_backtrace[0:size]
            if not dut.match_backtrace(frames, expected_backtrace):
                raise AssertionError(
                    'Unexpected backtrace in test {}:\n{}'.format(config, pformat(frames))
                )
            dut.revert_log_level()
        return

    if 'uart' in config:
        dut.process_coredump_uart()
    elif 'flash' in config:
        dut.process_coredump_flash()
    elif 'panic' in config:
        pass

    dut.expect('Rebooting...')


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_task_wdt_cpu0(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.expect_test_func_name(test_func_name)
    dut.expect_exact(
        'Task watchdog got triggered. The following tasks/users did not reset the watchdog in time:'
    )
    dut.expect_exact('CPU 0: main')
    dut.expect_none('register dump:')
    dut.expect_exact('Print CPU 0 (current core) backtrace')
    dut.expect_backtrace()
    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')

    if config == 'gdbstub':
        common_test(
            dut,
            config,
            expected_backtrace=[
                'test_task_wdt_cpu0',
                'app_main'
            ],
        )
    else:
        common_test(dut, config)


@pytest.mark.parametrize('config', CONFIGS_ESP32, indirect=True)
@pytest.mark.generic
def test_task_wdt_cpu1(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.expect_test_func_name(test_func_name)
    dut.expect_exact(
        'Task watchdog got triggered. The following tasks/users did not reset the watchdog in time:'
    )
    dut.expect_exact('CPU 1: Infinite loop')
    dut.expect_none('register dump:')
    dut.expect_exact('Print CPU 1 backtrace')
    dut.expect_backtrace()
    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')

    if config == 'gdbstub':
        common_test(
            dut,
            config,
            expected_backtrace=[
                'infinite_loop'
            ],
        )
    else:
        common_test(dut, config)


@pytest.mark.parametrize('config', CONFIGS_ESP32, indirect=True)
@pytest.mark.generic
def test_task_wdt_both_cpus(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.expect_test_func_name(test_func_name)
    dut.expect_exact(
        'Task watchdog got triggered. The following tasks/users did not reset the watchdog in time:'
    )
    dut.expect_exact('CPU 0: Infinite loop')
    dut.expect_exact('CPU 1: Infinite loop')
    dut.expect_none('register dump:')
    dut.expect_exact('Print CPU 0 (current core) backtrace')
    dut.expect_backtrace()
    dut.expect_exact('Print CPU 1 backtrace')
    dut.expect_backtrace()
    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')

    if config == 'gdbstub':
        common_test(
            dut,
            config,
            expected_backtrace=[
                'infinite_loop'
            ],
        )
    else:
        common_test(dut, config)


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_int_wdt(
    dut: PanicTestDut, target: str, config: str, test_func_name: str
) -> None:
    dut.expect_test_func_name(test_func_name)
    dut.expect_gme('Interrupt wdt timeout on CPU0')
    dut.expect_reg_dump(0)
    dut.expect_backtrace()
    if target == 'esp32s2':
        dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')

    if target != 'esp32s2':  # esp32s2 is single-core
        dut.expect_reg_dump(1)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation')

    common_test(dut, config, expected_backtrace=get_default_backtrace(test_func_name))


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_int_wdt_cache_disabled(
    dut: PanicTestDut, target: str, config: str, test_func_name: str
) -> None:
    dut.expect_test_func_name(test_func_name)
    dut.expect_gme('Interrupt wdt timeout on CPU0')
    dut.expect_reg_dump(0)
    dut.expect_backtrace()
    if target == 'esp32s2':
        dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')

    if target != 'esp32s2':  # esp32s2 is single-core
        dut.expect_reg_dump(1)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation')

    common_test(dut, config, expected_backtrace=get_default_backtrace(test_func_name))


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.xfail('config.getvalue("target") == "esp32s2"', reason='raised IllegalInstruction instead')
@pytest.mark.generic
def test_cache_error(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.expect_test_func_name(test_func_name)
    dut.expect_gme('Cache disabled but cached memory region accessed')
    dut.expect_reg_dump(0)
    dut.expect_backtrace()
    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')
    common_test(
        dut, config, expected_backtrace=['die'] + get_default_backtrace(test_func_name)
    )


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_stack_overflow(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.expect_test_func_name(test_func_name)
    dut.expect_gme('Unhandled debug exception')
    dut.expect_exact('Stack canary watchpoint triggered (main)')
    dut.expect_reg_dump(0)
    dut.expect_backtrace()
    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')
    common_test(dut, config, expected_backtrace=get_default_backtrace(test_func_name))


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_instr_fetch_prohibited(
    dut: PanicTestDut, config: str, test_func_name: str
) -> None:
    dut.expect_test_func_name(test_func_name)
    dut.expect_gme('InstrFetchProhibited')
    dut.expect_reg_dump(0)
    dut.expect_backtrace()
    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')
    common_test(
        dut,
        config,
        expected_backtrace=['_init'] + get_default_backtrace(test_func_name),
    )


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_illegal_instruction(
    dut: PanicTestDut, config: str, test_func_name: str
) -> None:
    dut.expect_test_func_name(test_func_name)
    dut.expect_gme('IllegalInstruction')
    dut.expect_reg_dump(0)
    dut.expect_backtrace()
    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')
    common_test(dut, config, expected_backtrace=get_default_backtrace(test_func_name))


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_storeprohibited(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.expect_test_func_name(test_func_name)
    dut.expect_gme('StoreProhibited')
    dut.expect_reg_dump(0)
    dut.expect_backtrace()
    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')
    common_test(dut, config, expected_backtrace=get_default_backtrace(test_func_name))


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_abort(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.expect_test_func_name(test_func_name)
    dut.expect(r'abort\(\) was called at PC [0-9xa-f]+ on core 0')
    dut.expect_backtrace()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])

    if config == 'gdbstub':
        common_test(
            dut,
            config,
            expected_backtrace=[
                'panic_abort',
                'esp_system_abort',
                'abort'
            ] + get_default_backtrace(test_func_name),
        )
    else:
        common_test(dut, config)


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_ub(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.expect_test_func_name(test_func_name)
    dut.expect('Undefined behavior of type out_of_bounds')
    dut.expect_backtrace()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])

    if config == 'gdbstub':
        common_test(
            dut,
            config,
            expected_backtrace=[
                'panic_abort',
                'esp_system_abort',
                '__ubsan_default_handler',
                '__ubsan_handle_out_of_bounds'
            ] + get_default_backtrace(test_func_name),
        )
    else:
        common_test(dut, config)


#########################
# for config panic only #
#########################
@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.xfail('config.getvalue("target") == "esp32s2"', reason='raised IllegalInstruction instead')
@pytest.mark.parametrize('config', ['panic'], indirect=True)
@pytest.mark.generic
def test_abort_cache_disabled(
    dut: PanicTestDut, config: str, test_func_name: str
) -> None:
    dut.expect_test_func_name(test_func_name)
    dut.expect(r'abort\(\) was called at PC [0-9xa-f]+ on core 0')
    dut.expect_backtrace()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])
    common_test(dut, config, expected_backtrace=get_default_backtrace(test_func_name))


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.parametrize('config', ['panic'], indirect=True)
@pytest.mark.generic
def test_assert(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.expect_test_func_name(test_func_name)
    dut.expect(
        re.compile(
            rb'assert failed:[\s\w()]*?\s[.\w/]*\.(?:c|cpp|h|hpp):\d.*$', re.MULTILINE
        )
    )
    dut.expect_backtrace()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])
    common_test(dut, config, expected_backtrace=get_default_backtrace(test_func_name))


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.xfail('config.getvalue("target") == "esp32s2"', reason='raised IllegalInstruction instead')
@pytest.mark.parametrize('config', ['panic'], indirect=True)
@pytest.mark.generic
def test_assert_cache_disabled(
    dut: PanicTestDut, config: str, test_func_name: str
) -> None:
    dut.expect_test_func_name(test_func_name)
    dut.expect(re.compile(rb'assert failed: [0-9xa-fA-F]+.*$', re.MULTILINE))
    dut.expect_backtrace()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])
    common_test(dut, config, expected_backtrace=get_default_backtrace(test_func_name))
