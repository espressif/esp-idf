# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import json
import logging
import os
import re
import subprocess

import pexpect
import pytest
from pytest_embedded_idf import IdfDut


@pytest.mark.supported_targets
@pytest.mark.jtag
def test_idf_gdb(dut: IdfDut) -> None:
    # Need to wait a moment to connect via OpenOCD after the hard reset happened.
    # Along with this check that app runs ok
    dut.expect('Hello world!')

    # Don't need to have output from UART anymore
    dut.serial.stop_redirect_thread()

    desc_path = os.path.join(dut.app.binary_path, 'project_description.json')
    with open(desc_path, 'r') as f:
        project_desc = json.load(f)

    with open(os.path.join(dut.logdir, 'ocd.txt'), 'w') as ocd_log:
        cmd = ['openocd'] + project_desc['debug_arguments_openocd'].split()
        openocd_scripts = os.getenv('OPENOCD_SCRIPTS')
        if openocd_scripts:
            cmd.extend(['-s', openocd_scripts])

        logging.info('Running %s', cmd)
        ocd = subprocess.Popen(cmd, stdout=ocd_log, stderr=ocd_log)

        try:
            with open(os.path.join(dut.logdir, 'gdb.txt'), 'w') as gdb_log, \
                pexpect.spawn(f'idf.py -B {dut.app.binary_path} gdb --batch',
                              timeout=60,
                              logfile=gdb_log,
                              encoding='utf-8',
                              codec_errors='ignore') as p:
                p.expect(re.compile(r'add symbol table from file.*bootloader.elf'))
                p.expect(re.compile(r'add symbol table from file.*rom.elf'))  # if fail here: add target support here https://github.com/espressif/esp-rom-elfs
                p.expect_exact('hit Temporary breakpoint 1, app_main ()')
        finally:
            ocd.terminate()
            ocd.kill()
