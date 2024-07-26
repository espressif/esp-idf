# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import logging
import os
import re
import subprocess
import sys
from typing import Any
from typing import Dict
from typing import List
from typing import Optional
from typing import TextIO
from typing import Union

import pexpect
from panic_utils import attach_logger
from panic_utils import NoGdbProcessError
from panic_utils import quote_string
from panic_utils import sha256
from panic_utils import verify_valid_gdb_subprocess
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

    def __init__(self, *args: Any, **kwargs: Any) -> None:
        super().__init__(*args, **kwargs)

        self.gdbmi: Optional[GdbController] = None
        # record this since pygdbmi is using logging.debug to generate some single character mess
        self.log_level = logging.getLogger().level
        # pygdbmi is using logging.debug to generate some single character mess
        if self.log_level <= logging.DEBUG:
            logging.getLogger().setLevel(logging.INFO)

        self.coredump_output: Optional[TextIO] = None

    def close(self) -> None:
        if self.gdbmi:
            logging.info('Waiting for GDB to exit')
            self.gdbmi.exit()

        super().close()

    def revert_log_level(self) -> None:
        logging.getLogger().setLevel(self.log_level)

    @property
    def is_xtensa(self) -> bool:
        return self.target in self.XTENSA_TARGETS

    @property
    def is_multi_core(self) -> bool:
        return self.target in ['esp32', 'esp32s3', 'esp32p4']

    def run_test_func(self, test_func_name: str) -> None:
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
        assert self.is_xtensa, 'Backtrace can be printed only on Xtensa'
        match = self.expect(r'Backtrace:( 0x[0-9a-fA-F]{8}:0x[0-9a-fA-F]{8})+(?P<corrupted> \|<-CORRUPTED)?')
        assert not match.group('corrupted')

    def expect_corrupted_backtrace(self) -> None:
        assert self.is_xtensa, 'Backtrace can be printed only on Xtensa'
        self.expect_exact('Backtrace:')
        self.expect_exact('CORRUPTED')

    def expect_stack_dump(self) -> None:
        assert not self.is_xtensa, 'Stack memory dump is only printed on RISC-V'
        self.expect_exact('Stack memory:')

    def expect_gme(self, reason: str) -> None:
        """Expect method for Guru Meditation Errors"""
        self.expect_exact(f"Guru Meditation Error: Core  0 panic'ed ({reason})")

    def expect_reg_dump(self, core: int = 0) -> None:
        """Expect method for the register dump"""
        self.expect(r'Core\s+%d register dump:' % core)

    def expect_cpu_reset(self) -> None:
        # no digital system reset for panic handling restarts (see IDF-7255)
        self.expect(r'.*rst:.*(RTC_SW_CPU_RST|SW_CPU_RESET|SW_CPU)')

    def expect_elf_sha256(self) -> None:
        """Expect method for ELF SHA256 line"""
        elf_sha256 = sha256(self.app.elf_file)
        elf_sha256_len = int(
            self.app.sdkconfig.get('CONFIG_APP_RETRIEVE_LEN_ELF_SHA', '9')
        )
        self.expect_exact('ELF file SHA256: ' + elf_sha256[0:elf_sha256_len])

    def expect_coredump(self, output_file_name: str, patterns: List[Union[str, re.Pattern]]) -> None:
        with open(output_file_name, 'r') as file:
            coredump = file.read()
            for pattern in patterns:
                if isinstance(pattern, str):
                    position = coredump.find(pattern)
                    assert position != -1, f"'{pattern}' not in the coredump output"
                elif isinstance(pattern, re.Pattern):
                    match = pattern.findall(coredump)
                    assert match, f"'{pattern.pattern}' not in the coredump output"
                else:
                    raise ValueError(f'Unsupported input type: {type(pattern).__name__}')

    def _call_espcoredump(
        self, extra_args: List[str], output_file_name: str
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
            '-b115200',
            'info_corefile',
        ]
        espcoredump_args += extra_args
        espcoredump_args.append(self.app.elf_file)
        logging.info('Running %s', ' '.join(espcoredump_args))
        logging.info('espcoredump output is written to %s', self.coredump_output.name)

        self.serial.close()
        subprocess.check_call(espcoredump_args, stdout=self.coredump_output)
        self.coredump_output.flush()
        self.coredump_output.seek(0)

    def process_coredump_uart(self, expected: Optional[List[Union[str, re.Pattern]]] = None) -> None:
        """Extract the core dump from UART output of the test, run espcoredump on it"""
        self.expect(self.COREDUMP_UART_START)
        res = self.expect('(.+)' + self.COREDUMP_UART_END)
        coredump_base64 = res.group(1).decode('utf8')
        with open(os.path.join(self.logdir, 'coredump_data.b64'), 'w') as coredump_file:
            logging.info('Writing UART base64 core dump to %s', coredump_file.name)
            coredump_file.write(coredump_base64)

        output_file_name = os.path.join(self.logdir, 'coredump_uart_result.txt')
        self._call_espcoredump(
            ['--core-format', 'b64', '--core', coredump_file.name], output_file_name
        )
        if expected:
            self.expect_coredump(output_file_name, expected)

    def process_coredump_flash(self, expected: Optional[List[Union[str, re.Pattern]]] = None) -> Any:
        coredump_file_name = os.path.join(self.logdir, 'coredump_data.bin')
        logging.info('Writing flash binary core dump to %s', coredump_file_name)
        output_file_name = os.path.join(self.logdir, 'coredump_flash_result.txt')
        self._call_espcoredump(
            ['--core-format', 'raw', '--save-core', coredump_file_name], output_file_name
        )
        if expected:
            self.expect_coredump(output_file_name, expected)
        return coredump_file_name

    def gdb_write(self, command: str) -> Any:
        """
        Wrapper to write to gdb with a longer timeout, as test runner
        host can be slow sometimes
        """
        assert self.gdbmi, 'This function should be called only after run_gdb'
        return self.gdbmi.write(command, timeout_sec=10)

    def run_gdb(self) -> None:
        """
        Runs GDB and connects it to the "serial" port of the DUT.
        After this, the DUT expect methods can no longer be used to capture output.
        """
        gdb_args = ['--nx', '--quiet', '--interpreter=mi2']
        if self.is_xtensa:
            gdb_path = f'xtensa-{self.target}-elf-gdb'
        else:
            gdb_path = 'riscv32-esp-elf-gdb'
        try:
            from pygdbmi.constants import GdbTimeoutError
            gdb_command = [gdb_path] + gdb_args
            self.gdbmi = GdbController(command=gdb_command)
            pygdbmi_logger = attach_logger()
        except ImportError:
            # fallback for pygdbmi<0.10.0.0.
            from pygdbmi.gdbcontroller import GdbTimeoutError
            self.gdbmi = GdbController(gdb_path=gdb_path, gdb_args=gdb_args)
            pygdbmi_logger = self.gdbmi.logger

        # pygdbmi logs to console by default, make it log to a file instead
        pygdbmi_log_file_name = os.path.join(self.logdir, 'pygdbmi_log.txt')
        pygdbmi_logger.setLevel(logging.DEBUG)
        while pygdbmi_logger.hasHandlers():
            pygdbmi_logger.removeHandler(pygdbmi_logger.handlers[0])
        log_handler = logging.FileHandler(pygdbmi_log_file_name)
        log_handler.setFormatter(
            logging.Formatter('%(asctime)s %(levelname)s: %(message)s')
        )
        logging.info(f'Saving pygdbmi logs to {pygdbmi_log_file_name}')
        pygdbmi_logger.addHandler(log_handler)
        try:
            gdb_command = self.gdbmi.command
        except AttributeError:
            # fallback for pygdbmi < 0.10
            gdb_command = self.gdbmi.cmd

        logging.info(f'Running command: "{" ".join(quote_string(c) for c in gdb_command)}"')
        for _ in range(10):
            try:
                # GdbController creates a process with subprocess.Popen(). Is it really running? It is probable that
                # an RPI under high load will get non-responsive during creating a lot of processes.
                if not hasattr(self.gdbmi, 'verify_valid_gdb_subprocess'):
                    # for pygdbmi >= 0.10.0.0
                    verify_valid_gdb_subprocess(self.gdbmi.gdb_process)
                resp = self.gdbmi.get_gdb_response(
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

    # Prepare gdb for the gdb stub
    def start_gdb_for_gdbstub(self) -> None:

        self.run_gdb()

        # Connect GDB to UART
        self.serial.close()
        logging.info('Connecting to GDB Stub...')
        self.gdb_write('-gdb-set serial baud 115200')

        if sys.platform == 'darwin':
            assert '/dev/tty.' not in self.serial.port, \
                '/dev/tty.* ports can\'t be used with GDB on macOS. Use with /dev/cu.* instead.'

        # Make sure we get the 'stopped' notification
        responses = self.gdb_write('-target-select remote ' + self.serial.port)
        stop_response = self.find_gdb_response('stopped', 'notify', responses)

        retries = 3
        while not stop_response and retries > 0:
            logging.info('Sending -exec-interrupt')
            responses = self.gdb_write('-exec-interrupt')
            stop_response = self.find_gdb_response('stopped', 'notify', responses)
            retries -= 1

        frame = stop_response['payload']['frame']
        if 'file' not in frame:
            frame['file'] = '?'
        if 'line' not in frame:
            frame['line'] = '?'
        logging.info('Stopped in {func} at {addr} ({file}:{line})'.format(**frame))

        # Drain remaining responses
        if self.gdbmi:
            self.gdbmi.get_gdb_response(raise_error_on_timeout=False)

    # Prepare gdb to debug coredump file
    def start_gdb_for_coredump(self, elf_file: str) -> None:

        self.run_gdb()
        self.gdb_write('core {}'.format(elf_file))

    def gdb_backtrace(self) -> Any:
        """
        Returns the list of stack frames for the current thread.
        Each frame is a dictionary, refer to pygdbmi docs for the format.
        """
        assert self.gdbmi

        responses = self.gdb_write('-stack-list-frames')
        return self.find_gdb_response('done', 'result', responses)['payload']['stack']

    def gdb_data_eval_expr(self, expr: str) -> Any:
        responses = self.gdb_write('-data-evaluate-expression "%s"' % expr)
        return self.find_gdb_response('done', 'result', responses)['payload']['value']

    @staticmethod
    def verify_gdb_backtrace(
        gdb_backtrace: List[Any], expected_functions_list: List[Any]
    ) -> None:
        """
        Raises an assert if the function names listed in expected_functions_list do not match the backtrace
        given by gdb_backtrace argument. The latter is in the same format as returned by gdb_backtrace()
        function.
        """
        actual_functions_list = [frame['func'] for frame in gdb_backtrace]
        if actual_functions_list != expected_functions_list:
            logging.error(f'Expected backtrace: {expected_functions_list}')
            logging.error(f'Actual backtrace: {actual_functions_list}')
            assert False, 'Got unexpected backtrace'

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
