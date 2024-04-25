# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os.path
import time

import pexpect.fdpexpect
import pytest
from pytest_embedded_idf import IdfDut


@pytest.mark.esp32
@pytest.mark.jtag
@pytest.mark.parametrize('config', ['app_trace_jtag'], indirect=True)
@pytest.mark.parametrize('embedded_services', ['esp,idf,jtag'], indirect=True)
def test_examples_sysview_tracing_heap_log(idf_path: str, dut: IdfDut) -> None:
    trace_log = os.path.join(os.path.dirname(dut.gdb._logfile), 'heap_log.svdat')  # pylint: disable=protected-access

    dut.gdb.write('set width unlimited')  # Don't split output lines for easy parsing
    dut.gdb.write('mon reset halt')
    dut.gdb.write('maintenance flush register-cache')

    dut.gdb.write('tb heap_trace_start')
    dut.gdb.write('commands', non_blocking=True)
    dut.gdb.write(f'mon esp sysview start file://{trace_log}', non_blocking=True)
    dut.gdb.write('c', non_blocking=True)
    dut.gdb.write('end')

    dut.gdb.write('tb heap_trace_stop')
    dut.gdb.write('commands', non_blocking=True)
    dut.gdb.write('mon esp sysview stop', non_blocking=True)
    dut.gdb.write('end')

    dut.gdb.write('c')
    dut.expect('esp_apptrace: Initialized TRAX on CPU0')

    time.sleep(1)  # make sure that the sysview file has been generated
    with pexpect.spawn(' '.join([os.path.join(idf_path, 'tools', 'esp_app_trace', 'sysviewtrace_proc.py'),
                                 '-p',
                                 '-b', dut.app.elf_file,
                                 trace_log])) as sysviewtrace:
        sysviewtrace.expect(r'Found \d+ leaked bytes in \d+ blocks.', timeout=120)

    with open(dut.gdb._logfile) as fr:  # pylint: disable=protected-access
        gdb_pexpect_proc = pexpect.fdpexpect.fdspawn(fr.fileno())
        gdb_pexpect_proc.expect_exact(
            'Thread 2 "main" hit Temporary breakpoint 1, heap_trace_start (mode_param', timeout=10)  # should be (mode_param=HEAP_TRACE_ALL) # TODO GCC-329
        gdb_pexpect_proc.expect_exact('Thread 2 "main" hit Temporary breakpoint 2, heap_trace_stop ()', timeout=10)
