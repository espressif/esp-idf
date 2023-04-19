# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import os
import sys

import pytest

sys.path.append(os.path.expandvars(os.path.join('$IDF_PATH', 'tools', 'test_apps', 'system', 'panic')))
from test_panic_util import PanicTestDut  # noqa: E402


@pytest.mark.supported_targets
@pytest.mark.temp_skip_ci(targets=['esp32c2', 'esp32h2'], reason='resolve IDF-7264')
@pytest.mark.generic
def test_gdbstub_runtime(dut: PanicTestDut) -> None:
    dut.expect_exact('tested app is runnig.')
    dut.write(b'\x03')  # send Ctrl-C
    dut.start_gdb()

    # Test breakpoint
    cmd = '-break-insert --source test_app_main.c --line 23'
    response = dut.find_gdb_response('done', 'result', dut.gdb_write(cmd))
    assert response is not None
    cmd = '-exec-continue'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('running', 'result', responses) is not None
    if not dut.find_gdb_response('stopped', 'notify', responses):
        # does not stoped on breakpoint yet
        responses = dut.gdbmi.get_gdb_response(timeout_sec=3)
    assert dut.find_gdb_response('stopped', 'notify', responses) is not None
    payload = dut.find_gdb_response('stopped', 'notify', responses)['payload']
    assert payload['reason'] == 'breakpoint-hit'
    assert payload['bkptno'] == '1'
    assert payload['frame']['func'] == 'app_main'
    assert payload['frame']['line'] == '23'
    assert payload['stopped-threads'] == 'all'

    # Test step command
    cmd = '-exec-step'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('running', 'result', responses) is not None
    if not dut.find_gdb_response('stopped', 'notify', responses):
        # does not stoped on breakpoint yet
        responses = dut.gdbmi.get_gdb_response(timeout_sec=3)
    assert dut.find_gdb_response('stopped', 'notify', responses) is not None
    payload = dut.find_gdb_response('stopped', 'notify', responses)['payload']
    assert payload['reason'] == 'end-stepping-range'
    assert payload['frame']['func'] == 'foo'
    assert payload['frame']['line'] == '14'
    assert payload['stopped-threads'] == 'all'

    # Test finish command
    cmd = '-exec-finish'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('running', 'result', responses) is not None
    if not dut.find_gdb_response('stopped', 'notify', responses):
        # does not stoped on breakpoint yet
        responses = dut.gdbmi.get_gdb_response(timeout_sec=3)
    assert dut.find_gdb_response('stopped', 'notify', responses) is not None
    payload = dut.find_gdb_response('stopped', 'notify', responses)['payload']
    assert payload['reason'] == 'function-finished'
    assert payload['frame']['line'] == '23'
    assert payload['frame']['func'] == 'app_main'
    assert payload['stopped-threads'] == 'all'

    # Test next command
    cmd = '-exec-next'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('running', 'result', responses) is not None
    if not dut.find_gdb_response('stopped', 'notify', responses):
        # does not stoped on breakpoint yet
        responses = dut.gdbmi.get_gdb_response(timeout_sec=3)
    assert dut.find_gdb_response('stopped', 'notify', responses) is not None
    payload = dut.find_gdb_response('stopped', 'notify', responses)['payload']
    assert payload['reason'] == 'end-stepping-range'
    assert payload['frame']['line'] == '21'
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
    responses = dut.gdbmi.send_signal_to_gdb(2)
    # assert dut.find_gdb_response('stopped', 'notify', responses) is not None
    # ?? No response? check we stopped
    dut.gdb_backtrace()

    # test watchpoint
    cmd = '-break-watch var_2'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('done', 'result', responses) is not None
    cmd = '-exec-continue'
    responses = dut.gdb_write(cmd)
    assert dut.find_gdb_response('running', 'result', responses) is not None
    if not dut.find_gdb_response('stopped', 'notify', responses):
        # does not stoped on breakpoint yet
        responses = dut.gdbmi.get_gdb_response(timeout_sec=3)
    payload = dut.find_gdb_response('stopped', 'notify', responses)['payload']
    assert payload['reason'] == 'signal-received'
    assert payload['frame']['func'] == 'foo'
    assert payload['stopped-threads'] == 'all'
    # Uncomment this when implement send reason to gdb: GCC-313
    #
    # assert payload['reason'] == 'watchpoint-trigger'
    # assert int(payload['value']['new']) == int(payload['value']['old']) + 1
    # assert payload['frame']['line'] == '14'
