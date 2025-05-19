# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import json
import logging
import os.path
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


def _test_gcov(dut: IdfDut) -> None:
    # create the generated .gcda folder, otherwise would have error: failed to open file.
    # normally this folder would be created via `idf.py build`. but in CI the non-related files would not be preserved
    os.makedirs(os.path.join(dut.app.binary_path, 'esp-idf', 'main', 'CMakeFiles', '__idf_main.dir'), exist_ok=True)
    os.makedirs(os.path.join(dut.app.binary_path, 'esp-idf', 'sample', 'CMakeFiles', '__idf_sample.dir'), exist_ok=True)

    dut.expect_exact('example: Ready for OpenOCD connection', timeout=5)
    openocd = OpenOCD(dut).run()
    assert openocd

    def expect_counter_output(loop: int, timeout: int = 10) -> None:
        dut.expect_exact(
            [f'blink_dummy_func: Counter = {loop}', f'some_dummy_func: Counter = {loop * 2}'],
            expect_all=True,
            timeout=timeout,
        )

    def dump_coverage(cmd: str) -> None:
        response = openocd.write(cmd)

        expect_lines = [
            'Targets connected.',
            'gcov_example_main.c.gcda',
            'gcov_example_func.c.gcda',
            'some_funcs.c.gcda',
            'Targets disconnected.',
        ]

        for line in response.splitlines():
            for expect in expect_lines[:]:
                if expect in line:
                    if expect.endswith('.gcda'):  # check file exists
                        file_path = line.split()[3].strip("'")
                        assert os.path.isfile(file_path)

                    expect_lines.remove(expect)

        assert len(expect_lines) == 0

    try:
        openocd.connect_telnet()
        openocd.write('log_output {}'.format(openocd.log_file))
        openocd.write('reset run')
        dut.expect_exact('example: Ready for OpenOCD connection', timeout=5)

        expect_counter_output(0)
        dut.expect('Ready to dump GCOV data...', timeout=5)

        # Test two hard-coded dumps
        dump_coverage('esp gcov dump')
        dut.expect('GCOV data have been dumped.', timeout=5)
        expect_counter_output(1)
        dut.expect('Ready to dump GCOV data...', timeout=5)
        dump_coverage('esp gcov dump')
        dut.expect('GCOV data have been dumped.', timeout=5)

        for i in range(2, 6):
            expect_counter_output(i)

        for _ in range(3):
            time.sleep(1)
            # Test instant run-time dump
            dump_coverage('esp gcov')
    finally:
        openocd.kill()


@pytest.mark.jtag
@idf_parametrize('target', ['esp32', 'esp32c2', 'esp32s2'], indirect=['target'])
def test_gcov(dut: IdfDut) -> None:
    _test_gcov(dut)


@pytest.mark.usb_serial_jtag
@idf_parametrize('target', ['esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2'], indirect=['target'])
def test_gcov_usj(dut: IdfDut) -> None:
    _test_gcov(dut)
