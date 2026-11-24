# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import re

import pytest
from elftools.elf.elffile import ELFFile
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'target',
    ['esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32p4', 'esp32s31', 'esp32h4', 'esp32h21'],
    indirect=['target'],
)
def test_eh_frame_wdt(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.confirm_write('"Test task wdt can print backtrace with eh-frame"', expect_str='Running')

    # Expect a backtrace which is at least 3 PC-SP pairs deep
    dut.expect(r'Backtrace: (0x[a-fA-F0-9]+:0x[a-fA-F0-9]+\s*){3,}')


@pytest.mark.generic
@idf_parametrize(
    'target',
    ['esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32p4', 'esp32s31', 'esp32h4', 'esp32h21'],
    indirect=['target'],
)
def test_eh_frame_panic(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.confirm_write('"Test panic can print backtrace with eh-frame"', expect_str='Running')

    # Expect a backtrace which is at least 3 PC-SP pairs deep
    dut.expect(r'Backtrace: (0x[a-fA-F0-9]+:0x[a-fA-F0-9]+\s*){3,}')


def symbol_at(elf_file: str, address: int) -> str:
    """Name of the function that holds address"""
    with open(elf_file, 'rb') as f:
        symtab = ELFFile(f).get_section_by_name('.symtab')
        assert symtab is not None, 'The test requires an unstripped ELF'
        for sym in symtab.iter_symbols():
            if sym['st_info']['type'] != 'STT_FUNC':
                continue
            if sym['st_value'] <= address < sym['st_value'] + sym['st_size']:
                return str(sym.name)
    return ''


def expect_backtrace_from_caller(dut: Dut) -> None:
    """The backtrace printed between the markers starts in esp_backtrace_print(), then in its caller, and goes on
    past the test function"""
    output = dut.expect(r'Backtrace of the caller:\r?\n([\s\S]*?)Backtrace of the caller done').group(1)
    backtrace = re.search(rb'Backtrace:([^\r\n]*)', output)
    assert backtrace is not None, f'No backtrace in {output!r}'
    pcs = [int(pc, 16) for pc in re.findall(rb'0x([0-9a-f]{8}):0x[0-9a-f]{8}', backtrace.group(1))]
    assert len(pcs) >= 3, f'Backtrace too short: {backtrace.group(0)!r}'
    functions = [symbol_at(dut.app.elf_file, pc) for pc in pcs[:2]]
    assert functions == ['esp_backtrace_print', 'backtrace_print_caller'], f'Backtrace starts in {functions}'


@pytest.mark.generic
@idf_parametrize(
    'target',
    ['esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32p4', 'esp32s31', 'esp32h4', 'esp32h21'],
    indirect=['target'],
)
@pytest.mark.require_elf
def test_eh_frame_backtrace_print_from_task(dut: Dut) -> None:
    # In task context, esp_backtrace_print() must unwind from its caller, not from the frame that the last
    # interrupt of the task saved
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"Test esp_backtrace_print() from a task prints the backtrace of its caller with eh-frame"')
    expect_backtrace_from_caller(dut)
    dut.expect_unity_test_output()
