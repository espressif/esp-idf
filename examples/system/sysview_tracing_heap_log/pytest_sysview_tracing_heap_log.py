# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os.path
import typing

import pexpect.fdpexpect
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

if typing.TYPE_CHECKING:
    from conftest import OpenOCD


def _test_examples_sysview_tracing_heap_log(openocd_dut: 'OpenOCD', idf_path: str, dut: IdfDut) -> None:
    # Construct trace log paths
    trace_log = [
        os.path.join(dut.logdir, 'heap_log0.svdat')  # pylint: disable=protected-access
    ]
    if not dut.app.sdkconfig.get('ESP_SYSTEM_SINGLE_CORE_MODE') or dut.target == 'esp32s3':
        trace_log.append(os.path.join(dut.logdir, 'heap_log1.svdat'))  # pylint: disable=protected-access
    trace_files = ' '.join([f'file://{log}' for log in trace_log])

    # Prepare gdbinit file
    gdb_logfile = os.path.join(dut.logdir, 'gdb.txt')
    gdbinit_orig = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'gdbinit')
    gdbinit = os.path.join(dut.logdir, 'gdbinit')
    with open(gdbinit_orig, 'r') as f_r, open(gdbinit, 'w') as f_w:
        for line in f_r:
            if line.startswith('mon esp sysview start'):
                f_w.write(f'mon esp sysview start {trace_files}\n')
            else:
                f_w.write(line)

    dut.expect_exact('example: Ready for OpenOCD connection', timeout=5)
    with openocd_dut.run() as oocd:
        if dut.target == 'esp32p4':
            oocd.write('esp appimage_offset 0x20000')
        with open(gdb_logfile, 'w') as gdb_log, pexpect.spawn(
            f'idf.py -B {dut.app.binary_path} gdb --batch -x {gdbinit}',
            timeout=60,
            logfile=gdb_log,
            encoding='utf-8',
            codec_errors='ignore',
        ) as p:
            # Wait for sysview files to be generated
            p.expect_exact('Tracing is STOPPED')

    # Process sysview trace logs
    command = [os.path.join(idf_path, 'tools', 'esp_app_trace', 'sysviewtrace_proc.py'), '-p'] + trace_log
    with pexpect.spawn(' '.join(command)) as sysviewtrace:
        sysviewtrace.expect(r'Found \d+ leaked bytes in \d+ blocks.', timeout=120)

    # Validate GDB logs
    with open(gdb_logfile, encoding='utf-8') as fr:  # pylint: disable=protected-access
        gdb_pexpect_proc = pexpect.fdpexpect.fdspawn(fr.fileno())
        gdb_pexpect_proc.expect_exact(
            'Thread 2 "main" hit Temporary breakpoint 1, heap_trace_start (mode_param', timeout=10
        )  # should be (mode_param=HEAP_TRACE_ALL) # TODO GCC-329
        gdb_pexpect_proc.expect_exact('Thread 2 "main" hit Temporary breakpoint 2, heap_trace_stop ()', timeout=10)


@pytest.mark.parametrize('config', ['app_trace_jtag'], indirect=True)
@pytest.mark.jtag
@idf_parametrize('target', ['esp32', 'esp32c2', 'esp32s2'], indirect=['target'])
def test_examples_sysview_tracing_heap_log(openocd_dut: 'OpenOCD', idf_path: str, dut: IdfDut) -> None:
    _test_examples_sysview_tracing_heap_log(openocd_dut, idf_path, dut)


@pytest.mark.parametrize('config', ['app_trace_jtag'], indirect=True)
@pytest.mark.usb_serial_jtag
@idf_parametrize(
    'target', ['esp32s3', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32p4'], indirect=['target']
)
def test_examples_sysview_tracing_heap_log_usj(openocd_dut: 'OpenOCD', idf_path: str, dut: IdfDut) -> None:
    _test_examples_sysview_tracing_heap_log(openocd_dut, idf_path, dut)
