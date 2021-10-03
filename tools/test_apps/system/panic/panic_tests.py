#!/usr/bin/env python
import re
from pprint import pformat

from test_panic_util.test_panic_util import get_dut


def get_default_backtrace(test_name):
    return [
        test_name,
        'app_main',
        'main_task',
        'vPortTaskWrapper'
    ]


def test_common(dut, test_name, expected_backtrace=None):
    if expected_backtrace is None:
        expected_backtrace = get_default_backtrace(dut.test_name)

    if 'gdbstub' in test_name:
        dut.expect('Entering gdb stub now.')
        dut.start_gdb()
        frames = dut.gdb_backtrace()
        if not dut.match_backtrace(frames, expected_backtrace):
            raise AssertionError('Unexpected backtrace in test {}:\n{}'.format(test_name, pformat(frames)))
        return

    if 'uart' in test_name:
        dut.expect(dut.COREDUMP_UART_END)

    dut.expect('Rebooting...')

    if 'uart' in test_name:
        dut.process_coredump_uart()
        # TODO: check backtrace
    elif 'flash' in test_name:
        dut.process_coredump_flash()
        # TODO: check backtrace
    elif 'panic' in test_name:
        # TODO: check backtrace
        pass


def task_wdt_inner(env, test_name):
    with get_dut(env, test_name, 'test_task_wdt', qemu_wdt_enable=True) as dut:
        dut.expect('Task watchdog got triggered. The following tasks did not reset the watchdog in time:')
        dut.expect('CPU 0: main')
        dut.expect(re.compile(r'abort\(\) was called at PC [0-9xa-f]+ on core 0'))
        dut.expect_none('register dump:')
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation')
        if ('gdbstub' in test_name):
            test_common(dut, test_name, expected_backtrace=[
                # Backtrace interrupted when abort is called, IDF-842
                'panic_abort', 'esp_system_abort'
            ])
        else:
            test_common(dut, test_name)


def int_wdt_inner(env, test_name):
    with get_dut(env, test_name, 'test_int_wdt', qemu_wdt_enable=True) as dut:
        dut.expect_gme('Interrupt wdt timeout on CPU0')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_none('Guru Meditation')
        dut.expect_reg_dump(1)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation')
        test_common(dut, test_name)


def int_wdt_cache_disabled_inner(env, test_name):
    with get_dut(env, test_name, 'test_int_wdt_cache_disabled', qemu_wdt_enable=True) as dut:
        dut.expect_gme('Interrupt wdt timeout on CPU0')
        dut.expect_reg_dump(0)
        dut.expect('Backtrace:')
        dut.expect_none('Guru Meditation')
        dut.expect_reg_dump(1)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation')
        test_common(dut, test_name)


def cache_error_inner(env, test_name):
    with get_dut(env, test_name, 'test_cache_error') as dut:
        dut.expect_gme('Cache disabled but cached memory region accessed')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation')
        test_common(dut, test_name,
                    expected_backtrace=['die'] + get_default_backtrace(dut.test_name))


def abort_inner(env, test_name):
    with get_dut(env, test_name, 'test_abort') as dut:
        dut.expect(re.compile(r'abort\(\) was called at PC [0-9xa-f]+ on core 0'))
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation', 'Re-entered core dump')
        if ('gdbstub' in test_name):
            test_common(dut, test_name, expected_backtrace=[
                # Backtrace interrupted when abort is called, IDF-842
                'panic_abort', 'esp_system_abort'
            ])
        else:
            test_common(dut, test_name)


def abort_cached_disabled_inner(env, test_name):
    with get_dut(env, test_name, 'test_abort_cache_disabled') as dut:
        dut.expect(re.compile(r'abort\(\) was called at PC [0-9xa-f]+ on core 0'))
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation', 'Re-entered core dump')
        test_common(dut, test_name)


def assert_inner(env, test_name):
    with get_dut(env, test_name, 'test_assert') as dut:
        dut.expect(re.compile(r'(assert failed:[\s\w\(\)]*?\s[\.\w\/]*\.(?:c|cpp|h|hpp):\d*.*)'))
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation', 'Re-entered core dump')
        test_common(dut, test_name)


def assert_cached_disabled_inner(env, test_name):
    with get_dut(env, test_name, 'test_assert_cache_disabled') as dut:
        dut.expect(re.compile(r'(assert failed: [0-9xa-fA-F]+.*)'))
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation', 'Re-entered core dump')
        test_common(dut, test_name)


def storeprohibited_inner(env, test_name):
    with get_dut(env, test_name, 'test_storeprohibited') as dut:
        dut.expect_gme('StoreProhibited')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation')
        test_common(dut, test_name)


def stack_overflow_inner(env, test_name):
    with get_dut(env, test_name, 'test_stack_overflow') as dut:
        dut.expect_gme('Unhandled debug exception')
        dut.expect('Stack canary watchpoint triggered (main)')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation')
        test_common(dut, test_name)


def illegal_instruction_inner(env, test_name):
    with get_dut(env, test_name, 'test_illegal_instruction') as dut:
        dut.expect_gme('IllegalInstruction')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation')
        test_common(dut, test_name)


def instr_fetch_prohibited_inner(env, test_name):
    with get_dut(env, test_name, 'test_instr_fetch_prohibited') as dut:
        dut.expect_gme('InstrFetchProhibited')
        dut.expect_reg_dump(0)
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation')
        test_common(dut, test_name,
                    expected_backtrace=['_init'] + get_default_backtrace(dut.test_name))


def ub_inner(env, test_name):
    with get_dut(env, test_name, 'test_ub') as dut:
        dut.expect(re.compile(r'Undefined behavior of type out_of_bounds'))
        dut.expect_backtrace()
        dut.expect_elf_sha256()
        dut.expect_none('Guru Meditation', 'Re-entered core dump')
        if ('gdbstub' in test_name):
            test_common(dut, test_name, expected_backtrace=[
                # Backtrace interrupted when abort is called, IDF-842
                'panic_abort', 'esp_system_abort'
            ])
        else:
            test_common(dut, test_name)
