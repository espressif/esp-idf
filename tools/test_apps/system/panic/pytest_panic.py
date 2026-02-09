# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import itertools
import re
from collections.abc import Sequence
from re import Pattern
from typing import Any

import pexpect
import pytest
from pytest_embedded_idf.utils import idf_parametrize
from test_panic_util import PanicTestDut

TARGETS_XTENSA_SINGLE_CORE = ['esp32s2']
TARGETS_XTENSA_DUAL_CORE = ['esp32', 'esp32s3']
TARGETS_XTENSA = TARGETS_XTENSA_SINGLE_CORE + TARGETS_XTENSA_DUAL_CORE

TARGETS_RISCV_SINGLE_CORE = ['esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32c61']
TARGETS_RISCV_DUAL_CORE = ['esp32p4']
TARGETS_RISCV_TARGETS = TARGETS_RISCV_SINGLE_CORE + TARGETS_RISCV_DUAL_CORE

TARGETS_RISCV = TARGETS_RISCV_SINGLE_CORE + TARGETS_RISCV_DUAL_CORE

# Markers for all the targets this test currently runs on
TARGETS_ALL = TARGETS_XTENSA + TARGETS_RISCV

# Some tests only run on dual-core targets, they use the config below.
TARGETS_DUAL_CORE = TARGETS_XTENSA_DUAL_CORE + TARGETS_RISCV_DUAL_CORE

CONFIGS = list(
    itertools.chain(
        itertools.product(
            [
                'coredump_flash_default',
                'coredump_flash_soft_sha',
                'coredump_uart_default',
                'coredump_flash_custom_stack',
                'gdbstub',
                'panic',
            ],
            TARGETS_ALL,
        )
    )
)

CONFIGS_UBSAN = list(
    itertools.chain(
        itertools.product(
            [
                'gdbstub',
                'panic',
            ],
            TARGETS_ALL,
        )
    )
)

CONFIG_PANIC = list(itertools.chain(itertools.product(['panic'], ['supported_targets'])))
CONFIG_PANIC_DUAL_CORE = list(itertools.chain(itertools.product(['panic'], TARGETS_DUAL_CORE)))
CONFIG_PANIC_HALT = list(itertools.chain(itertools.product(['panic_halt'], TARGETS_ALL)))

CONFIGS_BACKTRACE = list(
    itertools.chain(
        # One single-core target and one dual-core target is enough
        itertools.product(['framepointer'], ['esp32c3', 'esp32p4'])
    )
)

CONFIGS_DUAL_CORE = list(
    itertools.chain(
        itertools.product(
            [
                'coredump_flash_default',
                'coredump_uart_default',
                'gdbstub',
                'panic',
            ],
            TARGETS_DUAL_CORE,
        )
    )
)

CONFIGS_HW_STACK_GUARD = list(
    itertools.chain(
        itertools.product(
            ['coredump_uart_default', 'gdbstub', 'panic'],
            TARGETS_RISCV,
        )
    )
)

CONFIGS_HW_STACK_GUARD_DUAL_CORE = list(
    itertools.chain(
        itertools.product(
            ['coredump_uart_default', 'gdbstub', 'panic'],
            TARGETS_RISCV_DUAL_CORE,
        )
    )
)

CONFIG_CAPTURE_DRAM = list(
    itertools.chain(itertools.product(['coredump_flash_capture_dram', 'coredump_uart_capture_dram'], TARGETS_ALL))
)

CONFIG_COREDUMP_SUMMARY = list(itertools.chain(itertools.product(['coredump_flash_default'], TARGETS_ALL)))

CONFIG_COREDUMP_SUMMARY_FLASH_ENCRYPTED = list(
    itertools.chain(
        itertools.product(['coredump_flash_encrypted'], ['esp32', 'esp32c3']),
        itertools.product(['coredump_flash_encrypted_coredump_plain'], ['esp32', 'esp32c3']),
    )
)

# Panic abort information will start with this string.
PANIC_ABORT_PREFIX = 'Panic reason: '


def get_default_backtrace(config: str) -> list[str]:
    return [config, 'app_main', 'main_task', 'vPortTaskWrapper']


def expect_coredump_flash_write_logs(dut: PanicTestDut, config: str, check_cpu_reset: bool | None = True) -> None:
    dut.expect_exact('Save core dump to flash...')
    if 'extram_stack' in config:
        dut.expect_exact('Backing up stack @')
        dut.expect_exact('Restoring stack')
    dut.expect_exact('Core dump has been saved to flash.')
    dut.expect(dut.REBOOT)
    if check_cpu_reset:
        dut.expect_cpu_reset()


def expect_coredump_uart_write_logs(dut: PanicTestDut, check_cpu_reset: bool | None = True) -> Any:
    # ================= CORE DUMP START =================
    # B8AAAMAEgAGAAAAXAEAAAAAAABkAAAA
    # ...
    # ================= CORE DUMP END =================
    # Coredump checksum='9730d7ff'
    # Rebooting...
    # ..
    # rst:0xc (SW_CPU_RESET),boot:

    # Read all uart logs until the end of the reset reason
    uart_str = dut.expect(',boot:', return_what_before_match=True).decode('utf-8', errors='ignore')
    coredump_base64 = uart_str.split(dut.COREDUMP_UART_START)[1].split(dut.COREDUMP_UART_END)[0].strip()
    uart_str = uart_str.split(dut.COREDUMP_UART_END)[1]
    assert re.search(dut.COREDUMP_CHECKSUM, uart_str)
    assert re.search(dut.REBOOT, uart_str)
    if check_cpu_reset:
        assert re.search(dut.CPU_RESET, uart_str)
    return coredump_base64


def common_test(
    dut: PanicTestDut,
    config: str,
    expected_backtrace: list[str] | None = None,
    check_cpu_reset: bool | None = True,
    expected_coredump: Sequence[str | Pattern[Any]] | None = None,
) -> None:
    if 'gdbstub' in config:
        if 'coredump' in config:
            uart_str = dut.expect(dut.COREDUMP_CHECKSUM, return_what_before_match=True).decode('utf-8')
            coredump_base64 = uart_str.split(dut.COREDUMP_UART_START)[1].split(dut.COREDUMP_UART_END)[0].strip()
            dut.process_coredump_uart(coredump_base64, expected_coredump)
        dut.expect_exact('Entering gdb stub now.')
        dut.start_gdb_for_gdbstub()
        frames = dut.gdb_backtrace()
        if expected_backtrace is not None:
            dut.verify_gdb_backtrace(frames, expected_backtrace)
        dut.revert_log_level()
        return  # don't expect "Rebooting" output below

    if 'uart' in config:
        coredump_base64 = expect_coredump_uart_write_logs(dut, check_cpu_reset)
        dut.process_coredump_uart(coredump_base64, expected_coredump)
        check_cpu_reset = False  # CPU reset is already checked in expect_coredump_uart_write_logs
    elif 'flash' in config:
        expect_coredump_flash_write_logs(dut, config, check_cpu_reset)
        dut.process_coredump_flash(expected_coredump)
        check_cpu_reset = False  # CPU reset is already checked in expect_coredump_flash_write_logs
    elif 'panic' in config:
        dut.expect(dut.REBOOT, timeout=60)

    if check_cpu_reset:
        dut.expect_cpu_reset()


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_task_wdt_cpu0(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_exact('Task watchdog got triggered. The following tasks/users did not reset the watchdog in time:')
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

    coredump_pattern = (
        PANIC_ABORT_PREFIX + 'Task watchdog got triggered. '
        'The following tasks/users did not reset the watchdog in time:\n - '
    )
    if dut.is_multi_core:
        coredump_pattern += 'IDLE0 (CPU 0)'
    else:
        coredump_pattern += 'IDLE (CPU 0)'

    common_test(
        dut, config, expected_backtrace=get_default_backtrace(test_func_name), expected_coredump=[coredump_pattern]
    )


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_DUAL_CORE, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_task_wdt_cpu1(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_exact('Task watchdog got triggered. The following tasks/users did not reset the watchdog in time:')
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

    coredump_pattern = (
        PANIC_ABORT_PREFIX + 'Task watchdog got triggered. '
        'The following tasks/users did not reset the watchdog in time:\n - IDLE1 (CPU 1)'
    )
    common_test(dut, config, expected_backtrace=expected_backtrace, expected_coredump=[coredump_pattern])


@idf_parametrize(
    'config,target,markers',
    [
        ('coredump_flash_extram_stack_heap_esp32', 'esp32', (pytest.mark.psram,)),
        ('coredump_flash_extram_stack_heap_esp32s2', 'esp32s2', (pytest.mark.generic,)),
        ('coredump_flash_extram_stack_heap_esp32s3', 'esp32s3', (pytest.mark.quad_psram,)),
        ('coredump_flash_extram_stack_bss_esp32', 'esp32', (pytest.mark.psram,)),
        ('coredump_flash_extram_stack_bss_esp32s2', 'esp32s2', (pytest.mark.generic,)),
        ('coredump_flash_extram_stack_bss_esp32s3', 'esp32s3', (pytest.mark.quad_psram,)),
    ],
    indirect=['config', 'target'],
)
def test_panic_extram_stack(dut: PanicTestDut, config: str) -> None:
    if 'heap' in config:
        dut.run_test_func('test_panic_extram_stack_heap')
    else:
        dut.run_test_func('test_panic_extram_stack_bss')
    dut.expect_none('Allocated stack is not in external RAM')
    dut.expect_none('Guru Meditation')
    dut.expect_backtrace()
    dut.expect_elf_sha256()

    if dut.target == 'esp32':
        # ESP32 External data memory range [0x3f800000-0x3fc00000)
        coredump_pattern = re.compile('.coredump.tasks.data (0x3[fF][8-9a-bA-B][0-9a-fA-F]{5}) (0x[a-fA-F0-9]+) RW')
    elif dut.target == 'esp32s2':
        # ESP32-S2 External data memory range [0x3f500000-0x3ff80000)
        coredump_pattern = re.compile(
            '.coredump.tasks.data (0x3[fF][5-9a-fA-F][0-7][0-9a-fA-F]{4}) (0x[a-fA-F0-9]+) RW'
        )
    else:
        # ESP32-S3 External data memory range [0x3c000000-0x3e000000)
        coredump_pattern = re.compile('.coredump.tasks.data (0x3[c-dC-D][0-9a-fA-F]{6}) (0x[a-fA-F0-9]+) RW')

    common_test(dut, config, expected_backtrace=None, expected_coredump=[coredump_pattern])


@pytest.mark.psram
@idf_parametrize('config, target', [('coredump_flash_extram_attr_esp32', 'esp32')], indirect=['config', 'target'])
def test_panic_extram_attr(dut: PanicTestDut, config: str) -> None:
    dut.run_test_func('test_panic_extram_attr')
    regex_pattern = rb'assert failed:[\s\w()]*?\s[.\w/]*\.(?:c|cpp|h|hpp):\d.*$'
    dut.expect(re.compile(regex_pattern, re.MULTILINE))
    dut.expect_backtrace()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])

    expect_coredump_flash_write_logs(dut, config)
    core_elf_file = dut.process_coredump_flash()

    dut.start_gdb_for_coredump(core_elf_file)

    assert dut.gdb_data_eval_expr('g_extram_bss_var') == '123456'
    assert dut.gdb_data_eval_expr('g_extram_noinit_var') == '789012'


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_int_wdt(dut: PanicTestDut, target: str, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Interrupt wdt timeout on CPU0')
    dut.expect_reg_dump(0)
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()

    if target in TARGETS_DUAL_CORE:
        dut.expect_reg_dump(1)
        if dut.is_xtensa:
            dut.expect_backtrace()

    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')

    common_test(dut, config, expected_backtrace=get_default_backtrace(test_func_name))


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_int_wdt_cache_disabled(dut: PanicTestDut, target: str, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Interrupt wdt timeout on CPU0')
    dut.expect_reg_dump(0)
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()

    if target in TARGETS_DUAL_CORE:
        dut.expect_reg_dump(1)
        if dut.is_xtensa:
            dut.expect_backtrace()

    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')

    common_test(dut, config, expected_backtrace=get_default_backtrace(test_func_name))


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_cache_error(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    if dut.target in ['esp32c3', 'esp32c2']:
        dut.expect_gme('Cache error')
        dut.expect_exact('Cached memory region accessed while ibus or cache is disabled')
    elif dut.target in ['esp32s2']:
        # Cache error interrupt is not enabled, IDF-1558
        dut.expect_gme('IllegalInstruction')
    elif dut.target in ['esp32', 'esp32s3']:
        dut.expect_gme('Cache error')
        dut.expect_exact('Cache disabled but cached memory region accessed')
    else:
        dut.expect_gme('Cache error')
        dut.expect_exact('Cache access error')
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

    common_test(dut, config, expected_backtrace=expected_backtrace, check_cpu_reset=(dut.target != 'esp32'))


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
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


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_instr_fetch_prohibited(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    if dut.is_xtensa:
        dut.expect_gme('InstrFetchProhibited')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        expected_backtrace = ['_invalid_pc_placeholder'] + get_default_backtrace(test_func_name)
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


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_illegal_instruction(dut: PanicTestDut, config: str, test_func_name: str) -> None:
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


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_storeprohibited(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    check_x_prohibited(dut, config, test_func_name, 'Store')


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_loadprohibited(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    check_x_prohibited(dut, config, test_func_name, 'Load')


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
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
        expected_backtrace=['panic_abort', 'esp_system_abort', 'abort'] + get_default_backtrace(test_func_name),
        expected_coredump=[coredump_pattern],
    )


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_UBSAN, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
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

    common_test(
        dut,
        config,
        expected_backtrace=[
            'panic_abort',
            'esp_system_abort',
            '__ubsan_default_handler',
            '__ubsan_handle_out_of_bounds',
        ]
        + get_default_backtrace(test_func_name),
    )


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_abort_cache_disabled(dut: PanicTestDut, config: str, test_func_name: str) -> None:
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
        expected_backtrace=['panic_abort', 'esp_system_abort', 'abort'] + get_default_backtrace(test_func_name),
        expected_coredump=[coredump_pattern],
    )


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
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
        expected_backtrace=['panic_abort', 'esp_system_abort', '__assert_func'] + get_default_backtrace(test_func_name),
        expected_coredump=[coredump_pattern],
    )


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_assert_cache_disabled(dut: PanicTestDut, config: str, test_func_name: str) -> None:
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
        expected_backtrace=['panic_abort', 'esp_system_abort', '__assert_func'] + get_default_backtrace(test_func_name),
        expected_coredump=[coredump_pattern],
    )


def cache_error_log_check(dut: PanicTestDut) -> None:
    if dut.is_xtensa:
        if dut.target == 'esp32s3':
            dut.expect_exact("Guru Meditation Error: Core  0 panic'ed (Cache error)")
            dut.expect_exact('Dbus write to cache rejected, error address')
        else:
            dut.expect_exact("Guru Meditation Error: Core  0 panic'ed (LoadStoreError)")
    else:
        dut.expect_exact("Guru Meditation Error: Core  0 panic'ed (Store access fault)")
    dut.expect_reg_dump(0)
    dut.expect_cpu_reset()


@pytest.mark.generic
@idf_parametrize('config, target', CONFIG_PANIC, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_assert_cache_write_back_error_can_print_backtrace(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    cache_error_log_check(dut)


@pytest.mark.generic
@pytest.mark.parametrize('config', ['panic_delay'], indirect=True)
@idf_parametrize('target', ['esp32'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
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


@pytest.mark.generic
@idf_parametrize('config, target', CONFIG_PANIC, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_panic_handler_stuck0(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)

    # Expect a panic handler stuck message
    dut.expect_exact('Panic handler stuck')

    # Expect a reboot
    dut.expect_cpu_reset()


@pytest.mark.generic
@idf_parametrize('config, target', CONFIG_PANIC_DUAL_CORE, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_panic_handler_stuck1(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)

    # Expect a panic handler stuck message
    dut.expect_exact('Panic handler stuck')

    # Expect a reboot
    dut.expect_cpu_reset()


@pytest.mark.generic
@idf_parametrize('config, target', CONFIG_PANIC, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_panic_handler_crash0(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)

    # Expect a panic handler crash message
    dut.expect_exact('Panic handler crashed 1 times')

    # Expect a the second panic handler crash message
    dut.expect_exact('Panic handler crashed 2 times')

    # Expect bailout message
    dut.expect_exact('Panic handler entered multiple times. Abort panic handling. Rebooting ...')

    # Expect a reboot
    dut.expect_cpu_reset()


@pytest.mark.generic
@idf_parametrize('config, target', CONFIG_PANIC_DUAL_CORE, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_panic_handler_crash1(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)

    # Expect a panic handler crash message
    dut.expect_exact('Panic handler crashed 1 times')

    # Expect a the second panic handler crash message
    dut.expect_exact('Panic handler crashed 2 times')

    # Expect bailout message
    dut.expect_exact('Panic handler entered multiple times. Abort panic handling. Rebooting ...')

    # Expect a reboot
    dut.expect_cpu_reset()


#########################
# for memprot test only #
#########################

# Memprot-related tests are supported only on targets with PMS/PMA peripheral;
# currently ESP32-S2, ESP32-C3, ESP32-C2, ESP32-H2, ESP32-H21, ESP32-C6, ESP32-P4, ESP32-C5 and ESP32-C61 are supported
CONFIGS_MEMPROT_IDRAM = list(
    zip(
        [
            'memprot_esp32s2',
            'memprot_esp32c3',
            'memprot_esp32c2',
            'memprot_esp32c5',
            'memprot_esp32c61',
            'memprot_esp32h2',
            'memprot_esp32p4',
            'memprot_esp32h21',
        ],
        ['esp32s2', 'esp32c3', 'esp32c2', 'esp32c5', 'esp32c61', 'esp32h2', 'esp32p4', 'esp32h21'],
    )
)

CONFIGS_MEMPROT_IDRAM_L2_MEM_NON_CACHE = list(zip(['memprot_esp32p4'], ['esp32p4']))

CONFIGS_MEMPROT_DCACHE = list(zip(['memprot_esp32s2'], ['esp32s2']))

CONFIGS_MEMPROT_RTC_FAST_MEM = list(
    zip(
        [
            'memprot_esp32s2',
            'memprot_esp32c3',
            'memprot_esp32c5',
            'memprot_esp32c6',
            'memprot_esp32h2',
            'memprot_esp32p4',
            'memprot_esp32h21',
        ],
        ['esp32s2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4', 'esp32h21'],
    )
)

CONFIGS_MEMPROT_RTC_SLOW_MEM = list(zip(['memprot_esp32s2'], ['esp32s2']))

CONFIGS_MEMPROT_FLASH_IDROM = list(
    zip(
        [
            'memprot_esp32c5',
            'memprot_esp32c6',
            'memprot_esp32c61',
            'memprot_esp32h2',
            'memprot_esp32p4',
            'memprot_esp32h21',
        ],
        ['esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32p4', 'esp32h21'],
    )
)

CONFIGS_MEMPROT_FLASH_IDROM_L2_NON_CACHE = list(zip(['memprot_esp32p4'], ['esp32p4']))

CONFIGS_MEMPROT_SPIRAM_XIP_IROM_ALIGNMENT_HEAP = list(
    zip(
        ['memprot_spiram_xip_esp32c5', 'memprot_spiram_xip_esp32c61', 'memprot_spiram_xip_esp32p4'],
        ['esp32c5', 'esp32c61', 'esp32p4'],
    )
)

CONFIGS_MEMPROT_SPIRAM_XIP_IROM_ALIGNMENT_HEAP_L2_NON_CACHE = list(
    zip(
        ['memprot_spiram_xip_esp32p4'],
        ['esp32p4'],
    )
)


CONFIGS_MEMPROT_SPIRAM_XIP_DROM_ALIGNMENT_HEAP = list(
    zip(
        [
            'memprot_spiram_xip_esp32s3',
            'memprot_spiram_xip_esp32c5',
            'memprot_spiram_xip_esp32c61',
            'memprot_spiram_xip_esp32p4',
        ],
        ['esp32s3', 'esp32c5', 'esp32c61', 'esp32p4'],
    )
)

CONFIGS_MEMPROT_SPIRAM_XIP_DROM_ALIGNMENT_HEAP_L2_NON_CACHE = list(
    zip(
        ['memprot_spiram_xip_esp32p4'],
        ['esp32p4'],
    )
)

CONFIGS_MEMPROT_INVALID_REGION_PROTECTION_USING_PMA = list(
    zip(
        [
            'memprot_esp32c5',
            'memprot_esp32c6',
            'memprot_esp32c61',
            'memprot_esp32h2',
            'memprot_esp32p4',
            'memprot_esp32h21',
        ],
        ['esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32p4', 'esp32h21'],
    )
)


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_MEMPROT_DCACHE, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_dcache_read_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_exact(r'Test error: Test function has returned')
    dut.expect_cpu_reset()


# TODO: IDF-6820: ESP32-S2 -> Fix multiple panic reasons in different runs
@pytest.mark.generic
@pytest.mark.xfail(targets=['esp32s2'], reason='Incorrect panic reason may be observed', run=False)
@idf_parametrize('config, target', CONFIGS_MEMPROT_DCACHE, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_dcache_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Memory protection fault')
    dut.expect(r'Write operation at address [0-9xa-f]+ not permitted \((\S+)\)')
    dut.expect_reg_dump(0)
    dut.expect_backtrace()
    dut.expect_cpu_reset()


def iram_reg1_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)

    if dut.target == 'esp32s2':
        dut.expect_gme('Memory protection fault')
        dut.expect(r'Write operation at address [0-9xa-f]+ not permitted \((\S+)\)')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
    elif dut.target == 'esp32c3':
        dut.expect_exact(r'Test error: Test function has returned')
    else:
        dut.expect_gme('Store access fault')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()

    dut.expect_cpu_reset()


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners')
@idf_parametrize('config, target', CONFIGS_MEMPROT_IDRAM, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_iram_reg1_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    iram_reg1_write_violation(dut, test_func_name)


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_MEMPROT_IDRAM_L2_MEM_NON_CACHE, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_non_cache_iram_reg1_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    if dut.target == 'esp32p4' and not dut.app.sdkconfig.get('ESP32P4_SELECTS_REV_LESS_V3'):
        iram_reg1_write_violation(dut, test_func_name)


def iram_reg_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
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
    else:
        dut.expect_gme('Store access fault')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()

    dut.expect_cpu_reset()


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners')
@idf_parametrize('config, target', CONFIGS_MEMPROT_IDRAM, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_iram_reg2_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    iram_reg_write_violation(dut, test_func_name)


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_MEMPROT_IDRAM_L2_MEM_NON_CACHE, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_non_cache_iram_reg2_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    if dut.target == 'esp32p4' and not dut.app.sdkconfig.get('ESP32P4_SELECTS_REV_LESS_V3'):
        iram_reg_write_violation(dut, test_func_name)


def iram_reg3_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
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
    else:
        dut.expect_gme('Store access fault')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()

    dut.expect_cpu_reset()


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners')
@idf_parametrize('config, target', CONFIGS_MEMPROT_IDRAM, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_iram_reg3_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    iram_reg_write_violation(dut, test_func_name)


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_MEMPROT_IDRAM_L2_MEM_NON_CACHE, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_non_cache_iram_reg3_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    if dut.target == 'esp32p4' and not dut.app.sdkconfig.get('ESP32P4_SELECTS_REV_LESS_V3'):
        iram_reg_write_violation(dut, test_func_name)


def iram_reg4_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
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
    else:
        dut.expect_gme('Store access fault')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()

    dut.expect_cpu_reset()


# TODO: IDF-6820: ESP32-S2 -> Fix incorrect panic reason: Unhandled debug exception
@pytest.mark.generic
@pytest.mark.xfail(targets=['esp32s2'], reason='Incorrect panic reason may be observed', run=False)
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners')
@idf_parametrize('config, target', CONFIGS_MEMPROT_IDRAM, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_iram_reg4_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    iram_reg_write_violation(dut, test_func_name)


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_MEMPROT_IDRAM_L2_MEM_NON_CACHE, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_non_cache_iram_reg4_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    if dut.target == 'esp32p4' and not dut.app.sdkconfig.get('ESP32P4_SELECTS_REV_LESS_V3'):
        iram_reg_write_violation(dut, test_func_name)


def dram_reg1_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)

    if dut.target == 'esp32s2':
        dut.expect_gme('Memory protection fault')
        dut.expect(r'Unknown operation at address [0-9xa-f]+ not permitted \((\S+)\)')
        dut.expect_reg_dump(0)
        dut.expect_backtrace(corrupted=True)
    else:
        dut.expect_gme('Instruction access fault')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()

    dut.expect_cpu_reset()


# TODO: IDF-6820: ESP32-S2 -> Fix multiple panic reasons in different runs
@pytest.mark.generic
@pytest.mark.xfail(targets=['esp32s2'], reason='Multiple panic reasons for the same test may surface', run=False)
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners')
@idf_parametrize('config, target', CONFIGS_MEMPROT_IDRAM, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_dram_reg1_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dram_reg1_execute_violation(dut, test_func_name)


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_MEMPROT_IDRAM_L2_MEM_NON_CACHE, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_non_cache_dram_reg1_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    if dut.target == 'esp32p4' and not dut.app.sdkconfig.get('ESP32P4_SELECTS_REV_LESS_V3'):
        dram_reg1_execute_violation(dut, test_func_name)


def dram_reg2_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)

    if dut.target == 'esp32s2':
        dut.expect_gme('InstructionFetchError')
        dut.expect_reg_dump(0)
        dut.expect_backtrace(corrupted=True)
    else:
        dut.expect_gme('Instruction access fault')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()

    dut.expect_cpu_reset()


# TODO: IDF-6820: ESP32-S2 -> Fix multiple panic reasons in different runs
@pytest.mark.generic
@pytest.mark.xfail(targets=['esp32s2'], reason='Multiple panic reasons for the same test may surface', run=False)
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners')
@idf_parametrize('config, target', CONFIGS_MEMPROT_IDRAM, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_dram_reg2_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dram_reg2_execute_violation(dut, test_func_name)


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_MEMPROT_IDRAM_L2_MEM_NON_CACHE, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_non_cache_dram_reg2_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    if dut.target == 'esp32p4' and not dut.app.sdkconfig.get('ESP32P4_SELECTS_REV_LESS_V3'):
        dram_reg2_execute_violation(dut, test_func_name)


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners')
@idf_parametrize('config, target', CONFIGS_MEMPROT_RTC_FAST_MEM, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_rtc_fast_reg1_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_exact(r'Test error: Test function has returned')
    dut.expect_cpu_reset()


@pytest.mark.generic
@pytest.mark.temp_skip(
    targets=['esp32c5', 'esp32c6', 'esp32h2', 'esp32p4', 'esp32h21'],
    reason='Not a violation condition, no PMS peripheral cases',
)
@idf_parametrize('config, target', CONFIGS_MEMPROT_RTC_FAST_MEM, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
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
@pytest.mark.generic
@pytest.mark.xfail(targets=['esp32s2'], reason='Multiple panic reasons for the same test may surface', run=False)
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners')
@idf_parametrize('config, target', CONFIGS_MEMPROT_RTC_FAST_MEM, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
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
    else:
        dut.expect_gme('Instruction access fault')
        dut.expect_reg_dump(0)
        dut.expect_stack_dump()

    dut.expect_cpu_reset()


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_MEMPROT_RTC_SLOW_MEM, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_rtc_slow_reg1_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Memory protection fault')
    dut.expect(r'Read operation at address [0-9xa-f]+ not permitted \((\S+)\)')
    dut.expect_reg_dump(0)
    dut.expect_backtrace(corrupted=True)
    dut.expect_cpu_reset()


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_MEMPROT_RTC_SLOW_MEM, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_rtc_slow_reg2_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Memory protection fault')
    dut.expect(r'Read operation at address [0-9xa-f]+ not permitted \((\S+)\)')
    dut.expect_reg_dump(0)
    dut.expect_backtrace(corrupted=True)
    dut.expect_cpu_reset()


def irom_reg_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Store access fault')
    dut.expect_reg_dump(0)
    dut.expect_cpu_reset()


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners')
@idf_parametrize('config, target', CONFIGS_MEMPROT_FLASH_IDROM, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_irom_reg_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    irom_reg_write_violation(dut, test_func_name)


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_MEMPROT_FLASH_IDROM_L2_NON_CACHE, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_non_cache_irom_reg_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    if dut.target == 'esp32p4' and not dut.app.sdkconfig.get('ESP32P4_SELECTS_REV_LESS_V3'):
        irom_reg_write_violation(dut, test_func_name)


def drom_reg_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Store access fault')
    dut.expect_reg_dump(0)
    dut.expect_cpu_reset()


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners')
@idf_parametrize('config, target', CONFIGS_MEMPROT_FLASH_IDROM, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_drom_reg_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    drom_reg_write_violation(dut, test_func_name)


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_MEMPROT_FLASH_IDROM_L2_NON_CACHE, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_non_cache_drom_reg_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    if dut.target == 'esp32p4' and not dut.app.sdkconfig.get('ESP32P4_SELECTS_REV_LESS_V3'):
        drom_reg_write_violation(dut, test_func_name)


def drom_reg_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Instruction access fault')
    dut.expect_reg_dump(0)
    dut.expect_cpu_reset()


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners')
@idf_parametrize('config, target', CONFIGS_MEMPROT_FLASH_IDROM, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_drom_reg_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    drom_reg_execute_violation(dut, test_func_name)


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_MEMPROT_FLASH_IDROM_L2_NON_CACHE, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_non_cache_drom_reg_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    if dut.target == 'esp32p4' and not dut.app.sdkconfig.get('ESP32P4_SELECTS_REV_LESS_V3'):
        drom_reg_execute_violation(dut, test_func_name)


def spiram_xip_irom_alignment_reg_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    try:
        dut.expect_gme('Instruction access fault')
    except Exception:
        dut.expect_exact('SPIRAM (IROM): IROM alignment gap not added into heap')
    dut.expect_reg_dump(0)
    dut.expect_cpu_reset()


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32c5'], reason='TODO IDF-14835')
@idf_parametrize('config, target', CONFIGS_MEMPROT_SPIRAM_XIP_IROM_ALIGNMENT_HEAP, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_spiram_xip_irom_alignment_reg_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    spiram_xip_irom_alignment_reg_execute_violation(dut, test_func_name)


@pytest.mark.generic
@idf_parametrize(
    'config, target', CONFIGS_MEMPROT_SPIRAM_XIP_IROM_ALIGNMENT_HEAP_L2_NON_CACHE, indirect=['config', 'target']
)
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_non_cache_spiram_xip_irom_alignment_reg_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    if dut.target == 'esp32p4' and not dut.app.sdkconfig.get('ESP32P4_SELECTS_REV_LESS_V3'):
        spiram_xip_irom_alignment_reg_execute_violation(dut, test_func_name)


def spiram_xip_drom_alignment_reg_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    try:
        if dut.target == 'esp32s3':
            dut.expect_gme('InstructionFetchError')
        else:
            dut.expect_gme('Instruction access fault')
    except Exception:
        dut.expect_exact('SPIRAM (DROM): DROM alignment gap not added into heap')
    dut.expect_reg_dump(0)
    dut.expect_cpu_reset()


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32c5'], reason='TODO IDF-14835')
@idf_parametrize('config, target', CONFIGS_MEMPROT_SPIRAM_XIP_DROM_ALIGNMENT_HEAP, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_spiram_xip_drom_alignment_reg_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    spiram_xip_drom_alignment_reg_execute_violation(dut, test_func_name)


@pytest.mark.generic
@idf_parametrize(
    'config, target', CONFIGS_MEMPROT_SPIRAM_XIP_DROM_ALIGNMENT_HEAP_L2_NON_CACHE, indirect=['config', 'target']
)
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_non_cache_spiram_xip_drom_alignment_reg_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    if dut.target == 'esp32p4' and not dut.app.sdkconfig.get('ESP32P4_SELECTS_REV_LESS_V3'):
        spiram_xip_drom_alignment_reg_execute_violation(dut, test_func_name)


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners')
@idf_parametrize('config, target', CONFIGS_MEMPROT_INVALID_REGION_PROTECTION_USING_PMA, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_invalid_memory_region_write_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Store access fault')
    dut.expect_reg_dump(0)
    dut.expect_cpu_reset()


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners')
@idf_parametrize('config, target', CONFIGS_MEMPROT_INVALID_REGION_PROTECTION_USING_PMA, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_invalid_memory_region_execute_violation(dut: PanicTestDut, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    dut.expect_gme('Instruction access fault')
    dut.expect_reg_dump(0)
    dut.expect_cpu_reset()


@pytest.mark.generic
@pytest.mark.parametrize('config', ['gdbstub_coredump'], indirect=True)
@idf_parametrize('target', ['esp32'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_gdbstub_coredump(dut: PanicTestDut) -> None:
    test_func_name = 'test_storeprohibited'
    dut.run_test_func(test_func_name)
    common_test(dut, 'gdbstub_coredump', get_default_backtrace(test_func_name))


def test_hw_stack_guard_cpu(dut: PanicTestDut, cpu: int) -> None:
    dut.expect_exact(f"Guru Meditation Error: Core  {cpu} panic'ed (Stack protection fault).")
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


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_HW_STACK_GUARD, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_hw_stack_guard_cpu0(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    test_hw_stack_guard_cpu(dut, 0)
    common_test(dut, config)


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_HW_STACK_GUARD_DUAL_CORE, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_hw_stack_guard_cpu1(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    test_hw_stack_guard_cpu(dut, 1)
    common_test(dut, config)


@pytest.mark.parametrize('config', ['panic'], indirect=True)
@pytest.mark.generic
@idf_parametrize('target', ['esp32'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_illegal_access(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    if dut.is_xtensa:
        dut.expect(r'\[1\] val: (-?\d+) at 0x80000000', timeout=30)
        dut.expect_gme('LoadProhibited')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation')


@pytest.mark.generic
@idf_parametrize('config, target', CONFIG_CAPTURE_DRAM, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_capture_dram(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    regex_pattern = rb'assert failed:[\s\w()]*?\s[.\w/]*\.(?:c|cpp|h|hpp):\d.*$'
    dut.expect(re.compile(regex_pattern, re.MULTILINE))
    if dut.is_xtensa:
        dut.expect_backtrace()
    else:
        dut.expect_stack_dump()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])

    core_elf_file = None
    if 'flash' in config:
        expect_coredump_flash_write_logs(dut, config)
        core_elf_file = dut.process_coredump_flash()
    elif 'uart' in config:
        coredump_base64 = expect_coredump_uart_write_logs(dut)
        core_elf_file = dut.process_coredump_uart(coredump_base64)
    assert core_elf_file is not None

    dut.start_gdb_for_coredump(core_elf_file)

    assert dut.gdb_data_eval_expr('g_data_var') == '43'
    assert dut.gdb_data_eval_expr('g_bss_var') == '55'
    assert re.search(r'0x[0-9a-fA-F]+ "Coredump Test"', dut.gdb_data_eval_expr('g_heap_ptr'))
    assert int(dut.gdb_data_eval_expr('g_cd_iram')) == 0x4243
    assert int(dut.gdb_data_eval_expr('g_cd_dram')) == 0x4344
    assert int(dut.gdb_data_eval_expr('g_noinit_var')) == 0xCAFEBABE
    buffer_value = str(dut.gdb_data_eval_expr('g_noinit_buffer'))
    assert 'NOINIT_TEST_STRING' in buffer_value

    if dut.target not in ['esp32c61', 'esp32c2']:
        assert int(dut.gdb_data_eval_expr('g_rtc_data_var')) == 0x55AA
        assert int(dut.gdb_data_eval_expr('g_rtc_fast_var')) == 0xAABBCCDD


def _test_coredump_summary(dut: PanicTestDut, flash_encrypted: bool, coredump_encrypted: bool) -> None:
    dut.run_test_func('test_setup_coredump_summary')
    dut.expect_cpu_reset()
    if flash_encrypted:
        dut.expect_exact('Flash encryption mode is DEVELOPMENT (not secure)')
    dut.run_test_func('test_coredump_summary')
    if flash_encrypted and not coredump_encrypted:
        dut.expect_exact('Flash encryption enabled in hardware and core dump partition is not encrypted!')
        return
    dut.expect_elf_sha256('App ELF file SHA256: ')
    dut.expect_exact('Crashed task: main')
    if dut.is_xtensa:
        dut.expect_exact('Exception cause: 0')
    else:
        dut.expect_exact('Exception cause: 2')
    dut.expect(PANIC_ABORT_PREFIX + r'assert failed:[\s\w()]*?\s[.\w/]*\.(?:c|cpp|h|hpp):\d.*$')


@pytest.mark.generic
@idf_parametrize('config, target', CONFIG_COREDUMP_SUMMARY, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_coredump_summary(dut: PanicTestDut) -> None:
    _test_coredump_summary(dut, False, False)


@pytest.mark.flash_encryption
@idf_parametrize('config, target', CONFIG_COREDUMP_SUMMARY_FLASH_ENCRYPTED, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_coredump_summary_flash_encrypted(dut: PanicTestDut, config: str) -> None:
    _test_coredump_summary(dut, True, config == 'coredump_flash_encrypted')


@pytest.mark.generic
@idf_parametrize('config', ['coredump_flash_default'], indirect=['config'])
@idf_parametrize('target', TARGETS_ALL, indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_tcb_corrupted(dut: PanicTestDut, target: str, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    if dut.is_xtensa:
        dut.expect(re.compile(rb"Guru Meditation Error: Core\s+\d\s+panic'ed \((LoadProhibited|StoreProhibited)\)"))
        dut.expect_reg_dump()
        dut.expect_backtrace()
    else:
        dut.expect(re.compile(rb"Guru Meditation Error: Core\s+\d\s+panic'ed \((Load|Store) access fault\)"))
        dut.expect_reg_dump()
        dut.expect_stack_dump()

    dut.expect_elf_sha256()
    dut.expect_none('Guru Meditation')

    # Verify that valid tasks are captured in coredump despite IDLE task corruption
    #        TCB             NAME
    # ---------- ----------------
    if dut.is_multi_core:
        regex_patterns = [
            rb'[0-9xa-fA-F]             main',
            rb'[0-9xa-fA-F]             ipc0',
            rb'[0-9xa-fA-F]             ipc1',
        ]
    else:
        regex_patterns = [rb'[0-9xa-fA-F]             main']

    coredump_pattern = [re.compile(pattern.decode('utf-8')) for pattern in regex_patterns]

    common_test(dut, config, expected_backtrace=None, expected_coredump=coredump_pattern)


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_BACKTRACE, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_panic_print_backtrace(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    dut.run_test_func(test_func_name)
    regex_pattern = rb'abort\(\) was called at PC [0-9xa-f]+ on core 0'
    dut.expect(regex_pattern)
    dut.expect_backtrace()
    dut.expect_elf_sha256()
    dut.expect_none(['Guru Meditation', 'Re-entered core dump'])

    coredump_pattern = re.compile(PANIC_ABORT_PREFIX + regex_pattern.decode('utf-8'))
    common_test(dut, config, expected_backtrace=None, expected_coredump=[coredump_pattern])


@pytest.mark.generic
@idf_parametrize('config, target', CONFIG_PANIC_HALT, indirect=['config', 'target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_panic_halt(dut: PanicTestDut) -> None:
    dut.run_test_func('test_panic_halt')
    dut.expect_exact('CPU halted.', timeout=30)
    dut.expect_none(dut.REBOOT, timeout=3)
