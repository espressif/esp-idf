# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os.path
import time

import pexpect.fdpexpect
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.jtag
@pytest.mark.parametrize('config', ['app_trace_jtag'], indirect=True)
@pytest.mark.parametrize('embedded_services', ['esp,idf,jtag'], indirect=True)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_examples_sysview_tracing_heap_log(idf_path: str, dut: IdfDut) -> None:
    # Construct trace log paths
    trace_log = [
        os.path.join(os.path.dirname(dut.gdb._logfile), 'heap_log0.svdat')  # pylint: disable=protected-access
    ]
    if dut.target in ['esp32', 'esp32s3', 'esp32p4']:
        trace_log.append(os.path.join(os.path.dirname(dut.gdb._logfile), 'heap_log1.svdat'))  # pylint: disable=protected-access

    # Set up GDB
    dut.gdb.write('set width unlimited')  # Don't split output lines for easy parsing
    dut.gdb.write('mon reset halt')
    dut.gdb.write('maintenance flush register-cache')

    # Start sysview tracing
    dut.gdb.write('tb heap_trace_start')
    dut.gdb.write('commands', non_blocking=True)
    trace_files = ' '.join([f'file://{log}' for log in trace_log])
    dut.gdb.write(f'mon esp sysview start {trace_files}', non_blocking=True)
    dut.gdb.write('c', non_blocking=True)
    dut.gdb.write('end')

    # Stop sysview tracing
    dut.gdb.write('tb heap_trace_stop')
    dut.gdb.write('commands', non_blocking=True)
    dut.gdb.write('mon esp sysview stop', non_blocking=True)
    dut.gdb.write('end')
    dut.gdb.write('c', non_blocking=True)

    # Wait for sysview files to be generated
    time.sleep(1)

    # Process sysview trace logs
    command = [os.path.join(idf_path, 'tools', 'esp_app_trace', 'sysviewtrace_proc.py'), '-p'] + trace_log
    with pexpect.spawn(' '.join(command)) as sysviewtrace:
        sysviewtrace.expect(r'Found \d+ leaked bytes in \d+ blocks.', timeout=120)

    # Validate GDB logs
    with open(dut.gdb._logfile, encoding='utf-8') as fr:  # pylint: disable=protected-access
        gdb_pexpect_proc = pexpect.fdpexpect.fdspawn(fr.fileno())
        gdb_pexpect_proc.expect_exact(
            'Thread 2 "main" hit Temporary breakpoint 1, heap_trace_start (mode_param', timeout=10
        )  # should be (mode_param=HEAP_TRACE_ALL) # TODO GCC-329
        gdb_pexpect_proc.expect_exact('Thread 2 "main" hit Temporary breakpoint 2, heap_trace_stop ()', timeout=10)
