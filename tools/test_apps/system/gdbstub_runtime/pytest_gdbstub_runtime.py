# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os
import os.path as path
import sys
from typing import Any
from typing import Dict

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
    dut.expect_exact('tested app is running.')
    dut.write(b'\x03')  # send Ctrl-C
    dut.start_gdb_for_gdbstub()


def run_and_break(dut: PanicTestDut, cmd: str) -> Dict[Any, Any]:
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('running', 'result', responses) is not None
    if not dut.find_gdb_response('stopped', 'notify', responses):  # have not stopped on breakpoint yet
        responses = dut.gdbmi.get_gdb_response(timeout_sec=3)
    assert dut.find_gdb_response('stopped', 'notify', responses) is not None
    payload = dut.find_gdb_response('stopped', 'notify', responses)['payload']
    assert isinstance(payload, dict)
    return payload


@pytest.mark.generic
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_hwloop_jump(dut: PanicTestDut) -> None:
    start_gdb(dut)

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


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_gdbstub_runtime(dut: PanicTestDut) -> None:
    start_gdb(dut)

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
    cmd = '-gdb-set do_panic=1'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('done', 'result', responses) is not None

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
