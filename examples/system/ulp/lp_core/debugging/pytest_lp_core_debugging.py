# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os.path
import time
import typing

import pexpect.fdpexpect
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

if typing.TYPE_CHECKING:
    from conftest import OpenOCD


@idf_parametrize('target', ['esp32c5', 'esp32c6', 'esp32p4', 'esp32s31'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32s31'], reason='s31-lpcore not supported in latest OpenOCD release yet')
@pytest.mark.usb_serial_jtag
def test_lp_core_debugging(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    dut.expect('Do some work on HP core...')

    # Prepare gdbinit file
    gdb_logfile = os.path.join(dut.logdir, 'gdb.txt')
    gdbinit_orig = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'gdbinit')
    gdbinit = os.path.join(dut.logdir, 'gdbinit')
    with open(gdbinit_orig) as f_r, open(gdbinit, 'w') as f_w:
        for line in f_r:
            if line.startswith('add-symbol'):
                f_w.write(f'add-symbol {dut.app.binary_path}/gdbinit/ulp_debug_example.elf\n')
            else:
                f_w.write(line)

    # Launch with gdbinit and check abort function is called
    with (
        openocd_dut.run() as oocd,
        open(gdb_logfile, 'w') as gdb_log,
        pexpect.spawn(
            f'idf.py -B {dut.app.binary_path} gdb --batch -x {gdbinit}',
            timeout=60,
            logfile=gdb_log,
            encoding='utf-8',
            codec_errors='ignore',
        ) as p,
    ):
        p.expect('hit Breakpoint 2, do_crash')
        # dont intercept ebreak in abort
        oocd.write(f'{dut.target}.lp.cpu configure -ebreak exception')
        # let the panic handler finish
        oocd.write('resume')
        time.sleep(2)

    # Validate GDB logs
    with open(gdb_logfile, encoding='utf-8') as fr:  # pylint: disable=protected-access
        gdb_pexpect_proc = pexpect.fdpexpect.fdspawn(fr.fileno())
        gdb_pexpect_proc.expect('hit Temporary breakpoint 1, main')
        gdb_pexpect_proc.expect('hit Breakpoint 3, do_things')
        gdb_pexpect_proc.expect('hit Breakpoint 4, ulp_lp_core_delay_us')
        gdb_pexpect_proc.expect('hit Breakpoint 2, do_crash')
