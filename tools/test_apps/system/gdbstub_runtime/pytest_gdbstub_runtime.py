# SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os
import os.path as path
import sys
from collections.abc import Callable
from typing import Any

import pytest
from pytest_embedded_idf.utils import idf_parametrize

sys.path.append(path.expandvars(path.join('$IDF_PATH', 'tools', 'test_apps', 'system', 'panic')))
from test_panic_util import PanicTestDut  # noqa: E402


def get_line_number(lookup: str, offset: int = 0) -> int:
    src_file = path.join(path.dirname(path.abspath(__file__)), 'main', 'test_app_main.c')
    with open(src_file) as f:
        for num, line in enumerate(f, 1):
            if lookup in line:
                return num + offset
    return -1


def start_gdb(dut: PanicTestDut) -> None:
    dut.expect_exact('waiting start_testing variable to be changed.')
    dut.write(b'\x03')  # send Ctrl-C
    dut.start_gdb_for_gdbstub()


def run_and_break(dut: PanicTestDut, cmd: str) -> dict[Any, Any]:
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('running', 'result', responses) is not None
    if not dut.find_gdb_response('stopped', 'notify', responses):  # have not stopped on breakpoint yet
        responses = dut.gdbmi.get_gdb_response(timeout_sec=3)
    assert dut.find_gdb_response('stopped', 'notify', responses) is not None
    payload = dut.find_gdb_response('stopped', 'notify', responses)['payload']
    assert isinstance(payload, dict)
    return payload


def dut_set_variable(dut: PanicTestDut, var_name: str, value: int) -> None:
    cmd = f'-gdb-set {var_name}={value}'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('done', 'result', responses) is not None


def dut_enable_test(dut: PanicTestDut, testcase: str | None = None) -> None:
    dut_set_variable(dut, 'start_testing', 1)

    # enable specific testcase (otherwise default testcase)
    if testcase:
        dut_set_variable(dut, f'do_test_{testcase}', 1)


def dut_get_threads(dut: PanicTestDut) -> Any:
    cmd = '-thread-info'
    responses = dut.gdb_write(cmd)
    if not responses[0]['message']:
        responses = dut.gdb_write(cmd)
    assert responses is not None
    return responses[0]['payload']['threads']


@pytest.mark.generic
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_hwloop_jump(dut: PanicTestDut) -> None:
    start_gdb(dut)

    # enable coprocessors registers testing
    dut_enable_test(dut, 'xesppie_loops')

    cmd = '-break-insert --source xesppie_loops.S --function test_loop_start'
    response = dut.find_gdb_response('done', 'result', dut.gdb_write(cmd))
    assert response is not None

    # go to the beginning of the loop
    cmd = '-exec-continue'
    payload = run_and_break(dut, cmd)
    assert payload['reason'] == 'breakpoint-hit'
    assert payload['bkptno'] == '1'
    assert payload['frame']['func'] == 'test_xesppie_loops'
    assert payload['stopped-threads'] == 'all'

    cmd = '-break-delete 1'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('done', 'result', responses) is not None

    # go through the loop
    loop_count = 3
    while loop_count:
        inst_count = 2
        while inst_count:
            cmd = '-exec-step'
            payload = run_and_break(dut, cmd)
            assert payload['reason'] == 'end-stepping-range'
            assert payload['frame']['func'] == 'test_xesppie_loops'
            assert payload['stopped-threads'] == 'all'
            inst_count -= 1
        cmd = '-data-list-register-values d 11'
        responses = dut.gdb_write(cmd)
        response = dut.find_gdb_response('done', 'result', responses)
        assert response is not None
        payload = response['payload']
        assert payload['register-values'][0]['number'] == '11'
        assert payload['register-values'][0]['value'] == f'{loop_count}'
        loop_count -= 1

    # go through the func prologue
    remaining_instructions = 3
    while remaining_instructions:
        cmd = '-exec-step'
        payload = run_and_break(dut, cmd)
        assert payload['reason'] == 'end-stepping-range'
        assert payload['frame']['func'] == 'test_xesppie_loops'
        assert payload['stopped-threads'] == 'all'
        remaining_instructions -= 1

    # Now we stepping back to app_main
    cmd = '-exec-step'
    payload = run_and_break(dut, cmd)
    assert payload['reason'] == 'end-stepping-range'
    assert payload['frame']['func'] == 'app_main'
    assert payload['stopped-threads'] == 'all'


def check_registers_numbers(dut: PanicTestDut) -> None:
    cmd = '-data-list-register-values d'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('done', 'result', responses) is not None

    registers = responses[0]['payload']['register-values']
    assert len(registers) == 83  # 80 registers supported + xesppie misc + pseudo frm, fflags

    r_id = 0
    for r in registers:
        assert int(r['number']) == r_id

        if r_id == 4211:  # check if value of q0 register is uint128
            assert 'uint128' in r['value']

        if r_id == 64:
            r_id = 68  # fcsr
        elif r_id == 68:
            r_id = 4211  # q0
        else:
            r_id += 1


def set_riscv_float_registers(dut: PanicTestDut, t_id: int, addition: int) -> None:
    cmd = f'-thread-select {t_id}'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('done', 'result', responses) is not None

    for i in range(32):
        cmd = f'-data-write-register-values d {33 + i} {i + addition}'
        responses = dut.gdb_write(cmd)
        assert dut.find_gdb_response('done', 'result', responses) is not None

    # Note that it's a gap between the last floating register number and fcsr register number.
    cmd = f'-data-write-register-values d 68 {32 + addition}'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('done', 'result', responses) is not None


def set_xtensa_float_registers(dut: PanicTestDut, t_id: int, addition: int) -> None:
    """Set Xtensa FPU registers via GDB.

    Xtensa FPU register numbers:
    - f0-f15: registers 87-102
    - fcr: register 103
    - fsr: register 104
    """
    cmd = f'-thread-select {t_id}'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('done', 'result', responses) is not None

    if dut.target == 'esp32':
        fpu_current_register = 87
    elif dut.target == 'esp32s3':
        fpu_current_register = 84
    else:
        raise ValueError(f'Unsupported target: {dut.target}')

    for i in range(18):  # 16 f* registers + fcr + fsr
        cmd = f'-data-write-register-values d {fpu_current_register} {i + addition}'
        responses = dut.gdb_write(cmd)
        assert dut.find_gdb_response('done', 'result', responses) is not None
        fpu_current_register += 1


def set_riscv_pie_registers(dut: PanicTestDut, t_id: int, addition: int) -> None:
    cmd = f'-thread-select {t_id}'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('done', 'result', responses) is not None

    def set_gdb_128_bit_register(reg: str, byte: int) -> None:
        val64 = f'0x{hex(byte)[2:] * 8}'
        value = f'{{{val64}, {val64}}}'
        cmd = f'-interpreter-exec console "set ${reg}.v2_int64={value}"'
        responses = dut.gdb_write(cmd)
        assert dut.find_gdb_response('done', 'result', responses) is not None

    for i in range(8):
        set_gdb_128_bit_register(f'q{i}', 0x10 + i + addition)

    set_gdb_128_bit_register('qacc_l_l', 0x18 + addition)
    set_gdb_128_bit_register('qacc_l_h', 0x19 + addition)
    set_gdb_128_bit_register('qacc_h_l', 0x1A + addition)
    set_gdb_128_bit_register('qacc_h_h', 0x1B + addition)
    set_gdb_128_bit_register('ua_state', 0x1C + addition)

    xacc_val = ','.join([hex(0x1D + addition)] * 5)
    cmd = f'-interpreter-exec console "set $xacc={{{xacc_val}}}"'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('done', 'result', responses) is not None


def coproc_registers_test(dut: PanicTestDut, regs_type: str, set_registers: Callable) -> None:
    # set start test breakpoint
    cmd = f'-break-insert --source coproc_regs.c --function test_{regs_type} --label {regs_type}_start'
    response = dut.find_gdb_response('done', 'result', dut.gdb_write(cmd))
    assert response is not None

    # stop when the second task is stopped
    for i in range(2):
        cmd = '-exec-continue'
        payload = run_and_break(dut, cmd)
        assert payload['reason'] == 'breakpoint-hit'
        assert payload['frame']['func'] == f'test_{regs_type}'
        assert payload['stopped-threads'] == 'all'

    threads = dut_get_threads(dut)

    """
    Set expected values to both testing tasks.
    This will test setting register for both:
      - Task coproc owner (direct registers write)
      - Other tasks (write registers to task's stack)
    """
    found_count = 0
    for t in threads:
        for task_num in [1, 2]:
            if f'test_{regs_type}_{task_num}' in t['details']:
                set_registers(dut, t['id'], task_num)
                found_count += 1

    assert found_count == 2, f'Expected 2 coproc tasks, found {found_count}'

    dut_set_variable(dut, f'test_{regs_type}_ready', 1)

    cmd = '-break-delete'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('done', 'result', responses) is not None

    cmd = f'-break-insert --source coproc_regs.c --function test_coproc_regs --label {regs_type}_succeed'
    response = dut.find_gdb_response('done', 'result', dut.gdb_write(cmd))
    assert response is not None

    cmd = '-exec-continue'
    payload = run_and_break(dut, cmd)
    assert payload['reason'] == 'breakpoint-hit'
    assert payload['frame']['func'] == 'test_coproc_regs'
    assert payload['stopped-threads'] == 'all'

    threads = dut_get_threads(dut)

    found_count = 0
    for t in threads:
        for task_num in [1, 2]:
            if f'test_{regs_type}_{task_num}' in t['details']:
                found_count += 1

    assert found_count == 0, f'Expected 0 coproc tasks, found {found_count}'


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32s3', 'esp32p4'], indirect=['target'])
def test_coproc_registers(dut: PanicTestDut) -> None:
    start_gdb(dut)

    # enable coprocessors registers testing
    dut_enable_test(dut, 'coproc_regs')

    if dut.is_xtensa:
        coproc_registers_test(dut, 'fpu', set_xtensa_float_registers)
    else:
        check_registers_numbers(dut)
        coproc_registers_test(dut, 'fpu', set_riscv_float_registers)
        if dut.target == 'esp32p4':
            coproc_registers_test(dut, 'pie', set_riscv_pie_registers)


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_gdbstub_runtime(dut: PanicTestDut) -> None:
    start_gdb(dut)

    dut_enable_test(dut)

    # Test breakpoint
    cmd = '-break-insert --source test_app_main.c --function app_main --label label_1'
    response = dut.find_gdb_response('done', 'result', dut.gdb_write(cmd))
    assert response is not None
    cmd = '-exec-continue'
    payload = run_and_break(dut, cmd)
    assert payload['reason'] == 'breakpoint-hit'
    assert payload['bkptno'] == '1'
    assert payload['frame']['func'] == 'app_main'
    assert payload['frame']['line'] == str(get_line_number('label_1:', 1))
    assert payload['stopped-threads'] == 'all'

    # Test step command
    cmd = '-exec-step'
    payload = run_and_break(dut, cmd)
    assert payload['reason'] == 'end-stepping-range'
    assert payload['frame']['func'] == 'foo'
    assert payload['frame']['line'] == str(get_line_number('var_2+=2;'))
    assert payload['stopped-threads'] == 'all'

    # Test finish command
    cmd = '-exec-finish'
    payload = run_and_break(dut, cmd)
    assert payload['reason'] == 'function-finished'
    # On riscv we may have situation when returned from a function but stay on exactly the same line
    #            foo();
    # 4200ae5c:	f99ff0ef          	jal	ra,4200adf4 <foo>
    # 4200ae60:	a011                	j	4200ae64 <app_main+0x4e>    <----------- here after return from foo()
    #        }
    assert payload['frame']['line'] == str(
        get_line_number('label_3:', 1) if dut.is_xtensa else get_line_number('foo();', 0)
    )
    assert payload['frame']['func'] == 'app_main'
    assert payload['stopped-threads'] == 'all'

    cmd = '-exec-continue'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('running', 'result', responses) is not None
    assert dut.find_gdb_response('running', 'notify', responses) is not None

    # Test next command
    cmd = '-exec-next'
    payload = run_and_break(dut, cmd)
    assert payload['reason'] == 'end-stepping-range'
    assert payload['frame']['line'] == str(get_line_number('label_3:', 1))
    assert payload['frame']['func'] == 'app_main'
    assert payload['stopped-threads'] == 'all'

    # test delete breakpoint
    cmd = '-break-delete 1'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('done', 'result', responses) is not None
    cmd = '-exec-continue'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('running', 'result', responses) is not None
    assert dut.find_gdb_response('running', 'notify', responses) is not None

    # test ctrl-c
    os.kill(dut.gdbmi.gdb_process.pid, 2)
    # responses = dut.gdbmi.send_signal_to_gdb(2)  # https://github.com/cs01/pygdbmi/issues/97
    # assert dut.find_gdb_response('stopped', 'notify', responses) is not None
    # ?? No response? check we stopped
    dut.gdb_backtrace()

    # test watchpoint
    cmd = '-break-watch var_2'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('done', 'result', responses) is not None
    cmd = '-exec-continue'
    payload = run_and_break(dut, cmd)
    assert payload['reason'] == 'signal-received'
    assert payload['frame']['func'] == 'foo'
    assert payload['stopped-threads'] == 'all'
    # Uncomment this when implement send reason to gdb: GCC-313
    #
    # assert payload['reason'] == 'watchpoint-trigger'
    # assert int(payload['value']['new']) == int(payload['value']['old']) + 1
    # assert payload['frame']['line'] == '14'

    cmd = '-break-delete 2'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('done', 'result', responses) is not None

    # test set variable
    dut_set_variable(dut, 'do_panic', 1)

    # test panic handling
    cmd = '-exec-continue'
    payload = run_and_break(dut, cmd)
    assert payload['reason'] == 'signal-received'
    assert payload['signal-name'] == 'SIGSEGV'
    assert payload['frame']['func'] == 'app_main'
    assert payload['frame']['line'] == str(get_line_number('label_5', 1))
    assert payload['stopped-threads'] == 'all'


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32', 'esp32s2', 'esp32s3'], reason='fix IDF-7927')
@idf_parametrize('target', ['esp32', 'esp32s2', 'esp32s3'], indirect=['target'])
def test_gdbstub_runtime_xtensa_stepping_bug(dut: PanicTestDut) -> None:
    start_gdb(dut)

    dut_enable_test(dut)

    # Test breakpoint
    cmd = '-break-insert --source test_app_main.c --function app_main --label label_1'
    response = dut.find_gdb_response('done', 'result', dut.gdb_write(cmd))
    assert response is not None
    cmd = '-exec-continue'
    payload = run_and_break(dut, cmd)
    assert payload['reason'] == 'breakpoint-hit'
    assert payload['bkptno'] == '1'
    assert payload['frame']['func'] == 'app_main'
    assert payload['frame']['line'] == str(get_line_number('label_1:', 1))
    assert payload['stopped-threads'] == 'all'

    # Test step command
    cmd = '-exec-step'
    payload = run_and_break(dut, cmd)
    assert payload['reason'] == 'end-stepping-range'
    assert payload['frame']['func'] == 'foo'
    assert payload['frame']['line'] == str(get_line_number('var_2+=2;'))
    assert payload['stopped-threads'] == 'all'

    # Test next command
    cmd = '-exec-next'
    payload = run_and_break(dut, cmd)
    assert payload['reason'] == 'end-stepping-range'
    assert payload['frame']['line'] == str(get_line_number('var_2--;', 0))
    assert payload['frame']['func'] == 'foo'
    assert payload['stopped-threads'] == 'all'
