# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import json
import logging
import os
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


def _test_examples_app_trace_basic(dut: IdfDut) -> None:
    dut.expect_exact('example: Waiting for OpenOCD connection', timeout=5)
    openocd = OpenOCD(dut).run()
    assert openocd
    try:
        openocd.connect_telnet()
        openocd.write('log_output {}'.format(openocd.log_file))
        openocd.write('reset run')
        dut.expect_exact('example: Waiting for OpenOCD connection', timeout=5)
        time.sleep(1)  # wait for APPTRACE_INIT semihosting call
        openocd.write('esp apptrace start file://apptrace.log 0 2000 3 0 0')
        openocd.apptrace_wait_stop()

        search_strings = ['Targets connected.', 'Disconnect targets...']
        with open(openocd.log_file, encoding='utf-8') as oocd_log:  # pylint: disable=protected-access
            cores = 1 if dut.app.sdkconfig.get('ESP_SYSTEM_SINGLE_CORE_MODE') is True else 2
            search_strings.append('App trace params: from {} cores,'.format(cores))
            for search_str in search_strings:
                found = False
                oocd_log.seek(0)
                for line in oocd_log:
                    if search_str in line:
                        found = True
                        break
                if found is not True:
                    raise RuntimeError(f'"{search_str}" could not be found in {openocd.log_file}')  # pylint: disable=protected-access

        with open('apptrace.log', encoding='utf-8') as apptrace_log:
            for sample_num in range(1, 51):
                log_str = 'Apptrace test data[{}]:{}'.format(sample_num, sample_num * sample_num)
                found = False
                for line in apptrace_log:
                    if log_str in line:
                        found = True
                        break
                if found is not True:
                    raise RuntimeError('"{}" could not be found in {}'.format(log_str, 'apptrace.log'))
    finally:
        openocd.kill()


@pytest.mark.jtag
@idf_parametrize('target', ['esp32', 'esp32c2', 'esp32s2'], indirect=['target'])
def test_examples_app_trace_basic(dut: IdfDut) -> None:
    _test_examples_app_trace_basic(dut)


@pytest.mark.usb_serial_jtag
@idf_parametrize('target', ['esp32s3', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2'], indirect=['target'])
def test_examples_app_trace_basic_usj(dut: IdfDut) -> None:
    _test_examples_app_trace_basic(dut)
