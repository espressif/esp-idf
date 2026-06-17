# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import re
import sys
from pathlib import Path

import pytest
from pytest_embedded_idf.utils import idf_parametrize

PANIC_BASE_APP = Path(__file__).resolve().parent.parent / 'panic_base'
sys.path.insert(0, str(PANIC_BASE_APP))

import pytest_panic as panic_tests  # noqa: E402
from pytest_panic import PANIC_ABORT_PREFIX  # noqa: E402
from pytest_panic import common_test  # noqa: E402
from pytest_panic import expect_coredump_flash_write_logs  # noqa: E402
from pytest_panic import expect_coredump_uart_write_logs  # noqa: E402
from pytest_panic import get_default_backtrace  # noqa: E402
from test_panic_util import PanicTestDut  # noqa: E402

TARGETS_ALL = panic_tests.TARGETS_ALL
TARGETS_DUAL_CORE = panic_tests.TARGETS_DUAL_CORE
TARGETS_RISCV = panic_tests.TARGETS_RISCV
TARGETS_RISCV_DUAL_CORE = panic_tests.TARGETS_RISCV_DUAL_CORE
COREDUMP_APP = str(Path(__file__).resolve().parent)

COREDUMP_UNSUPPORTED_TARGETS = {'esp32h4'}
COREDUMP_TARGETS_ALL = [target for target in TARGETS_ALL if target not in COREDUMP_UNSUPPORTED_TARGETS]
COREDUMP_TARGETS_DUAL_CORE = [target for target in TARGETS_DUAL_CORE if target not in COREDUMP_UNSUPPORTED_TARGETS]
COREDUMP_TARGETS_RISCV = [target for target in TARGETS_RISCV if target not in COREDUMP_UNSUPPORTED_TARGETS]
COREDUMP_TARGETS_RISCV_DUAL_CORE = [
    target for target in TARGETS_RISCV_DUAL_CORE if target not in COREDUMP_UNSUPPORTED_TARGETS
]

CONFIGS = panic_tests.configs_for_app(
    COREDUMP_APP,
    [
        'coredump_flash_default',
        'coredump_flash_soft_sha',
        'coredump_uart_default',
        'coredump_flash_custom_stack',
    ],
)

CONFIGS_DUAL_CORE = panic_tests.configs_for_app(
    COREDUMP_APP,
    [
        'coredump_flash_default',
        'coredump_uart_default',
    ],
)

CONFIGS_HW_STACK_GUARD = panic_tests.configs_for_app(COREDUMP_APP, ['coredump_uart_default'])
CONFIGS_HW_STACK_GUARD_DUAL_CORE = panic_tests.configs_for_app(COREDUMP_APP, ['coredump_uart_default'])
CONFIG_CAPTURE_DRAM = panic_tests.configs_for_app(
    COREDUMP_APP, ['coredump_flash_capture_dram', 'coredump_uart_capture_dram']
)
CONFIG_COREDUMP_SUMMARY = panic_tests.configs_for_app(COREDUMP_APP, ['coredump_flash_default'])
CONFIG_COREDUMP_SUMMARY_FLASH_ENCRYPTED = panic_tests.configs_for_app(
    COREDUMP_APP, ['coredump_flash_encrypted', 'coredump_flash_encrypted_coredump_plain']
)
CONFIG_GDBSTUB_COREDUMP = panic_tests.configs_for_app(COREDUMP_APP, ['gdbstub_coredump'])
CONFIG_TCB_CORRUPTED = panic_tests.configs_for_app(COREDUMP_APP, ['coredump_flash_default'])


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIGS, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_ALL, indirect=['target'])
def test_task_wdt_cpu0(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    panic_tests.test_task_wdt_cpu0(dut, config, test_func_name)


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIGS_DUAL_CORE, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_DUAL_CORE, indirect=['target'])
def test_task_wdt_cpu1(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    panic_tests.test_task_wdt_cpu1(dut, config, test_func_name)


@pytest.mark.parametrize(
    'app_path, config, target',
    [
        pytest.param(COREDUMP_APP, 'coredump_flash_extram_stack_heap_esp32', 'esp32', marks=(pytest.mark.psram,)),
        pytest.param(COREDUMP_APP, 'coredump_flash_extram_stack_heap_esp32s2', 'esp32s2', marks=(pytest.mark.generic,)),
        pytest.param(
            COREDUMP_APP, 'coredump_flash_extram_stack_heap_esp32s3', 'esp32s3', marks=(pytest.mark.quad_psram,)
        ),
        pytest.param(COREDUMP_APP, 'coredump_flash_extram_stack_bss_esp32', 'esp32', marks=(pytest.mark.psram,)),
        pytest.param(COREDUMP_APP, 'coredump_flash_extram_stack_bss_esp32s2', 'esp32s2', marks=(pytest.mark.generic,)),
        pytest.param(
            COREDUMP_APP, 'coredump_flash_extram_stack_bss_esp32s3', 'esp32s3', marks=(pytest.mark.quad_psram,)
        ),
    ],
    indirect=True,
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


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIGS, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_ALL, indirect=['target'])
def test_int_wdt(dut: PanicTestDut, target: str, config: str, test_func_name: str) -> None:
    panic_tests.test_int_wdt(dut, target, config, test_func_name)


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIGS, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_ALL, indirect=['target'])
def test_int_wdt_cache_disabled(dut: PanicTestDut, target: str, config: str, test_func_name: str) -> None:
    panic_tests.test_int_wdt_cache_disabled(dut, target, config, test_func_name)


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIGS, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_ALL, indirect=['target'])
def test_cache_error(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    panic_tests.test_cache_error(dut, config, test_func_name)


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIGS, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_ALL, indirect=['target'])
def test_stack_overflow(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    panic_tests.test_stack_overflow(dut, config, test_func_name)


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIGS, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_ALL, indirect=['target'])
def test_instr_fetch_prohibited(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    panic_tests.test_instr_fetch_prohibited(dut, config, test_func_name)


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIGS, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_ALL, indirect=['target'])
def test_illegal_instruction(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    panic_tests.test_illegal_instruction(dut, config, test_func_name)


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIGS, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_ALL, indirect=['target'])
def test_storeprohibited(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    panic_tests.test_storeprohibited(dut, config, test_func_name)


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIGS, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_ALL, indirect=['target'])
def test_loadprohibited(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    panic_tests.test_loadprohibited(dut, config, test_func_name)


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIGS, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_ALL, indirect=['target'])
def test_abort(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    panic_tests.test_abort(dut, config, test_func_name)


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIGS, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_ALL, indirect=['target'])
def test_abort_cache_disabled(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    panic_tests.test_abort_cache_disabled(dut, config, test_func_name)


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIGS, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_ALL, indirect=['target'])
def test_assert(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    panic_tests.test_assert(dut, config, test_func_name)


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIGS, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_ALL, indirect=['target'])
def test_assert_cache_disabled(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    panic_tests.test_assert_cache_disabled(dut, config, test_func_name)


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIG_GDBSTUB_COREDUMP, indirect=True)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_gdbstub_coredump(dut: PanicTestDut) -> None:
    test_func_name = 'test_storeprohibited'
    dut.run_test_func(test_func_name)
    common_test(dut, 'gdbstub_coredump', get_default_backtrace(test_func_name))


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIGS_HW_STACK_GUARD, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_RISCV, indirect=['target'])
def test_hw_stack_guard_cpu0(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    panic_tests.test_hw_stack_guard_cpu0(dut, config, test_func_name)


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIGS_HW_STACK_GUARD_DUAL_CORE, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_RISCV_DUAL_CORE, indirect=['target'])
def test_hw_stack_guard_cpu1(dut: PanicTestDut, config: str, test_func_name: str) -> None:
    panic_tests.test_hw_stack_guard_cpu1(dut, config, test_func_name)


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIG_CAPTURE_DRAM, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_ALL, indirect=['target'])
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
@pytest.mark.parametrize('app_path, config', CONFIG_COREDUMP_SUMMARY, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_ALL, indirect=['target'])
def test_coredump_summary(dut: PanicTestDut) -> None:
    _test_coredump_summary(dut, False, False)


@pytest.mark.flash_encryption
@pytest.mark.parametrize('app_path, config', CONFIG_COREDUMP_SUMMARY_FLASH_ENCRYPTED, indirect=True)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_coredump_summary_flash_encrypted(dut: PanicTestDut, config: str) -> None:
    _test_coredump_summary(dut, True, config == 'coredump_flash_encrypted')


@pytest.mark.generic
@pytest.mark.parametrize('app_path, config', CONFIG_TCB_CORRUPTED, indirect=True)
@idf_parametrize('target', COREDUMP_TARGETS_ALL, indirect=['target'])
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
