#!/usr/bin/env python
import sys

import panic_tests as test
from test_panic_util.test_panic_util import panic_test, run_all


# test_task_wdt
@panic_test(target=['ESP32', 'ESP32S2'])
def test_panic_task_wdt(env, _extra_data):
    test.task_wdt_inner(env, 'panic')


@panic_test()
def test_coredump_task_wdt_uart_elf_crc(env, _extra_data):
    test.task_wdt_inner(env, 'coredump_uart_elf_crc')


@panic_test()
def test_coredump_task_wdt_uart_bin_crc(env, _extra_data):
    test.task_wdt_inner(env, 'coredump_uart_bin_crc')


@panic_test()
def test_coredump_task_wdt_flash_elf_sha(env, _extra_data):
    test.task_wdt_inner(env, 'coredump_flash_elf_sha')


@panic_test()
def test_coredump_task_wdt_flash_bin_crc(env, _extra_data):
    test.task_wdt_inner(env, 'coredump_flash_bin_crc')


@panic_test()
def test_gdbstub_task_wdt(env, _extra_data):
    test.task_wdt_inner(env, 'gdbstub')


# test_int_wdt

@panic_test()
def test_panic_int_wdt(env, _extra_data):
    test.int_wdt_inner(env, 'panic')


@panic_test()
def test_coredump_int_wdt_uart_elf_crc(env, _extra_data):
    test.int_wdt_inner(env, 'coredump_uart_elf_crc')


@panic_test()
def test_coredump_int_wdt_uart_bin_crc(env, _extra_data):
    test.int_wdt_inner(env, 'coredump_uart_bin_crc')


@panic_test()
def test_coredump_int_wdt_flash_elf_sha(env, _extra_data):
    test.int_wdt_inner(env, 'coredump_flash_elf_sha')


@panic_test()
def test_coredump_int_wdt_flash_bin_crc(env, _extra_data):
    test.int_wdt_inner(env, 'coredump_flash_bin_crc')


@panic_test()
def test_gdbstub_int_wdt(env, _extra_data):
    test.int_wdt_inner(env, 'gdbstub')


# test_int_wdt_cache_disabled

@panic_test()
def test_panic_int_wdt_cache_disabled(env, _extra_data):
    test.int_wdt_cache_disabled_inner(env, 'panic')


@panic_test()
def test_coredump_int_wdt_cache_disabled_uart_elf_crc(env, _extra_data):
    test.int_wdt_cache_disabled_inner(env, 'coredump_uart_elf_crc')


@panic_test()
def test_coredump_int_wdt_cache_disabled_uart_bin_crc(env, _extra_data):
    test.int_wdt_cache_disabled_inner(env, 'coredump_uart_bin_crc')


@panic_test()
def test_coredump_int_wdt_cache_disabled_flash_elf_sha(env, _extra_data):
    test.int_wdt_cache_disabled_inner(env, 'coredump_flash_elf_sha')


@panic_test()
def test_coredump_int_wdt_cache_disabled_flash_bin_crc(env, _extra_data):
    test.int_wdt_cache_disabled_inner(env, 'coredump_flash_bin_crc')


@panic_test()
def test_gdbstub_int_wdt_cache_disabled(env, _extra_data):
    test.int_wdt_cache_disabled_inner(env, 'gdbstub')


# test_cache_error

@panic_test()
def test_panic_cache_error(env, _extra_data):
    test.cache_error_inner(env, 'panic')


@panic_test()
def test_coredump_cache_error_uart_elf_crc(env, _extra_data):
    test.cache_error_inner(env, 'coredump_uart_elf_crc')


@panic_test()
def test_coredump_cache_error_uart_bin_crc(env, _extra_data):
    test.cache_error_inner(env, 'coredump_uart_bin_crc')


@panic_test()
def test_coredump_cache_error_flash_elf_sha(env, _extra_data):
    test.cache_error_inner(env, 'coredump_flash_elf_sha')


@panic_test()
def test_coredump_cache_error_flash_bin_crc(env, _extra_data):
    test.cache_error_inner(env, 'coredump_flash_bin_crc')


@panic_test()
def test_gdbstub_cache_error(env, _extra_data):
    test.cache_error_inner(env, 'gdbstub')


# test_stack_overflow

@panic_test(target=['ESP32', 'ESP32S2'])
def test_panic_stack_overflow(env, _extra_data):
    test.stack_overflow_inner(env, 'panic')


@panic_test()
def test_coredump_stack_overflow_uart_elf_crc(env, _extra_data):
    test.stack_overflow_inner(env, 'coredump_uart_elf_crc')


@panic_test()
def test_coredump_stack_overflow_uart_bin_crc(env, _extra_data):
    test.stack_overflow_inner(env, 'coredump_uart_bin_crc')


@panic_test()
def test_coredump_stack_overflow_flash_elf_sha(env, _extra_data):
    test.stack_overflow_inner(env, 'coredump_flash_elf_sha')


@panic_test()
def test_coredump_stack_overflow_flash_bin_crc(env, _extra_data):
    test.stack_overflow_inner(env, 'coredump_flash_bin_crc')


@panic_test()
def test_gdbstub_stack_overflow(env, _extra_data):
    test.stack_overflow_inner(env, 'gdbstub')


# test_instr_fetch_prohibited

@panic_test(target=['ESP32', 'ESP32S2'])
def test_panic_instr_fetch_prohibited(env, _extra_data):
    test.instr_fetch_prohibited_inner(env, 'panic')


@panic_test()
def test_coredump_instr_fetch_prohibited_uart_elf_crc(env, _extra_data):
    test.instr_fetch_prohibited_inner(env, 'coredump_uart_elf_crc')


@panic_test()
def test_coredump_instr_fetch_prohibited_uart_bin_crc(env, _extra_data):
    test.instr_fetch_prohibited_inner(env, 'coredump_uart_bin_crc')


@panic_test()
def test_coredump_instr_fetch_prohibited_flash_elf_sha(env, _extra_data):
    test.instr_fetch_prohibited_inner(env, 'coredump_flash_elf_sha')


@panic_test()
def test_coredump_instr_fetch_prohibited_flash_bin_crc(env, _extra_data):
    test.instr_fetch_prohibited_inner(env, 'coredump_flash_bin_crc')


@panic_test()
def test_gdbstub_instr_fetch_prohibited(env, _extra_data):
    test.instr_fetch_prohibited_inner(env, 'gdbstub')


# test_illegal_instruction

@panic_test(target=['ESP32', 'ESP32S2'])
def test_panic_illegal_instruction(env, _extra_data):
    test.illegal_instruction_inner(env, 'panic')


@panic_test()
def test_coredump_illegal_instruction_uart_elf_crc(env, _extra_data):
    test.illegal_instruction_inner(env, 'coredump_uart_elf_crc')


@panic_test()
def test_coredump_illegal_instruction_uart_bin_crc(env, _extra_data):
    test.illegal_instruction_inner(env, 'coredump_uart_bin_crc')


@panic_test()
def test_coredump_illegal_instruction_flash_elf_sha(env, _extra_data):
    test.illegal_instruction_inner(env, 'coredump_flash_elf_sha')


@panic_test()
def test_coredump_illegal_instruction_flash_bin_crc(env, _extra_data):
    test.illegal_instruction_inner(env, 'coredump_flash_bin_crc')


@panic_test()
def test_gdbstub_illegal_instruction(env, _extra_data):
    test.illegal_instruction_inner(env, 'gdbstub')


# test_storeprohibited

@panic_test(target=['ESP32', 'ESP32S2'])
def test_panic_storeprohibited(env, _extra_data):
    test.storeprohibited_inner(env, 'panic')


@panic_test()
def test_coredump_storeprohibited_uart_elf_crc(env, _extra_data):
    test.storeprohibited_inner(env, 'coredump_uart_elf_crc')


@panic_test()
def test_coredump_storeprohibited_uart_bin_crc(env, _extra_data):
    test.storeprohibited_inner(env, 'coredump_uart_bin_crc')


@panic_test()
def test_coredump_storeprohibited_flash_elf_sha(env, _extra_data):
    test.storeprohibited_inner(env, 'coredump_flash_elf_sha')


@panic_test()
def test_coredump_storeprohibited_flash_bin_crc(env, _extra_data):
    test.storeprohibited_inner(env, 'coredump_flash_bin_crc')


@panic_test()
def test_gdbstub_storeprohibited(env, _extra_data):
    test.storeprohibited_inner(env, 'gdbstub')


# test_abort

@panic_test(target=['ESP32', 'ESP32S2'])
def test_panic_abort(env, _extra_data):
    test.abort_inner(env, 'panic')


@panic_test(target=['ESP32'])
def test_panic_abort_cache_disabled(env, _extra_data):
    test.abort_cached_disabled_inner(env, 'panic')


@panic_test()
def test_coredump_abort_uart_elf_crc(env, _extra_data):
    test.abort_inner(env, 'coredump_uart_elf_crc')


@panic_test()
def test_coredump_abort_uart_bin_crc(env, _extra_data):
    test.abort_inner(env, 'coredump_uart_bin_crc')


@panic_test()
def test_coredump_abort_flash_elf_sha(env, _extra_data):
    test.abort_inner(env, 'coredump_flash_elf_sha')


@panic_test()
def test_coredump_abort_flash_bin_crc(env, _extra_data):
    test.abort_inner(env, 'coredump_flash_bin_crc')


@panic_test()
def test_gdbstub_abort(env, _extra_data):
    test.abort_inner(env, 'gdbstub')


# test_assert

@panic_test(target=['ESP32', 'ESP32S2'])
def test_panic_assert(env, _extra_data):
    test.assert_inner(env, 'panic')


@panic_test(target=['ESP32'])
def test_panic_assert_cache_disabled(env, _extra_data):
    test.assert_cached_disabled_inner(env, 'panic')


# test_ub

@panic_test()
def test_panic_ub(env, _extra_data):
    test.ub_inner(env, 'panic')


@panic_test()
def test_coredump_ub_uart_elf_crc(env, _extra_data):
    test.ub_inner(env, 'coredump_uart_elf_crc')


@panic_test()
def test_coredump_ub_uart_bin_crc(env, _extra_data):
    test.ub_inner(env, 'coredump_uart_bin_crc')


@panic_test()
def test_coredump_ub_flash_elf_sha(env, _extra_data):
    test.ub_inner(env, 'coredump_flash_elf_sha')


@panic_test()
def test_coredump_ub_flash_bin_crc(env, _extra_data):
    test.ub_inner(env, 'coredump_flash_bin_crc')


@panic_test()
def test_gdbstub_ub(env, _extra_data):
    test.ub_inner(env, 'gdbstub')


if __name__ == '__main__':
    run_all(__file__, sys.argv[1:])
