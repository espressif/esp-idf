# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# pylint: disable=W0621  # redefined-outer-name

import logging
import os
import subprocess
import sys
from typing import Any, Dict, List, TextIO

import pexpect
import pytest
from _pytest.fixtures import FixtureRequest
from _pytest.monkeypatch import MonkeyPatch
from panic_utils import NoGdbProcessError, attach_logger, quote_string, sha256, verify_valid_gdb_subprocess
from pygdbmi.gdbcontroller import GdbController
from pytest_embedded_idf.app import IdfApp
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.serial import IdfSerial


class PanicTestDut(IdfDut):
    BOOT_CMD_ADDR = 0x9000
    BOOT_CMD_SIZE = 0x1000
    DEFAULT_EXPECT_TIMEOUT = 10
    COREDUMP_UART_START = '================= CORE DUMP START ================='
    COREDUMP_UART_END = '================= CORE DUMP END ================='

    app: IdfApp
    serial: IdfSerial

    def __init__(self, *args, **kwargs) -> None:  # type: ignore
        super().__init__(*args, **kwargs)

        self.gdb: GdbController = None  # type: ignore
        # record this since pygdbmi is using logging.debug to generate some single character mess
        self.log_level = logging.getLogger().level
        # pygdbmi is using logging.debug to generate some single character mess
        if self.log_level <= logging.DEBUG:
            logging.getLogger().setLevel(logging.INFO)

        self.coredump_output: TextIO = None  # type: ignore

    def close(self) -> None:
        if self.gdb:
            self.gdb.exit()

        super().close()

    def revert_log_level(self) -> None:
        logging.getLogger().setLevel(self.log_level)

    def expect_test_func_name(self, test_func_name: str) -> None:
        self.expect_exact('Enter test name:')
        self.write(test_func_name)
        self.expect_exact('Got test name: ' + test_func_name)

    def expect_none(self, pattern, **kwargs) -> None:  # type: ignore
        """like dut.expect_all, but with an inverse logic"""
        if 'timeout' not in kwargs:
            kwargs['timeout'] = 1

        try:
            res = self.expect(pattern, **kwargs)
            raise AssertionError(f'Unexpected: {res.group().decode("utf8")}')
        except pexpect.TIMEOUT:
            pass

    def expect_backtrace(self) -> None:
        self.expect_exact('Backtrace:')
        self.expect_none('CORRUPTED')

    def expect_gme(self, reason: str) -> None:
        """Expect method for Guru Meditation Errors"""
        self.expect_exact(f"Guru Meditation Error: Core  0 panic'ed ({reason})")

    def expect_reg_dump(self, core: int = 0) -> None:
        """Expect method for the register dump"""
        self.expect(r'Core\s+%d register dump:' % core)

    def expect_elf_sha256(self) -> None:
        """Expect method for ELF SHA256 line"""
        elf_sha256 = sha256(self.app.elf_file)
        elf_sha256_len = int(
            self.app.sdkconfig.get('CONFIG_APP_RETRIEVE_LEN_ELF_SHA', '16')
        )
        self.expect_exact('ELF file SHA256: ' + elf_sha256[0:elf_sha256_len])

    def _call_espcoredump(
        self, extra_args: List[str], coredump_file_name: str, output_file_name: str
    ) -> None:
        # no "with" here, since we need the file to be open for later inspection by the test case
        if not self.coredump_output:
            self.coredump_output = open(output_file_name, 'w')

        espcoredump_script = os.path.join(
            os.environ['IDF_PATH'], 'components', 'espcoredump', 'espcoredump.py'
        )
        espcoredump_args = [
            sys.executable,
            espcoredump_script,
            'info_corefile',
            '--core',
            coredump_file_name,
        ]
        espcoredump_args += extra_args
        espcoredump_args.append(self.app.elf_file)
        logging.info('Running %s', ' '.join(espcoredump_args))
        logging.info('espcoredump output is written to %s', self.coredump_output.name)

        subprocess.check_call(espcoredump_args, stdout=self.coredump_output)
        self.coredump_output.flush()
        self.coredump_output.seek(0)

    def process_coredump_uart(self) -> None:
        """Extract the core dump from UART output of the test, run espcoredump on it"""
        self.expect(self.COREDUMP_UART_START)
        res = self.expect('(.+)' + self.COREDUMP_UART_END)
        coredump_base64 = res.group(1).decode('utf8')
        with open(os.path.join(self.logdir, 'coredump_data.b64'), 'w') as coredump_file:
            logging.info('Writing UART base64 core dump to %s', coredump_file.name)
            coredump_file.write(coredump_base64)

        output_file_name = os.path.join(self.logdir, 'coredump_uart_result.txt')
        self._call_espcoredump(
            ['--core-format', 'b64'], coredump_file.name, output_file_name
        )

    def process_coredump_flash(self) -> None:
        """Extract the core dump from flash, run espcoredump on it"""
        coredump_file_name = os.path.join(self.logdir, 'coredump_data.bin')
        logging.info('Writing flash binary core dump to %s', coredump_file_name)
        self.serial.dump_flash(partition='coredump', output=coredump_file_name)

        output_file_name = os.path.join(self.logdir, 'coredump_flash_result.txt')
        self._call_espcoredump(
            ['--core-format', 'raw'], coredump_file_name, output_file_name
        )

    def gdb_write(self, command: str) -> Any:
        """
        Wrapper to write to gdb with a longer timeout, as test runner
        host can be slow sometimes
        """
        return self.gdb.write(command, timeout_sec=10)

    def start_gdb(self) -> None:
        """
        Runs GDB and connects it to the "serial" port of the DUT.
        After this, the DUT expect methods can no longer be used to capture output.
        """
        gdb_path = self.toolchain_prefix + 'gdb'
        try:
            from pygdbmi.constants import GdbTimeoutError
            default_gdb_args = ['--nx', '--quiet', '--interpreter=mi2']
            gdb_command = [gdb_path] + default_gdb_args
            self.gdb = GdbController(command=gdb_command)
            pygdbmi_logger = attach_logger()
        except ImportError:
            # fallback for pygdbmi<0.10.0.0.
            from pygdbmi.gdbcontroller import GdbTimeoutError
            self.gdb = GdbController(gdb_path=gdb_path)
            pygdbmi_logger = self.gdb.logger

        # pygdbmi logs to console by default, make it log to a file instead
        pygdbmi_log_file_name = os.path.join(self.logdir, 'pygdbmi_log.txt')
        pygdbmi_logger.setLevel(logging.DEBUG)
        while pygdbmi_logger.hasHandlers():
            pygdbmi_logger.removeHandler(pygdbmi_logger.handlers[0])
        log_handler = logging.FileHandler(pygdbmi_log_file_name)
        log_handler.setFormatter(
            logging.Formatter('%(asctime)s %(levelname)s: %(message)s')
        )
        pygdbmi_logger.addHandler(log_handler)
        try:
            gdb_command = self.gdb.command
        except AttributeError:
            # fallback for pygdbmi < 0.10
            gdb_command = self.gdb.cmd

        logging.info(f'Running command: "{" ".join(quote_string(c) for c in gdb_command)}"')
        for _ in range(10):
            try:
                # GdbController creates a process with subprocess.Popen(). Is it really running? It is probable that
                # an RPI under high load will get non-responsive during creating a lot of processes.
                if not hasattr(self.gdb, 'verify_valid_gdb_subprocess'):
                    # for pygdbmi >= 0.10.0.0
                    verify_valid_gdb_subprocess(self.gdb.gdb_process)
                resp = self.gdb.get_gdb_response(
                    timeout_sec=10
                )  # calls verify_valid_gdb_subprocess() internally for pygdbmi < 0.10.0.0
                # it will be interesting to look up this response if the next GDB command fails (times out)
                logging.info('GDB response: %s', resp)
                break  # success
            except GdbTimeoutError:
                logging.warning(
                    'GDB internal error: cannot get response from the subprocess'
                )
            except NoGdbProcessError:
                logging.error('GDB internal error: process is not running')
                break  # failure - TODO: create another GdbController
            except ValueError:
                logging.error(
                    'GDB internal error: select() returned an unexpected file number'
                )

        # Set up logging for GDB remote protocol
        gdb_remotelog_file_name = os.path.join(self.logdir, 'gdb_remote_log.txt')
        self.gdb_write('-gdb-set remotelogfile ' + gdb_remotelog_file_name)

        # Load the ELF file
        self.gdb_write('-file-exec-and-symbols {}'.format(self.app.elf_file))

        # Connect GDB to UART
        self.serial.proc.close()
        logging.info('Connecting to GDB Stub...')
        self.gdb_write('-gdb-set serial baud 115200')
        responses = self.gdb_write('-target-select remote ' + self.serial.port)

        # Make sure we get the 'stopped' notification
        stop_response = self.find_gdb_response('stopped', 'notify', responses)
        if not stop_response:
            responses = self.gdb_write('-exec-interrupt')
            stop_response = self.find_gdb_response('stopped', 'notify', responses)
            assert stop_response
        frame = stop_response['payload']['frame']
        if 'file' not in frame:
            frame['file'] = '?'
        if 'line' not in frame:
            frame['line'] = '?'
        logging.info('Stopped in {func} at {addr} ({file}:{line})'.format(**frame))

        # Drain remaining responses
        self.gdb.get_gdb_response(raise_error_on_timeout=False)

    def gdb_backtrace(self) -> Any:
        """
        Returns the list of stack frames for the current thread.
        Each frame is a dictionary, refer to pygdbmi docs for the format.
        """
        assert self.gdb

        responses = self.gdb_write('-stack-list-frames')
        return self.find_gdb_response('done', 'result', responses)['payload']['stack']

    @staticmethod
    def match_backtrace(
        gdb_backtrace: List[Any], expected_functions_list: List[Any]
    ) -> bool:
        """
        Returns True if the function names listed in expected_functions_list match the backtrace
        given by gdb_backtrace argument. The latter is in the same format as returned by gdb_backtrace()
        function.
        """
        return all(
            [
                frame['func'] == expected_functions_list[i]
                for i, frame in enumerate(gdb_backtrace)
            ]
        )

    @staticmethod
    def find_gdb_response(
        message: str, response_type: str, responses: List[Any]
    ) -> Any:
        """
        Helper function which extracts one response from an array of GDB responses, filtering
        by message and type. Returned message is a dictionary, refer to pygdbmi docs for the format.
        """

        def match_response(response: Dict[str, Any]) -> bool:
            return response['message'] == message and response['type'] == response_type  # type: ignore

        filtered_responses = [r for r in responses if match_response(r)]
        if not filtered_responses:
            return None
        return filtered_responses[0]


@pytest.fixture(scope='module')
def monkeypatch_module(request: FixtureRequest) -> MonkeyPatch:
    mp = MonkeyPatch()
    request.addfinalizer(mp.undo)
    return mp


@pytest.fixture(scope='module', autouse=True)
def replace_dut_class(monkeypatch_module: MonkeyPatch) -> None:
    monkeypatch_module.setattr('pytest_embedded_idf.dut.IdfDut', PanicTestDut)
