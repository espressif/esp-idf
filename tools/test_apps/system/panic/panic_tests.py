#!/usr/bin/env python
import re
from test_panic_util.test_panic_util import get_dut


def test_common(dut, test_name):
    if "uart" in test_name:
        dut.expect(dut.COREDUMP_UART_END)
    dut.expect("Rebooting...")
    if "uart" in test_name:
        dut.process_coredump_uart
    elif "flash" in test_name:
        dut.process_coredump_flash


def task_wdt_inner(env, test_name):
    with get_dut(env, test_name, "test_task_wdt", qemu_wdt_enable=True) as dut:
        dut.expect("Task watchdog got triggered. The following tasks did not reset the watchdog in time:")
        dut.expect("CPU 0: main")
        dut.expect(re.compile(r"abort\(\) was called at PC [0-9xa-f]+ on core 0"))
        dut.expect_none("register dump:")
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation")
        test_common(dut, test_name)


def int_wdt_inner(env, test_name):
    with get_dut(env, test_name, "test_int_wdt", qemu_wdt_enable=True) as dut:
        dut.expect_gme("Interrupt wdt timeout on CPU0")
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_none("Guru Meditation")
        dut.expect_reg_dump(1)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation")
        test_common(dut, test_name)


def int_wdt_cache_disabled_inner(env, test_name):
    with get_dut(env, test_name, "test_int_wdt_cache_disabled", qemu_wdt_enable=True) as dut:
        dut.expect("Re-enable cpu cache.")
        dut.expect_gme("Interrupt wdt timeout on CPU0")
        dut.expect_reg_dump(0)
        dut.expect("Backtrace:")
        dut.expect_none("Guru Meditation")
        dut.expect_reg_dump(1)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation")
        test_common(dut, test_name)


def cache_error_inner(env, test_name):
    with get_dut(env, test_name, "test_cache_error") as dut:
        dut.expect("Re-enable cpu cache.")
        dut.expect_gme("Cache disabled but cached memory region accessed")
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation")
        test_common(dut, test_name)


def abort_inner(env, test_name):
    with get_dut(env, test_name, "test_abort") as dut:
        dut.expect(re.compile(r"abort\(\) was called at PC [0-9xa-f]+ on core 0"))
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation", "Re-entered core dump")
        test_common(dut, test_name)


def storeprohibited_inner(env, test_name):
    with get_dut(env, test_name, "test_storeprohibited") as dut:
        dut.expect_gme("StoreProhibited")
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation")
        test_common(dut, test_name)


def stack_overflow_inner(env, test_name):
    with get_dut(env, test_name, "test_stack_overflow") as dut:
        dut.expect_gme("Unhandled debug exception")
        dut.expect("Stack canary watchpoint triggered (main)")
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation")
        test_common(dut, test_name)


def illegal_instruction_inner(env, test_name):
    with get_dut(env, test_name, "test_illegal_instruction") as dut:
        dut.expect_gme("IllegalInstruction")
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation")
        test_common(dut, test_name)


def instr_fetch_prohibited_inner(env, test_name):
    with get_dut(env, test_name, "test_instr_fetch_prohibited") as dut:
        dut.expect_gme("InstrFetchProhibited")
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation")
        test_common(dut, test_name)
