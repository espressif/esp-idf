# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import re
from typing import List
from typing import Optional
from typing import Union

import pexpect
import pytest
from test_panic_util import PanicTestDut

TARGETS_XTENSA_SINGLE_CORE = [
    pytest.mark.esp32s2,
]

TARGETS_XTENSA_DUAL_CORE = [
    pytest.mark.esp32,
    pytest.mark.esp32s3,
]

TARGETS_XTENSA = TARGETS_XTENSA_SINGLE_CORE + TARGETS_XTENSA_DUAL_CORE

TARGETS_RISCV_SINGLE_CORE = [
    pytest.mark.esp32c2,
    pytest.mark.esp32c3,
    pytest.mark.esp32c6,
    pytest.mark.esp32h2,
]

TARGETS_RISCV_DUAL_CORE = [
    pytest.mark.esp32p4,
]

TARGETS_RISCV = TARGETS_RISCV_SINGLE_CORE + TARGETS_RISCV_DUAL_CORE

# Markers for all the targets this test currently runs on
TARGETS_ALL = TARGETS_XTENSA + TARGETS_RISCV

# Some tests only run on dual-core targets, they use the config below.
TARGETS_DUAL_CORE = TARGETS_XTENSA_DUAL_CORE + TARGETS_RISCV_DUAL_CORE

# Most tests run on all targets and with all configs.
# This list is passed to @pytest.mark.parametrize for each of the test cases.
# It creates an outer product of the sets: [configs] x [targets],
# with some exceptions.
CONFIGS = [
    pytest.param('coredump_flash_bin_crc', marks=TARGETS_ALL),
    pytest.param('coredump_flash_elf_sha', marks=TARGETS_ALL),
    pytest.param('coredump_uart_bin_crc', marks=TARGETS_ALL),
    pytest.param('coredump_uart_elf_crc', marks=TARGETS_ALL),
    pytest.param('coredump_flash_custom_stack', marks=TARGETS_RISCV),
    pytest.param('gdbstub', marks=TARGETS_ALL),
    pytest.param('panic', marks=TARGETS_ALL),
]

CONFIGS_DUAL_CORE = [
    pytest.param('coredump_flash_bin_crc', marks=TARGETS_DUAL_CORE),
    pytest.param('coredump_flash_elf_sha', marks=TARGETS_DUAL_CORE),
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
    pytest.param('coredump_extram_stack', marks=[pytest.mark.esp32, pytest.mark.psram]),
    pytest.param('coredump_extram_stack', marks=[pytest.mark.esp32s2, pytest.mark.generic]),
    pytest.param('coredump_extram_stack', marks=[pytest.mark.esp32s3, pytest.mark.quad_psram]),
]

CONFIGS_HW_STACK_GUARD = [
    pytest.param('coredump_flash_bin_crc', marks=TARGETS_RISCV),
    pytest.param('coredump_uart_bin_crc', marks=TARGETS_RISCV),
    pytest.param('coredump_uart_elf_crc', marks=TARGETS_RISCV),
    pytest.param('gdbstub', marks=TARGETS_RISCV),
    pytest.param('panic', marks=TARGETS_RISCV),
]

CONFIGS_HW_STACK_GUARD_DUAL_CORE = [
    pytest.param('coredump_flash_bin_crc', marks=TARGETS_RISCV_DUAL_CORE),
    pytest.param('coredump_uart_bin_crc', marks=TARGETS_RISCV_DUAL_CORE),
    pytest.param('coredump_uart_elf_crc', marks=TARGETS_RISCV_DUAL_CORE),
    pytest.param('gdbstub', marks=TARGETS_RISCV_DUAL_CORE),
    pytest.param('panic', marks=TARGETS_RISCV_DUAL_CORE),
]

CONFIG_CAPTURE_DRAM = [pytest.param('coredump_flash_capture_dram', marks=TARGETS_ALL)]

# Panic abort information will start with this string.
PANIC_ABORT_PREFIX = 'Panic reason: '


def get_default_backtrace(config: str) -> List[str]:
    return [config, 'app_main', 'main_task', 'vPortTaskWrapper']


def common_test(dut: PanicTestDut, config: str, expected_backtrace: Optional[List[str]] = None, check_cpu_reset: Optional[bool] = True,
                expected_coredump: Optional[List[Union[str, re.Pattern]]] = None) -> None:
    if 'gdbstub' in config:
        dut.expect_exact('Entering gdb stub now.')
        dut.start_gdb_for_gdbstub()
        frames = dut.gdb_backtrace()
        if expected_backtrace is not None:
            dut.verify_gdb_backtrace(frames, expected_backtrace)
        dut.revert_log_level()
        return  # don't expect "Rebooting" output below

    # We will only perform comparisons for ELF files, as we are not introducing any new fields to the binary file format.
    if 'bin' in config:
        expected_coredump = None

    if 'uart' in config:
        dut.process_coredump_uart(expected_coredump)
    elif 'flash' in config:
        dut.process_coredump_flash(expected_coredump)
    elif 'panic' in config:
        pass

    dut.expect('Rebooting...')

    if check_cpu_reset:
        dut.expect_cpu_reset()


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

    coredump_pattern = (PANIC_ABORT_PREFIX +
                        'Task watchdog got triggered. '
                        'The following tasks/users did not reset the watchdog in time:\n - ')
    if dut.is_multi_core:
        coredump_pattern += 'IDLE0 (CPU 0)'
    else:
        coredump_pattern += 'IDLE (CPU 0)'

    common_test(
        dut,
        config,
        expected_backtrace=get_default_backtrace(test_func_name),
        expected_coredump=[coredump_pattern]
    )


@pytest.mark.parametrize('config', CONFIGS_DUAL_CORE, indirect=True)
@pytest.mark.generic
def test_task_wdt_cpu1(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_exact(
        'Task watchdog got triggered. The following tasks/users did not reset the watchdog in time:'
    )
    dut.expect_exact('CPU 1: Infinite loop')
    expected_backtrace = ['infinite_loop', 'vPortTaskWrapper']
    if dut.is_xtensa:
        # see comment in test_task_wdt_cpu0
        dut.expect_none('register dump:')
        dut.expect_exact('Print CPU 1 backtrace')
        dut.expect_backtrace()
    else:
        # on RISC-V, need to dump both registers and stack memory to reconstruct the backtrace
        dut.expect_reg_dump(core=1)
        dut.expect_stack_dump()

    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')

    coredump_pattern = (PANIC_ABORT_PREFIX +
                        'Task watchdog got triggered. '
                        'The following tasks/users did not reset the watchdog in time:\n - IDLE1 (CPU 1)')
    common_test(
        dut,
        config,
        expected_backtrace=expected_backtrace,
        expected_coredump=[coredump_pattern]
    )


@pytest.mark.parametrize('config', CONFIGS_EXTRAM_STACK, indirect=True)
def test_panic_extram_stack(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
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
        dut.expect_reg_dump(1)
        if dut.is_xtensa:
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
        dut.expect_reg_dump(1)
        if dut.is_xtensa:
            dut.expect_backtrace()

    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')

    common_test(dut, config, expected_backtrace=get_default_backtrace(test_func_name))


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_cache_error(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    if dut.target in ['esp32c3', 'esp32c2']:
        dut.expect_gme('Cache error')
        dut.expect_exact('Cached memory region accessed while ibus or cache is disabled')
    elif dut.target in ['esp32c6', 'esp32h2']:
        dut.expect_gme('Cache error')
        dut.expect_exact('Cache access error')
    elif dut.target in ['esp32s2']:
        # Cache error interrupt is not enabled, IDF-1558
        dut.expect_gme('IllegalInstruction')
    elif dut.target in ['esp32p4']:  # TODO IDF-7515
        dut.expect_gme('Instruction access fault')
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
        dut, config, expected_backtrace=expected_backtrace, check_cpu_reset=(dut.target != 'esp32')
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


def check_x_prohibited(dut: PanicTestDut, config: str, test_func_name: str, operation: str) -> None:
    dut.run_test_func(test_func_name)
    if dut.is_xtensa:
        dut.expect_gme(f'{operation}Prohibited')
    else:
        dut.expect_gme(f'{operation} access fault')
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
    check_x_prohibited(dut, config, test_func_name, 'Store')


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_loadprohibited(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    check_x_prohibited(dut, config, test_func_name, 'Load')


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_abort(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    regex_pattern = rb'abort\(\) was called at PC [0-9xa-f]+ on core 0'
    dut.expect(regex_pattern)
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])

    coredump_pattern = re.compile(PANIC_ABORT_PREFIX + regex_pattern.decode('utf-8'))
    common_test(
        dut,
        config,
        expected_backtrace=[
            'panic_abort',
            'esp_system_abort',
            'abort'
        ] + get_default_backtrace(test_func_name),
        expected_coredump=[coredump_pattern]
    )


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_ub(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    regex_pattern = rb'Undefined behavior of type out_of_bounds'
    dut.expect(regex_pattern)
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])

    coredump_pattern = re.compile(PANIC_ABORT_PREFIX + regex_pattern.decode('utf-8'))
    common_test(
        dut,
        config,
        expected_backtrace=[
            'panic_abort',
            'esp_system_abort',
            '__ubsan_default_handler',
            '__ubsan_handle_out_of_bounds'
        ] + get_default_backtrace(test_func_name),
        expected_coredump=[coredump_pattern]
    )


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_abort_cache_disabled(
    dut: PanicTestDut, config: str, test_func_name: str
) -> None:
    if dut.target == 'esp32s2':
        pytest.xfail(reason='Crashes in itoa which is not in ROM, IDF-3572')
    dut.run_test_func(test_func_name)
    regex_pattern = rb'abort\(\) was called at PC [0-9xa-f]+ on core 0'
    dut.expect(regex_pattern)
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])

    coredump_pattern = re.compile(PANIC_ABORT_PREFIX + regex_pattern.decode('utf-8'))
    common_test(
        dut,
        config,
        expected_backtrace=[
            'panic_abort',
            'esp_system_abort',
            'abort'
        ] + get_default_backtrace(test_func_name),
        expected_coredump=[coredump_pattern]
    )


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_assert(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    regex_pattern = rb'assert failed:[\s\w()]*?\s[.\w/]*\.(?:c|cpp|h|hpp):\d.*$'
    dut.expect(re.compile(regex_pattern, re.MULTILINE))
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])

    coredump_pattern = re.compile(PANIC_ABORT_PREFIX + regex_pattern.decode('utf-8'), re.MULTILINE)
    common_test(
        dut,
        config,
        expected_backtrace=[
            'panic_abort',
            'esp_system_abort',
            '__assert_func'
        ] + get_default_backtrace(test_func_name),
        expected_coredump=[coredump_pattern]
    )


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_assert_cache_disabled(
    dut: PanicTestDut, config: str, test_func_name: str
) -> None:
    if dut.target == 'esp32s2':
        pytest.xfail(reason='Crashes in itoa which is not in ROM, IDF-3572')
    dut.run_test_func(test_func_name)
    regex_pattern = rb'assert failed: [0-9xa-fA-F]+.*$'
    dut.expect(re.compile(regex_pattern, re.MULTILINE))
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])

    coredump_pattern = re.compile(PANIC_ABORT_PREFIX + regex_pattern.decode('utf-8'), re.MULTILINE)
    common_test(
        dut,
        config,
        expected_backtrace=[
            'panic_abort',
            'esp_system_abort',
            '__assert_func'
        ] + get_default_backtrace(test_func_name),
        expected_coredump=[coredump_pattern]
    )


@pytest.mark.esp32
@pytest.mark.generic
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


#########################
# for memprot test only #
#########################

# Memprot-related tests are supported only on targets with PMS/PMA peripheral;
# currently ESP32-S2, ESP32-C3, ESP32-C2, ESP32-H2 and ESP32-C6 are supported
CONFIGS_MEMPROT_IDRAM = [
    pytest.param('memprot_esp32s2', marks=[pytest.mark.esp32s2]),
    pytest.param('memprot_esp32c3', marks=[pytest.mark.esp32c3]),
    pytest.param('memprot_esp32c2', marks=[pytest.mark.esp32c2]),
    pytest.param('memprot_esp32c6', marks=[pytest.mark.esp32c6]),
    pytest.param('memprot_esp32h2', marks=[pytest.mark.esp32h2]),
    pytest.param('memprot_esp32p4', marks=[pytest.mark.esp32p4])
]

CONFIGS_MEMPROT_DCACHE = [
    pytest.param('memprot_esp32s2', marks=pytest.mark.esp32s2),
]

CONFIGS_MEMPROT_RTC_FAST_MEM = [
    pytest.param('memprot_esp32s2', marks=[pytest.mark.esp32s2]),
    pytest.param('memprot_esp32c3', marks=[pytest.mark.esp32c3]),
    pytest.param('memprot_esp32c6', marks=[pytest.mark.esp32c6]),
    pytest.param('memprot_esp32h2', marks=[pytest.mark.esp32h2]),
    pytest.param('memprot_esp32p4', marks=[pytest.mark.esp32p4])
]

CONFIGS_MEMPROT_RTC_SLOW_MEM = [
    pytest.param('memprot_esp32s2', marks=[pytest.mark.esp32s2]),
]

CONFIGS_MEMPROT_FLASH_IDROM = [
    pytest.param('memprot_esp32c6', marks=[pytest.mark.esp32c6]),
    pytest.param('memprot_esp32h2', marks=[pytest.mark.esp32h2]),
    pytest.param('memprot_esp32p4', marks=[pytest.mark.esp32p4])
]


@pytest.mark.parametrize('config', CONFIGS_MEMPROT_DCACHE, indirect=True)
@pytest.mark.generic
def test_dcache_read_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_exact(r'Test error: Test function has returned')
    dut.expect_cpu_reset()


# TODO: IDF-6820: ESP32-S2 -> Fix multiple panic reasons in different runs
@pytest.mark.parametrize('config', CONFIGS_MEMPROT_DCACHE, indirect=True)
@pytest.mark.generic
@pytest.mark.xfail('config.getvalue("target") == "esp32s2"', reason='Incorrect panic reason may be observed', run=False)
def test_dcache_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Memory protection fault')
    dut.expect(r'Write operation at address [0-9xa-f]+ not permitted \((\S+)\)')
    dut.expect_reg_dump(0)
    dut.expect_backtrace()
    dut.expect_cpu_reset()


@pytest.mark.parametrize('config', CONFIGS_MEMPROT_IDRAM, indirect=True)
@pytest.mark.generic
def test_iram_reg1_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)

    if dut.target == 'esp32s2':
        dut.expect_gme('Memory protection fault')
        dut.expect(r'Write operation at address [0-9xa-f]+ not permitted \((\S+)\)')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
    elif dut.target == 'esp32c3':
        dut.expect_exact(r'Test error: Test function has returned')
    elif dut.target in ['esp32c2', 'esp32c6', 'esp32h2', 'esp32p4']:
        dut.expect_gme('Store access fault')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()

    dut.expect_cpu_reset()


@pytest.mark.parametrize('config', CONFIGS_MEMPROT_IDRAM, indirect=True)
@pytest.mark.generic
def test_iram_reg2_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)

    if dut.target == 'esp32s2':
        dut.expect_gme('Memory protection fault')
        dut.expect(r'Write operation at address [0-9xa-f]+ not permitted \((\S+)\)')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
    elif dut.target == 'esp32c3':
        dut.expect_gme('Memory protection fault')
        dut.expect(r'  memory type: (\S+)')
        dut.expect(r'  faulting address: [0-9xa-f]+')
        dut.expect(r'  operation type: (\S+)')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()
    elif dut.target in ['esp32c2', 'esp32c6', 'esp32h2', 'esp32p4']:
        dut.expect_gme('Store access fault')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()

    dut.expect_cpu_reset()


@pytest.mark.parametrize('config', CONFIGS_MEMPROT_IDRAM, indirect=True)
@pytest.mark.generic
def test_iram_reg3_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)

    if dut.target == 'esp32s2':
        dut.expect_gme('Memory protection fault')
        dut.expect(r'Write operation at address [0-9xa-f]+ not permitted \((\S+)\)')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
    elif dut.target == 'esp32c3':
        dut.expect_gme('Memory protection fault')
        dut.expect(r'  memory type: (\S+)')
        dut.expect(r'  faulting address: [0-9xa-f]+')
        dut.expect(r'  operation type: (\S+)')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()
    elif dut.target in ['esp32c2', 'esp32c6', 'esp32h2', 'esp32p4']:
        dut.expect_gme('Store access fault')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()

    dut.expect_cpu_reset()


# TODO: IDF-6820: ESP32-S2 -> Fix incorrect panic reason: Unhandled debug exception
@pytest.mark.parametrize('config', CONFIGS_MEMPROT_IDRAM, indirect=True)
@pytest.mark.generic
@pytest.mark.xfail('config.getvalue("target") == "esp32s2"', reason='Incorrect panic reason may be observed', run=False)
def test_iram_reg4_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)

    if dut.target == 'esp32s2':
        dut.expect_gme('Memory protection fault')
        dut.expect(r'Write operation at address [0-9xa-f]+ not permitted \((\S+)\)')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
    elif dut.target == 'esp32c3':
        dut.expect_gme('Memory protection fault')
        dut.expect(r'  memory type: (\S+)')
        dut.expect(r'  faulting address: [0-9xa-f]+')
        dut.expect(r'  operation type: (\S+)')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()
    elif dut.target in ['esp32c2', 'esp32c6', 'eps32h2']:
        dut.expect_gme('Store access fault')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()

    dut.expect_cpu_reset()


# TODO: IDF-6820: ESP32-S2 -> Fix multiple panic reasons in different runs
@pytest.mark.parametrize('config', CONFIGS_MEMPROT_IDRAM, indirect=True)
@pytest.mark.generic
@pytest.mark.xfail('config.getvalue("target") == "esp32s2"', reason='Multiple panic reasons for the same test may surface', run=False)
def test_dram_reg1_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)

    if dut.target == 'esp32s2':
        dut.expect_gme('Memory protection fault')
        dut.expect(r'Unknown operation at address [0-9xa-f]+ not permitted \((\S+)\)')
        dut.expect_reg_dump(0)
        dut.expect_corrupted_backtrace()
    elif dut.target in ['esp32c3', 'esp32c2', 'esp32c6', 'esp32h2', 'esp32p4']:
        dut.expect_gme('Instruction access fault')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()

    dut.expect_cpu_reset()


# TODO: IDF-6820: ESP32-S2 -> Fix multiple panic reasons in different runs
@pytest.mark.parametrize('config', CONFIGS_MEMPROT_IDRAM, indirect=True)
@pytest.mark.generic
@pytest.mark.xfail('config.getvalue("target") == "esp32s2"', reason='Multiple panic reasons for the same test may surface', run=False)
def test_dram_reg2_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)

    if dut.target == 'esp32s2':
        dut.expect_gme('InstructionFetchError')
        dut.expect_reg_dump(0)
        dut.expect_corrupted_backtrace()
    elif dut.target in ['esp32c3', 'esp32c2', 'esp32c6', 'esp32h2', 'esp32p4']:
        dut.expect_gme('Instruction access fault')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()

    dut.expect_cpu_reset()


@pytest.mark.parametrize('config', CONFIGS_MEMPROT_RTC_FAST_MEM, indirect=True)
@pytest.mark.generic
def test_rtc_fast_reg1_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_exact(r'Test error: Test function has returned')
    dut.expect_cpu_reset()


@pytest.mark.parametrize('config', CONFIGS_MEMPROT_RTC_FAST_MEM, indirect=True)
@pytest.mark.generic
@pytest.mark.skipif('config.getvalue("target") in ["esp32c6", "esp32h2", "esp32p4"]', reason='Not a violation condition, no PMS peripheral case')
def test_rtc_fast_reg2_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Memory protection fault')

    if dut.target == 'esp32s2':
        dut.expect(r'Read operation at address [0-9xa-f]+ not permitted \((\S+)\)')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
    elif dut.target == 'esp32c3':
        dut.expect(r'  memory type: (\S+)')
        dut.expect(r'  faulting address: [0-9xa-f]+')
        dut.expect(r'  operation type: (\S+)')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()

    dut.expect_cpu_reset()


# TODO: IDF-6820: ESP32-S2 -> Fix multiple panic reasons in different runs
@pytest.mark.parametrize('config', CONFIGS_MEMPROT_RTC_FAST_MEM, indirect=True)
@pytest.mark.generic
@pytest.mark.xfail('config.getvalue("target") == "esp32s2"', reason='Multiple panic reasons for the same test may surface', run=False)
def test_rtc_fast_reg3_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)

    if dut.target == 'esp32s2':
        dut.expect_gme('Memory protection fault')
        dut.expect(r'Unknown operation at address [0-9xa-f]+ not permitted \((\S+)\)')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
    elif dut.target == 'esp32c3':
        dut.expect_gme('Memory protection fault')
        dut.expect(r'  memory type: (\S+)')
        dut.expect(r'  faulting address: [0-9xa-f]+')
        dut.expect(r'  operation type: (\S+)')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()
    elif dut.target in ['esp32c6', 'esp32h2', 'esp32p4']:
        dut.expect_gme('Instruction access fault')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()

    dut.expect_cpu_reset()


@pytest.mark.parametrize('config', CONFIGS_MEMPROT_RTC_SLOW_MEM, indirect=True)
@pytest.mark.generic
def test_rtc_slow_reg1_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Memory protection fault')
    dut.expect(r'Read operation at address [0-9xa-f]+ not permitted \((\S+)\)')
    dut.expect_reg_dump(0)
    dut.expect_corrupted_backtrace()
    dut.expect_cpu_reset()


@pytest.mark.parametrize('config', CONFIGS_MEMPROT_RTC_SLOW_MEM, indirect=True)
@pytest.mark.generic
def test_rtc_slow_reg2_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Memory protection fault')
    dut.expect(r'Read operation at address [0-9xa-f]+ not permitted \((\S+)\)')
    dut.expect_reg_dump(0)
    dut.expect_corrupted_backtrace()
    dut.expect_cpu_reset()


@pytest.mark.parametrize('config', CONFIGS_MEMPROT_FLASH_IDROM, indirect=True)
@pytest.mark.generic
def test_irom_reg_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Store access fault')
    dut.expect_reg_dump(0)
    dut.expect_cpu_reset()


@pytest.mark.parametrize('config', CONFIGS_MEMPROT_FLASH_IDROM, indirect=True)
@pytest.mark.generic
def test_drom_reg_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Store access fault')
    dut.expect_reg_dump(0)
    dut.expect_cpu_reset()


@pytest.mark.parametrize('config', CONFIGS_MEMPROT_FLASH_IDROM, indirect=True)
@pytest.mark.generic
def test_drom_reg_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Instruction access fault')
    dut.expect_reg_dump(0)
    dut.expect_cpu_reset()


@pytest.mark.esp32
@pytest.mark.generic
@pytest.mark.parametrize('config', ['gdbstub_coredump'], indirect=True)
def test_gdbstub_coredump(dut: PanicTestDut) -> None:
    test_func_name = 'test_storeprohibited'
    dut.run_test_func(test_func_name)

    dut.process_coredump_uart()

    dut.expect_exact('Entering gdb stub now.')
    dut.start_gdb_for_gdbstub()
    frames = dut.gdb_backtrace()
    dut.verify_gdb_backtrace(frames, get_default_backtrace(test_func_name))
    dut.revert_log_level()
    return  # don't expect "Rebooting" output below


def test_hw_stack_guard_cpu(dut: PanicTestDut, cpu: int) -> None:
    dut.expect_exact(f'Guru Meditation Error: Core  {cpu} panic\'ed (Stack protection fault).')
    dut.expect_none('ASSIST_DEBUG is not triggered BUT interrupt occurred!')
    dut.expect_exact(f'Detected in task "HWSG{cpu}"')
    addr = dut.expect('at 0x([0-9a-fA-F]{8})')
    assert addr.group(1) != b'00000000'
    addr = dut.expect('Stack pointer: 0x([0-9a-fA-F]{8})')
    assert addr.group(1) != b'00000000'
    addr = dut.expect(r'Stack bounds: 0x([0-9a-fA-F]{8})')
    assert addr.group(1) != b'00000000'
    start_addr = int(addr.group(1), 16)
    addr = dut.expect(r' - 0x([0-9a-fA-F]{8})')
    assert addr.group(1) != b'00000000'
    end_addr = int(addr.group(1), 16)
    assert end_addr > start_addr


@pytest.mark.parametrize('config', CONFIGS_HW_STACK_GUARD, indirect=True)
@pytest.mark.generic
def test_hw_stack_guard_cpu0(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    test_hw_stack_guard_cpu(dut, 0)
    common_test(dut, config)


@pytest.mark.parametrize('config', CONFIGS_HW_STACK_GUARD_DUAL_CORE, indirect=True)
@pytest.mark.generic
def test_hw_stack_guard_cpu1(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    test_hw_stack_guard_cpu(dut, 1)
    common_test(dut, config)


@pytest.mark.esp32
@pytest.mark.parametrize('config', ['panic'], indirect=True)
@pytest.mark.generic
def test_illegal_access(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    if dut.is_xtensa:
        dut.expect(r'\[1\] val: (-?\d+) at 0x80000000', timeout=30)
        dut.expect_gme('LoadProhibited')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation')


@pytest.mark.parametrize('config', CONFIG_CAPTURE_DRAM, indirect=True)
@pytest.mark.generic
def test_capture_dram(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)

    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])

    core_elf_file = dut.process_coredump_flash()
    dut.start_gdb_for_coredump(core_elf_file)

    assert dut.gdb_data_eval_expr('g_data_var') == '43'
    assert dut.gdb_data_eval_expr('g_bss_var') == '55'
    assert re.search(r'0x[0-9a-fA-F]+ "Coredump Test"', dut.gdb_data_eval_expr('g_heap_ptr'))
    assert int(dut.gdb_data_eval_expr('g_cd_iram')) == 0x4243
    assert int(dut.gdb_data_eval_expr('g_cd_dram')) == 0x4344

    if dut.target != 'esp32c2':
        assert int(dut.gdb_data_eval_expr('g_rtc_data_var')) == 0x55AA
        assert int(dut.gdb_data_eval_expr('g_rtc_fast_var')) == 0xAABBCCDD
