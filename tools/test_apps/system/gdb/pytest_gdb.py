# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import json
import logging
import os
import re
import subprocess
import time
from subprocess import Popen
from typing import Optional

import pexpect
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

MAX_RETRIES = 3
RETRY_DELAY = 3  # seconds


def run_openocd(dut: IdfDut) -> Optional[Popen]:
    desc_path = os.path.join(dut.app.binary_path, 'project_description.json')
    try:
        with open(desc_path, 'r') as f:
            project_desc = json.load(f)
    except FileNotFoundError:
        logging.error('Project description file not found at %s', desc_path)
        return None

    openocd_scripts = os.getenv('OPENOCD_SCRIPTS')
    if not openocd_scripts:
        logging.error('OPENOCD_SCRIPTS environment variable is not set.')
        return None

    debug_args = project_desc.get('debug_arguments_openocd')
    if not debug_args:
        logging.error("'debug_arguments_openocd' key is missing in project_description.json")
        return None

    cmd = ['openocd'] + ['-s', openocd_scripts] + debug_args.split()

    # For debug purpose, make the value '4'
    ocd_env = os.environ.copy()
    ocd_env['LIBUSB_DEBUG'] = '1'

    for attempt in range(1, MAX_RETRIES + 1):
        logging.info('Attempt %d: Running %s', attempt, cmd)
        with open(os.path.join(dut.logdir, 'ocd.txt'), 'w') as ocd_log:
            try:
                ocd = subprocess.Popen(cmd, stdout=ocd_log, stderr=ocd_log, env=ocd_env)
                time.sleep(1)

                # Check if the process is running successfully
                if ocd.poll() is None:
                    return ocd
                else:
                    logging.error('OpenOCD exited with error code %d', ocd.returncode)
            except subprocess.SubprocessError as e:
                logging.error('Error running OpenOCD: %s', e)

        logging.warning("OpenOCD couldn't be run. Retrying in %d seconds...", RETRY_DELAY)
        time.sleep(RETRY_DELAY)

    logging.error('Failed to run OpenOCD after %d attempts.', MAX_RETRIES)

    return None


def _test_idf_gdb(dut: IdfDut) -> None:
    # Need to wait a moment to connect via OpenOCD after the hard reset happened.
    # Along with this check that app runs ok
    dut.expect('Hello world!')

    # Don't need to have output from UART anymore
    dut.serial.stop_redirect_thread()

    ocd = run_openocd(dut)
    assert ocd

    try:
        with open(os.path.join(dut.logdir, 'gdb.txt'), 'w') as gdb_log, pexpect.spawn(
            f'idf.py -B {dut.app.binary_path} gdb --batch',
            timeout=60,
            logfile=gdb_log,
            encoding='utf-8',
            codec_errors='ignore',
        ) as p:
            p.expect(re.compile(r'add symbol table from file.*bootloader.elf'))
            p.expect(
                re.compile(r'add symbol table from file.*rom.elf')
            )  # if fail here: add target support here https://github.com/espressif/esp-rom-elfs
            p.expect_exact('hit Temporary breakpoint 1, app_main ()')
    finally:
        # Check if the process is still running
        if ocd.poll() is None:
            ocd.terminate()
            ocd.kill()


@pytest.mark.jtag
@idf_parametrize('target', ['esp32', 'esp32s2'], indirect=['target'])
def test_idf_gdb(dut: IdfDut) -> None:
    _test_idf_gdb(dut)


@pytest.mark.usb_serial_jtag
@idf_parametrize('target', ['esp32c6', 'esp32h2'], indirect=['target'])
def test_idf_gdb_usj(dut: IdfDut) -> None:
    _test_idf_gdb(dut)
