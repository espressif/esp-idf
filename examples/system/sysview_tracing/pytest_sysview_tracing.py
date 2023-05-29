# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import re
import time

import pexpect.fdpexpect
import pytest
from pytest_embedded_idf import IdfDut


@pytest.mark.esp32
@pytest.mark.jtag
@pytest.mark.parametrize(
    'embedded_services',
    [
        'esp,idf,jtag',
    ],
    indirect=True,
)
def test_examples_sysview_tracing(dut: IdfDut) -> None:
    def dut_expect_task_event() -> None:
        dut.expect(re.compile(rb'example: Task\[0x3[0-9A-Fa-f]+\]: received event \d+'), timeout=30)

    dut.gdb.write('mon reset halt')
    dut.gdb.write('maintenance flush register-cache')
    dut.gdb.write('b app_main')

    dut.gdb.write('commands', non_blocking=True)
    dut.gdb.write(
        'mon esp sysview start file:///tmp/sysview_example0.svdat file:///tmp/sysview_example1.svdat', non_blocking=True
    )
    dut.gdb.write('c', non_blocking=True)
    dut.gdb.write('end')

    dut.gdb.write('c', non_blocking=True)
    time.sleep(1)  # to avoid EOF file error
    with open(dut.gdb._logfile) as fr:  # pylint: disable=protected-access
        gdb_pexpect_proc = pexpect.fdpexpect.fdspawn(fr.fileno())
        gdb_pexpect_proc.expect('Thread 2 "main" hit Breakpoint 1, app_main ()')

        dut.expect('example: Created task')  # dut has been restarted by gdb since the last dut.expect()
        dut_expect_task_event()

        # Do a sleep while sysview samples are captured.
        time.sleep(3)
        # GDB isn't responding now to any commands, therefore, the following command is issued to openocd
        dut.openocd.write('esp sysview stop')
