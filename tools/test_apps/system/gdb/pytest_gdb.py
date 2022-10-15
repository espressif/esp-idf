# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os
import re
import signal
import subprocess
import sys

import pexpect
import pytest
from pytest_embedded import Dut

try:
    from idf_py_actions.debug_ext import get_openocd_arguments
except ModuleNotFoundError:
    sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
    from idf_py_actions.debug_ext import get_openocd_arguments


@pytest.mark.supported_targets
@pytest.mark.test_jtag_arm
def test_idf_gdb(dut: Dut) -> None:
    # Need to wait a moment to connect via OpenOCD after the hard reset happened.
    # Along with this check that app runs ok
    dut.expect('Hello world!')

    # Don't need to have output from UART any more
    dut.serial.stop_redirect_thread()

    with open(os.path.join(dut.logdir, 'ocd.log'), 'w') as ocd_log:
        ocd = subprocess.Popen(f'openocd {get_openocd_arguments(dut.target)}', stdout=ocd_log, stderr=ocd_log, shell=True)

        try:
            gdb_env = os.environ.copy()
            gdb_env['ESP_IDF_GDB_TESTING'] = '1'

            with open(os.path.join(dut.logdir, 'gdb.log'), 'w') as gdb_log, \
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
            try:
                ocd.send_signal(signal.SIGINT)
                ocd.communicate(timeout=15)
            except subprocess.TimeoutExpired:
                ocd.kill()
                ocd.communicate()
