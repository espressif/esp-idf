# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import re
from typing import List, Optional

import pexpect
import pytest
from test_panic_util import PanicTestDut

# Markers for all the targets this test currently runs on
TARGETS_TESTED = [pytest.mark.esp32, pytest.mark.esp32s2, pytest.mark.esp32c3, pytest.mark.esp32s3, pytest.mark.esp32c2]

# Most tests run on all targets and with all configs.
# This list is passed to @pytest.mark.parametrize for each of the test cases.
# It creates an outer product of the sets: [configs] x [targets],
# with some exceptions.
CONFIGS = [
    pytest.param('coredump_flash_bin_crc', marks=TARGETS_TESTED),
    pytest.param('coredump_flash_elf_sha', marks=[pytest.mark.esp32]),  # sha256 only supported on esp32, IDF-1820
    pytest.param('coredump_uart_bin_crc', marks=TARGETS_TESTED),
    pytest.param('coredump_uart_elf_crc', marks=TARGETS_TESTED),
    pytest.param('gdbstub', marks=TARGETS_TESTED),
    pytest.param('panic', marks=TARGETS_TESTED),
]

# Some tests only run on dual-core targets, they use the config below.
TARGETS_DUAL_CORE = [pytest.mark.esp32, pytest.mark.esp32s3]
CONFIGS_DUAL_CORE = [
    pytest.param('coredump_flash_bin_crc', marks=TARGETS_DUAL_CORE),
    pytest.param('coredump_flash_elf_sha', marks=[pytest.mark.esp32]),  # sha256 only supported on esp32, IDF-1820
    pytest.param('coredump_uart_bin_crc', marks=TARGETS_DUAL_CORE),
    pytest.param('coredump_uart_elf_crc', marks=TARGETS_DUAL_CORE),
    pytest.param('gdbstub', marks=TARGETS_DUAL_CORE),
    pytest.param('panic', marks=TARGETS_DUAL_CORE),
]

# Some tests run on all targets but need to behave differently on the dual-core ones.
# This list is used to check if the target is a dual-core one.
TARGETS_DUAL_CORE_NAMES = [x.mark.name for x in TARGETS_DUAL_CORE]

# The tests which panic on external stack require PSRAM capable runners
CONFIGS_EXTRAM_STACK = [
    pytest.param('coredump_extram_stack', marks=[pytest.mark.esp32, pytest.mark.esp32s2, pytest.mark.psram, pytest.mark.esp32s3, pytest.mark.quad_psram])
]


def get_default_backtrace(config: str) -> List[str]:
    return [config, 'app_main', 'main_task', 'vPortTaskWrapper']


def common_test(dut: PanicTestDut, config: str, expected_backtrace: Optional[List[str]] = None) -> None:
    if 'gdbstub' in config:
        dut.expect_exact('Entering gdb stub now.')
        dut.start_gdb()
        frames = dut.gdb_backtrace()
        if expected_backtrace is not None:
            dut.verify_gdb_backtrace(frames, expected_backtrace)
        dut.revert_log_level()
        return  # don't expect "Rebooting" output below

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
    dut.run_test_func(test_func_name)
    dut.expect_exact(
        'Task watchdog got triggered. The following tasks/users did not reset the watchdog in time:'
    )
    dut.expect_exact('CPU 0: main')
    if dut.is_xtensa:
        # on Xtensa, dumping registers on abort is not necessary, we only need to dump the backtrace
        dut.expect_none('register dump:')
        dut.expect_exact('Print CPU 0 (current core) backtrace')
        dut.expect_backtrace()
    else:
        # on RISC-V, need to dump both registers and stack memory to reconstruct the backtrace
        dut.expect_reg_dump(core=0)
        dut.expect_stack_dump()
    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')

    common_test(
        dut,
        config,
        expected_backtrace=get_default_backtrace(test_func_name),
    )


@pytest.mark.parametrize('config', CONFIGS_DUAL_CORE, indirect=True)
@pytest.mark.generic
def test_task_wdt_cpu1(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_exact(
        'Task watchdog got triggered. The following tasks/users did not reset the watchdog in time:'
    )
    dut.expect_exact('CPU 1: Infinite loop')
    if dut.is_xtensa:
        # see comment in test_task_wdt_cpu0
        dut.expect_none('register dump:')
        dut.expect_exact('Print CPU 1 backtrace')
        dut.expect_backtrace()
        # On Xtensa, we get incorrect backtrace from GDB in this test
        expected_backtrace = ['infinite_loop', 'vPortTaskWrapper']
    else:
        assert False, 'No dual-core RISC-V chips yet, check this test case later'

    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')

    common_test(
        dut,
        config,
        expected_backtrace=expected_backtrace,
    )


@pytest.mark.parametrize('config', CONFIGS_DUAL_CORE, indirect=True)
@pytest.mark.generic
def test_task_wdt_both_cpus(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    if dut.target == 'esp32s3':
        pytest.xfail(reason='Only prints "Print CPU 1 backtrace", IDF-6560')
    dut.run_test_func(test_func_name)
    dut.expect_exact(
        'Task watchdog got triggered. The following tasks/users did not reset the watchdog in time:'
    )
    dut.expect_exact('CPU 0: Infinite loop')
    dut.expect_exact('CPU 1: Infinite loop')
    if dut.is_xtensa:
        # see comment in test_task_wdt_cpu0
        dut.expect_none('register dump:')
        dut.expect_exact('Print CPU 0 (current core) backtrace')
        dut.expect_backtrace()
        dut.expect_exact('Print CPU 1 backtrace')
        dut.expect_backtrace()
        # On Xtensa, we get incorrect backtrace from GDB in this test
        expected_backtrace = ['infinite_loop', 'vPortTaskWrapper']
    else:
        assert False, 'No dual-core RISC-V chips yet, check this test case later'
    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')

    common_test(
        dut,
        config,
        expected_backtrace=expected_backtrace,
    )


@pytest.mark.parametrize('config', CONFIGS_EXTRAM_STACK, indirect=True)
def test_panic_extram_stack(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.expect_test_func_name(test_func_name)
    dut.expect_none('Allocated stack is not in external RAM')
    dut.expect_none('Guru Meditation')
    dut.expect_backtrace()
    dut.expect_elf_sha256()
    # Check that coredump is getting written to flash
    dut.expect_exact('Save core dump to flash...')
    # And that the stack is replaced and restored
    dut.expect_exact('Backing up stack @')
    dut.expect_exact('Restoring stack')
    # The caller must be accessible after restoring the stack
    dut.expect_exact('Core dump has been saved to flash.')
    common_test(dut, config)


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_int_wdt(
    dut: PanicTestDut, target: str, config: str, test_func_name: str
) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Interrupt wdt timeout on CPU0')
    dut.expect_reg_dump(0)
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()

    if target in TARGETS_DUAL_CORE_NAMES:
        assert dut.is_xtensa, 'No dual-core RISC-V chips yet, check the test case'
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
    dut.run_test_func(test_func_name)
    dut.expect_gme('Interrupt wdt timeout on CPU0')
    dut.expect_reg_dump(0)
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()

    if target in TARGETS_DUAL_CORE_NAMES:
        assert dut.is_xtensa, 'No dual-core RISC-V chips yet, check the test case'
        dut.expect_reg_dump(1)
        dut.expect_backtrace()

    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')

    common_test(dut, config, expected_backtrace=get_default_backtrace(test_func_name))


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_cache_error(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    if dut.target in ['esp32c3', 'esp32c2']:
        # Cache error interrupt is not raised, IDF-6398
        dut.expect_gme('Illegal instruction')
    elif dut.target in ['esp32s2']:
        # Cache error interrupt is not enabled, IDF-1558
        dut.expect_gme('IllegalInstruction')
    else:
        dut.expect_gme('Cache disabled but cached memory region accessed')
    dut.expect_reg_dump(0)
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()
    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')
    expected_backtrace = ['die'] + get_default_backtrace(test_func_name)
    if dut.target in ['esp32s2', 'esp32s3']:
        # 'test_cache_error' missing from GDB backtrace on ESP32-S2 and ESP-S3, IDF-6561
        expected_backtrace = ['die', 'app_main', 'main_task', 'vPortTaskWrapper']
    common_test(
        dut, config, expected_backtrace=expected_backtrace
    )


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_stack_overflow(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    if dut.is_xtensa:
        dut.expect_gme('Unhandled debug exception')
        dut.expect_exact('Stack canary watchpoint triggered (main)')
    else:
        # Stack watchpoint handling missing on RISC-V, IDF-6397
        dut.expect_gme('Breakpoint')
    dut.expect_reg_dump(0)
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()
    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')
    common_test(dut, config, expected_backtrace=get_default_backtrace(test_func_name))


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_instr_fetch_prohibited(
    dut: PanicTestDut, config: str, test_func_name: str
) -> None:
    dut.run_test_func(test_func_name)
    if dut.is_xtensa:
        dut.expect_gme('InstrFetchProhibited')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        expected_backtrace = ['_init'] + get_default_backtrace(test_func_name)
    else:
        dut.expect_gme('Instruction access fault')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()
        # On RISC-V, GDB is not able to determine the correct backtrace after
        # a jump to an invalid address.
        expected_backtrace = ['??']

    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')
    common_test(
        dut,
        config,
        expected_backtrace=expected_backtrace,
    )


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_illegal_instruction(
    dut: PanicTestDut, config: str, test_func_name: str
) -> None:
    dut.run_test_func(test_func_name)
    if dut.is_xtensa:
        dut.expect_gme('IllegalInstruction')
    else:
        dut.expect_gme('Illegal instruction')
    dut.expect_reg_dump(0)
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()
    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')
    common_test(dut, config, expected_backtrace=get_default_backtrace(test_func_name))


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_storeprohibited(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    if dut.is_xtensa:
        dut.expect_gme('StoreProhibited')
    else:
        dut.expect_gme('Store access fault')
    dut.expect_reg_dump(0)
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()
    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')
    common_test(dut, config, expected_backtrace=get_default_backtrace(test_func_name))


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_abort(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect(r'abort\(\) was called at PC [0-9xa-f]+ on core 0')
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])

    common_test(
        dut,
        config,
        expected_backtrace=[
            'panic_abort',
            'esp_system_abort',
            'abort'
        ] + get_default_backtrace(test_func_name),
    )


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_ub(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect('Undefined behavior of type out_of_bounds')
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])

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


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_abort_cache_disabled(
    dut: PanicTestDut, config: str, test_func_name: str
) -> None:
    if dut.target == 'esp32s2':
        pytest.xfail(reason='Crashes in itoa which is not in ROM, IDF-3572')
    dut.run_test_func(test_func_name)
    dut.expect(r'abort\(\) was called at PC [0-9xa-f]+ on core 0')
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])
    common_test(
        dut,
        config,
        expected_backtrace=[
            'panic_abort',
            'esp_system_abort',
            'abort'
        ] + get_default_backtrace(test_func_name),
    )


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_assert(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect(
        re.compile(
            rb'assert failed:[\s\w()]*?\s[.\w/]*\.(?:c|cpp|h|hpp):\d.*$', re.MULTILINE
        )
    )
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])
    common_test(
        dut,
        config,
        expected_backtrace=[
            'panic_abort',
            'esp_system_abort',
            '__assert_func'
        ] + get_default_backtrace(test_func_name))


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_assert_cache_disabled(
    dut: PanicTestDut, config: str, test_func_name: str
) -> None:
    if dut.target == 'esp32s2':
        pytest.xfail(reason='Crashes in itoa which is not in ROM, IDF-3572')
    dut.run_test_func(test_func_name)
    dut.expect(re.compile(rb'assert failed: [0-9xa-fA-F]+.*$', re.MULTILINE))
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])
    common_test(
        dut,
        config,
        expected_backtrace=[
            'panic_abort',
            'esp_system_abort',
            '__assert_func'
        ] + get_default_backtrace(test_func_name))


@pytest.mark.esp32
@pytest.mark.parametrize('config', ['panic_delay'], indirect=True)
def test_panic_delay(dut: PanicTestDut) -> None:
    dut.run_test_func('test_storeprohibited')
    try:
        dut.expect_exact('Rebooting...', timeout=4)
    except pexpect.TIMEOUT:
        # We are supposed to NOT find the output for the specified time
        pass
    else:
        # If we actually match the output within the timeout, it means the delay didn't work
        raise AssertionError('Rebooted too early, delay is too short')

    dut.expect_exact('Rebooting...', timeout=3)
    dut.expect_exact('rst:0xc (SW_CPU_RESET)')
