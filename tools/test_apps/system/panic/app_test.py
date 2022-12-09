#!/usr/bin/env python
import re
from test_panic_util.test_panic_util import panic_test, get_dut, run_all


@panic_test()
def test_panic_task_wdt(env, extra_data):
    with get_dut(env, "panic", "test_task_wdt", qemu_wdt_enable=True) as dut:
        dut.expect("Task watchdog got triggered. The following tasks did not reset the watchdog in time:")
        dut.expect("CPU 0: main")
        dut.expect(re.compile(r"abort\(\) was called at PC [0-9xa-f]+ on core 0"))
        dut.expect_none("register dump:")
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("CORRUPTED", "Guru Meditation")
        dut.expect("Rebooting...")


@panic_test()
def test_panic_int_wdt(env, extra_data):
    with get_dut(env, "panic", "test_int_wdt", qemu_wdt_enable=True) as dut:
        dut.expect_gme("Interrupt wdt timeout on CPU0")
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_none("Guru Meditation")
        dut.expect_reg_dump(1)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation")
        dut.expect("Rebooting...")


@panic_test()
def test_cache_error(env, extra_data):
    with get_dut(env, "panic", "test_cache_error") as dut:
        dut.expect("Re-enable cpu cache.")
        dut.expect_gme("Cache disabled but cached memory region accessed")
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation")
        dut.expect("Rebooting...")


@panic_test()
def test_panic_int_wdt_cache_disabled(env, extra_data):
    with get_dut(env, "panic", "test_int_wdt_cache_disabled", qemu_wdt_enable=True) as dut:
        dut.expect("Re-enable cpu cache.")
        dut.expect_gme("Interrupt wdt timeout on CPU0")
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_none("Guru Meditation")
        dut.expect_reg_dump(1)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation")
        dut.expect("Rebooting...")


@panic_test()
def test_panic_abort(env, extra_data):
    with get_dut(env, "panic", "test_abort") as dut:
        dut.expect(re.compile(r"abort\(\) was called at PC [0-9xa-f]+ on core 0"))
        dut.expect_none("register dump:")
        dut.expect_backtrace()
        dut.expect_none("Guru Meditation")
        dut.expect("Rebooting...")


@panic_test()
def test_panic_storeprohibited(env, extra_data):
    with get_dut(env, "panic", "test_storeprohibited") as dut:
        dut.expect_gme("StoreProhibited")
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation")
        dut.expect("Rebooting...")


@panic_test()
def test_panic_stack_overflow(env, extra_data):
    with get_dut(env, "panic", "test_stack_overflow") as dut:
        dut.expect_gme("Unhandled debug exception")
        dut.expect("Stack canary watchpoint triggered (main)")
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation")
        dut.expect("Rebooting...")


@panic_test()
def test_panic_illegal_instruction(env, extra_data):
    with get_dut(env, "panic", "test_illegal_instruction") as dut:
        dut.expect_gme("IllegalInstruction")
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation")
        dut.expect("Rebooting...")


@panic_test()
def test_panic_instr_fetch_prohibited(env, extra_data):
    with get_dut(env, "panic", "test_instr_fetch_prohibited") as dut:
        dut.expect_gme("InstrFetchProhibited")
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation")
        dut.expect("Rebooting...")


@panic_test()
def test_coredump_uart_abort(env, extra_data):
    with get_dut(env, "coredump_uart", "test_abort") as dut:
        dut.expect(re.compile(r"abort\(\) was called at PC [0-9xa-f]+ on core 0"))
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation", "Re-entered core dump")
        dut.expect(dut.COREDUMP_UART_END)
        dut.expect("Rebooting...")
        dut.process_coredump_uart()
        # TODO: check the contents of core dump output


@panic_test()
def test_coredump_uart_int_wdt(env, extra_data):
    with get_dut(env, "coredump_uart", "test_int_wdt") as dut:
        dut.expect_gme("Interrupt wdt timeout on CPU0")
        dut.expect_reg_dump(0)
        dut.expect("Backtrace:")
        dut.expect_none("CORRUPTED", "Guru Meditation")
        dut.expect_reg_dump(1)
        dut.expect("Backtrace:")
        dut.expect_elf_sha256()
        dut.expect_none("CORRUPTED", "Guru Meditation")
        dut.expect("Rebooting...")
        dut.process_coredump_uart()
        # TODO: check the contents of core dump output


@panic_test()
def test_coredump_flash_abort(env, extra_data):
    with get_dut(env, "coredump_flash", "test_abort") as dut:
        dut.expect(re.compile(r"abort\(\) was called at PC [0-9xa-f]+ on core 0"))
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none("Guru Meditation", "Re-entered core dump")
        dut.expect("Rebooting...")
        dut.process_coredump_flash()
        # TODO: check the contents of core dump output


@panic_test()
def test_coredump_flash_int_wdt(env, extra_data):
    with get_dut(env, "coredump_flash", "test_int_wdt") as dut:
        dut.expect_gme("Interrupt wdt timeout on CPU0")
        dut.expect_reg_dump(0)
        dut.expect("Backtrace:")
        dut.expect_none("CORRUPTED", "Guru Meditation")
        dut.expect_reg_dump(1)
        dut.expect("Backtrace:")
        dut.expect_elf_sha256()
        dut.expect_none("CORRUPTED", "Guru Meditation")
        dut.expect("Rebooting...")
        dut.process_coredump_flash()
        # TODO: check the contents of core dump output


if __name__ == '__main__':
    run_all(__file__)
