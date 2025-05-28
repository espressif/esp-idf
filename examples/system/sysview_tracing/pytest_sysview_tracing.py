# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import json
import logging
import os.path
import re
import signal
import time
from telnetlib import Telnet
from typing import Any
from typing import Optional

import pexpect
import pytest
from pytest_embedded.utils import to_bytes
from pytest_embedded.utils import to_str
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

MAX_RETRIES = 3
RETRY_DELAY = 1
TELNET_PORT = 4444


class OpenOCD:
    def __init__(self, dut: 'IdfDut'):
        self.dut = dut
        self.telnet: Optional[Telnet] = None
        self.log_file = os.path.join(self.dut.logdir, 'ocd.txt')
        self.proc: Optional[pexpect.spawn] = None

    def run(self) -> Optional['OpenOCD']:
        desc_path = os.path.join(self.dut.app.binary_path, 'project_description.json')

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

        # For debug purposes, make the value '4'
        ocd_env = os.environ.copy()
        ocd_env['LIBUSB_DEBUG'] = '1'

        for _ in range(1, MAX_RETRIES + 1):
            try:
                self.proc = pexpect.spawn(
                    command='openocd',
                    args=['-s', openocd_scripts] + debug_args.split(),
                    timeout=5,
                    encoding='utf-8',
                    codec_errors='ignore',
                    env=ocd_env,
                )
                if self.proc and self.proc.isalive():
                    self.proc.expect_exact('Info : Listening on port 3333 for gdb connections', timeout=5)
                    return self
            except (pexpect.exceptions.EOF, pexpect.exceptions.TIMEOUT) as e:
                logging.error('Error running OpenOCD: %s', str(e))
                if self.proc and self.proc.isalive():
                    self.proc.terminate()
            time.sleep(RETRY_DELAY)

        logging.error('Failed to run OpenOCD after %d attempts.', MAX_RETRIES)
        return None

    def connect_telnet(self) -> None:
        for attempt in range(1, MAX_RETRIES + 1):
            try:
                self.telnet = Telnet('127.0.0.1', TELNET_PORT, 5)
                break
            except ConnectionRefusedError as e:
                logging.error('Error telnet connection: %s in attempt:%d', e, attempt)
                time.sleep(1)
        else:
            raise ConnectionRefusedError

    def write(self, s: str) -> Any:
        if self.telnet is None:
            logging.error('Telnet connection is not established.')
            return ''
        resp = self.telnet.read_very_eager()
        self.telnet.write(to_bytes(s, '\n'))
        resp += self.telnet.read_until(b'>')
        return to_str(resp)

    def apptrace_wait_stop(self, timeout: int = 30) -> None:
        stopped = False
        end_before = time.time() + timeout
        while not stopped:
            cmd_out = self.write('esp apptrace status')
            for line in cmd_out.splitlines():
                if line.startswith('Tracing is STOPPED.'):
                    stopped = True
                    break
            if not stopped and time.time() > end_before:
                raise pexpect.TIMEOUT('Failed to wait for apptrace stop!')
            time.sleep(1)

    def kill(self) -> None:
        # Check if the process is still running
        if self.proc and self.proc.isalive():
            self.proc.terminate()
            self.proc.kill(signal.SIGKILL)


def _test_examples_sysview_tracing(dut: IdfDut) -> None:
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
    openocd = OpenOCD(dut).run()
    assert openocd
    try:
        openocd.connect_telnet()
        openocd.write('log_output {}'.format(openocd.log_file))

        with open(gdb_logfile, 'w') as gdb_log, pexpect.spawn(
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
    finally:
        openocd.kill()


@pytest.mark.jtag
@idf_parametrize('target', ['esp32', 'esp32c2', 'esp32s2'], indirect=['target'])
def test_examples_sysview_tracing(dut: IdfDut) -> None:
    _test_examples_sysview_tracing(dut)


@pytest.mark.usb_serial_jtag
@idf_parametrize('target', ['esp32s3', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2'], indirect=['target'])
def test_examples_sysview_tracing_usj(dut: IdfDut) -> None:
    _test_examples_sysview_tracing(dut)
