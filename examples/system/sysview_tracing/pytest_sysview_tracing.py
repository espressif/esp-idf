# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os.path
import re
import time
import typing

import pexpect
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

if typing.TYPE_CHECKING:
    from conftest import OpenOCD


def _test_examples_sysview_tracing(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    # Construct trace log paths
    trace_log = [
        os.path.join(dut.logdir, 'sys_log0.svdat')  # pylint: disable=protected-access
    ]
    if not dut.app.sdkconfig.get('ESP_SYSTEM_SINGLE_CORE_MODE') or dut.target == 'esp32s3':
        trace_log.append(os.path.join(dut.logdir, 'sys_log1.svdat'))  # pylint: disable=protected-access
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

    def dut_expect_task_event() -> None:
        dut.expect(re.compile(rb'example: Task\[0x[0-9A-Fa-f]+\]: received event \d+'), timeout=30)

    dut.expect_exact('example: Ready for OpenOCD connection', timeout=5)
    with openocd_dut.run() as openocd, open(gdb_logfile, 'w') as gdb_log, pexpect.spawn(
        f'idf.py -B {dut.app.binary_path} gdb --batch -x {gdbinit}',
        timeout=60,
        logfile=gdb_log,
        encoding='utf-8',
        codec_errors='ignore',
    ) as p:
        p.expect_exact('hit Breakpoint 1, app_main ()')
        dut.expect('example: Created task')  # dut has been restarted by gdb since the last dut.expect()
        dut_expect_task_event()

        # Do a sleep while sysview samples are captured.
        time.sleep(3)
        openocd.write('esp sysview stop')


@pytest.mark.jtag
@idf_parametrize('target', ['esp32', 'esp32c2', 'esp32s2'], indirect=['target'])
def test_examples_sysview_tracing(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    _test_examples_sysview_tracing(openocd_dut, dut)


@pytest.mark.usb_serial_jtag
@idf_parametrize(
    'target', ['esp32s3', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32p4'], indirect=['target']
)
def test_examples_sysview_tracing_usj(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    _test_examples_sysview_tracing(openocd_dut, dut)
