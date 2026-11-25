# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import re

import pytest
from elftools.elf.elffile import ELFFile
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

UMODE_TARGETS = soc_filtered_targets('SOC_RISCV_SV32_SUPPORTED == 1 and SOC_INT_CLIC_SUPPORTED == 1')


@pytest.mark.generic
@pytest.mark.parametrize('config', ['default', 'release', 'unicore'], indirect=True)
@idf_parametrize('target', UMODE_TARGETS, indirect=['target'])
def test_esp_privilege(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='privilege')


@pytest.mark.generic
@pytest.mark.parametrize('config', ['psram'], indirect=True)
@idf_parametrize('target', ['esp32s31'], indirect=['target'])
def test_esp_privilege_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='privilege')


@pytest.mark.generic
@pytest.mark.parametrize('config', ['default', 'release', 'unicore'], indirect=True)
@idf_parametrize('target', UMODE_TARGETS, indirect=['target'])
@pytest.mark.parametrize(
    'case_name, reason',
    [
        ('M-mode illegal instruction still panics', 'Illegal instruction'),
        ('M-mode fault with nonzero scratch retains the machine stack', 'Illegal instruction'),
    ],
)
def test_esp_privilege_panic(dut: Dut, case_name: str, reason: str) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write(f'"{case_name}"')
    dut.expect(
        rf"Guru Meditation Error: Core\s+\d panic'ed \({reason}\)\. "
        r'Exception was unhandled\.'
    )
    dut.expect_exact('Rebooting...')


def symbol_at(elf_file: str, address: int) -> str:
    """Name of the function that holds address"""
    with open(elf_file, 'rb') as f:
        symtab = ELFFile(f).get_section_by_name('.symtab')
        for sym in symtab.iter_symbols():
            if sym['st_info']['type'] != 'STT_FUNC':
                continue
            if sym['st_value'] <= address < sym['st_value'] + sym['st_size']:
                return str(sym.name)
    return ''


def expect_user_frame(dut: Dut) -> None:
    """The register dump of the trap from U-mode, then the backtrace of the M-mode code that entered U-mode, which
    starts in the domain run"""
    dut.expect(r'Trap taken from U-mode\r?\nCore\s+\d register dump:')
    dut.expect_exact('Backtrace of the M-mode code that entered U-mode:')
    # The first pc of the backtrace with frame pointers, or the pc of the register dump that idf.py monitor decodes
    match = dut.expect(r'(?:Backtrace: ?|MEPC\s+: )(0x[0-9a-f]{8})')
    # The M-mode frame is not a trap from U-mode
    assert b'Trap taken from U-mode' not in match.group(0) and b'Trap taken from U-mode' not in dut.pexpect_proc.before
    pc = match.group(1)
    assert symbol_at(dut.app.elf_file, int(pc, 16)) == 'esp_privilege_umode_domain_run'


@pytest.mark.generic
@pytest.mark.parametrize('config', ['default', 'release', 'unicore', 'frame_pointer'], indirect=True)
@idf_parametrize('target', UMODE_TARGETS, indirect=['target'])
@pytest.mark.require_elf
def test_esp_privilege_task_wdt_umode(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"a task watchdog timeout in U-mode code prints the M-mode backtrace"')
    dut.expect_exact('Task watchdog got triggered.')
    expect_user_frame(dut)


@pytest.mark.generic
@pytest.mark.parametrize('config', ['default', 'release', 'frame_pointer'], indirect=True)
@idf_parametrize('target', UMODE_TARGETS, indirect=['target'])
@pytest.mark.require_elf
def test_esp_privilege_int_wdt_other_core_umode(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"an interrupt watchdog timeout prints the M-mode backtrace of the other core in U-mode code"')
    dut.expect(r"Guru Meditation Error: Core\s+\d panic'ed \(Interrupt wdt timeout on CPU\d\)")
    # The block of the core that timed out, then the block of the other core, which was running the U-mode code
    expect_user_frame(dut)
    dut.expect_exact('Rebooting...')


@pytest.mark.generic
@pytest.mark.parametrize('config', ['default', 'release', 'unicore', 'frame_pointer'], indirect=True)
@idf_parametrize('target', UMODE_TARGETS, indirect=['target'])
@pytest.mark.require_elf
def test_esp_privilege_backtrace_all_tasks(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"the backtrace of all tasks prints a task preempted in U-mode code as a trap from U-mode"')
    output = dut.expect(r'Backtrace of all tasks\r?\n([\s\S]*)Backtrace of all tasks done').group(1)
    # Only the task that runs the U-mode code is printed as a trap from U-mode
    assert output.count(b'Trap taken from U-mode') == 1, output
    user_frame = re.search(
        rb'umode_spin\r?\nTrap taken from U-mode\r?\n[\s\S]*?Backtrace of the M-mode code that entered U-mode:'
        rb'\r?\n[\s\S]*?(?:Backtrace: ?|MEPC\s+: )(0x[0-9a-f]{8})',
        output,
    )
    assert user_frame is not None, output
    assert symbol_at(dut.app.elf_file, int(user_frame.group(1), 16)) == 'esp_privilege_umode_domain_run'
    dut.expect_unity_test_output()


@pytest.mark.generic
@pytest.mark.parametrize('config', ['default', 'release', 'unicore', 'frame_pointer'], indirect=True)
@idf_parametrize('target', UMODE_TARGETS, indirect=['target'])
@pytest.mark.require_elf
def test_esp_privilege_backtrace_after_umode(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"the backtrace of a task that ran U-mode code is that of the caller"')
    output = dut.expect(r'Backtrace of the caller\r?\n([\s\S]*)Backtrace of the caller done').group(1)
    # The stale frame of the last interrupt in U-mode is not printed
    assert b'Trap taken from U-mode' not in output
    # The first pc of the backtrace with frame pointers, or the pc of the register dump that idf.py monitor decodes
    pc = re.search(rb'(?:Backtrace: ?|MEPC\s+: )(0x[0-9a-f]{8})', output)
    assert pc is not None
    assert symbol_at(dut.app.elf_file, int(pc.group(1), 16)) == 'esp_backtrace_print'
