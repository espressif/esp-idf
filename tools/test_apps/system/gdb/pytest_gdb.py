# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import logging
import os
import re
import subprocess
import sys

import pexpect
import pytest
from pytest_embedded_idf import IdfDut

try:
    from idf_py_actions.debug_ext import get_openocd_arguments
except ModuleNotFoundError:
    sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
    from idf_py_actions.debug_ext import get_openocd_arguments


@pytest.mark.supported_targets
@pytest.mark.jtag
def test_idf_gdb(dut: IdfDut) -> None:
    # Need to wait a moment to connect via OpenOCD after the hard reset happened.
    # Along with this check that app runs ok
    dut.expect('Hello world!')

    # Don't need to have output from UART anymore
    dut.serial.stop_redirect_thread()

    with open(os.path.join(dut.logdir, 'ocd.txt'), 'w') as ocd_log:
        cmd = ['openocd', *get_openocd_arguments(dut.target).split()]
        openocd_scripts = os.getenv('OPENOCD_SCRIPTS')
        if openocd_scripts:
            cmd.extend(['-s', openocd_scripts])

        logging.info('Running %s', cmd)
        ocd = subprocess.Popen(cmd, stdout=ocd_log, stderr=ocd_log)

        try:
            gdb_env = os.environ.copy()
            gdb_env['ESP_IDF_GDB_TESTING'] = '1'

            with open(os.path.join(dut.logdir, 'gdb.txt'), 'w') as gdb_log, \
                pexpect.spawn(f'idf.py -B {dut.app.binary_path} gdb --batch',
                              env=gdb_env,
                              timeout=60,
                              logfile=gdb_log,
                              encoding='utf-8',
                              codec_errors='ignore') as p:
                p.expect(re.compile(r'add symbol table from file.*bootloader.elf'))
                p.expect(re.compile(r'add symbol table from file.*rom.elf'))
                p.expect_exact('hit Temporary breakpoint 1, app_main ()')
        finally:
            ocd.terminate()
            ocd.kill()
