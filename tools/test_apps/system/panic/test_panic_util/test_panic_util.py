import logging
import os
import re
import subprocess
import sys
from subprocess import Popen

import ttfw_idf
from pygdbmi.gdbcontroller import GdbController
from tiny_test_fw import DUT, TinyFW, Utility
from tiny_test_fw.Utility import CaseConfig, SearchCases

# hard-coded to the path one level above - only intended to be used from the panic test app
TEST_PATH = os.path.relpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'), os.getenv('IDF_PATH'))
TEST_SUITE = 'Panic'


class NoGdbProcessError(ValueError):
    """Raise when trying to interact with gdb subprocess, but it does not exist.
    It may have been killed and removed, or failed to initialize for some reason."""

    pass


def verify_valid_gdb_subprocess(gdb_process: Popen) -> None:
    """Verify there is a process object, and that it is still running.
    Raise NoGdbProcessError if either of the above are not true."""
    if not gdb_process:
        raise NoGdbProcessError('gdb process is not attached')

    elif gdb_process.poll() is not None:
        raise NoGdbProcessError(
            'gdb process has already finished with return code: %s'
            % str(gdb_process.poll())
        )


def ok(data):
    """ Helper function used with dut.expect_any """
    pass


def unexpected(data):
    """ Helper function used with dut.expect_any """
    raise AssertionError('Unexpected: {}'.format(data))


class PanicTestApp(ttfw_idf.TestApp):
    pass


class PanicTestMixin(object):
    """ Provides custom functionality for the panic test DUT """
    BOOT_CMD_ADDR = 0x9000
    BOOT_CMD_SIZE = 0x1000
    DEFAULT_EXPECT_TIMEOUT = 10
    COREDUMP_UART_START = '================= CORE DUMP START ================='
    COREDUMP_UART_END = '================= CORE DUMP END ================='

    def start_test(self, test_name):
        """ Starts the app and sends it the test name """
        self.test_name = test_name
        # Start the app and verify that it has started up correctly
        self.start_capture_raw_data()
        self.start_app()
        self.expect('Enter test name: ')
        Utility.console_log('Setting boot command: ' + test_name)
        self.write(test_name)
        self.expect('Got test name: ' + test_name)

    def expect_none(self, *patterns, **timeout_args):
        """ like dut.expect_all, but with an inverse logic """
        found_data = []
        if 'timeout' not in timeout_args:
            timeout_args['timeout'] = 1

        def found(data):
            raise AssertionError('Unexpected: {}'.format(data))
            found_data.append(data)
        try:
            expect_items = [(pattern, found) for pattern in patterns]
            self.expect_any(*expect_items, **timeout_args)
            raise AssertionError('Unexpected: {}'.format(found_data))
        except DUT.ExpectTimeout:
            return True

    def expect_gme(self, reason):
        """ Expect method for Guru Meditation Errors """
        self.expect(r"Guru Meditation Error: Core  0 panic'ed (%s)" % reason)

    def expect_reg_dump(self, core=0):
        """ Expect method for the register dump """
        self.expect(re.compile(r'Core\s+%d register dump:' % core))

    def expect_elf_sha256(self):
        """ Expect method for ELF SHA256 line """
        elf_sha256 = self.app.get_elf_sha256()
        sdkconfig = self.app.get_sdkconfig()
        elf_sha256_len = int(sdkconfig.get('CONFIG_APP_RETRIEVE_LEN_ELF_SHA', '16'))
        self.expect('ELF file SHA256: ' + elf_sha256[0:elf_sha256_len])

    def expect_backtrace(self):
        self.expect('Backtrace:')
        self.expect_none('CORRUPTED')

    def __enter__(self):
        self._raw_data = None
        self.gdb = None
        return self

    def __exit__(self, type, value, traceback):
        log_folder = self.app.get_log_folder(TEST_SUITE)
        with open(os.path.join(log_folder, 'log_' + self.test_name + '.txt'), 'w') as log_file:
            Utility.console_log('Writing output of {} to {}'.format(self.test_name, log_file.name))
            log_file.write(self.get_raw_data())
        if self.gdb:
            self.gdb.exit()
        self.close()

    def get_raw_data(self):
        if not self._raw_data:
            self._raw_data = self.stop_capture_raw_data()
        return self._raw_data

    def _call_espcoredump(self, extra_args, coredump_file_name, output_file_name):
        # no "with" here, since we need the file to be open for later inspection by the test case
        self.coredump_output = open(output_file_name, 'w')
        espcoredump_script = os.path.join(os.environ['IDF_PATH'], 'components', 'espcoredump', 'espcoredump.py')
        espcoredump_args = [
            sys.executable,
            espcoredump_script,
            'info_corefile',
            '--core', coredump_file_name,
        ]
        espcoredump_args += extra_args
        espcoredump_args.append(self.app.elf_file)
        Utility.console_log('Running ' + ' '.join(espcoredump_args))
        Utility.console_log('espcoredump output is written to ' + self.coredump_output.name)

        subprocess.check_call(espcoredump_args, stdout=self.coredump_output)
        self.coredump_output.flush()
        self.coredump_output.seek(0)

    def process_coredump_uart(self):
        """ Extract the core dump from UART output of the test, run espcoredump on it """
        log_folder = self.app.get_log_folder(TEST_SUITE)
        data = self.get_raw_data()
        coredump_start = data.find(self.COREDUMP_UART_START)
        coredump_end = data.find(self.COREDUMP_UART_END)
        coredump_base64 = data[coredump_start + len(self.COREDUMP_UART_START):coredump_end]
        with open(os.path.join(log_folder, 'coredump_data_' + self.test_name + '.b64'), 'w') as coredump_file:
            Utility.console_log('Writing UART base64 core dump to ' + coredump_file.name)
            coredump_file.write(coredump_base64)

        output_file_name = os.path.join(log_folder, 'coredump_uart_result_' + self.test_name + '.txt')
        self._call_espcoredump(['--core-format', 'b64'], coredump_file.name, output_file_name)

    def process_coredump_flash(self):
        """ Extract the core dump from flash, run espcoredump on it """
        log_folder = self.app.get_log_folder(TEST_SUITE)
        coredump_file_name = os.path.join(log_folder, 'coredump_data_' + self.test_name + '.bin')
        Utility.console_log('Writing flash binary core dump to ' + coredump_file_name)
        self.dump_flash(coredump_file_name, partition='coredump')

        output_file_name = os.path.join(log_folder, 'coredump_flash_result_' + self.test_name + '.txt')
        self._call_espcoredump(['--core-format', 'raw'], coredump_file_name, output_file_name)

    def _gdb_write(self, command):
        """
        Wrapper to write to gdb with a longer timeout, as test runner
        host can be slow sometimes
        """
        return self.gdb.write(command, timeout_sec=10)

    def start_gdb(self):
        """
        Runs GDB and connects it to the "serial" port of the DUT.
        After this, the DUT expect methods can no longer be used to capture output.
        """
        self.stop_receive()
        self._port_close()

        Utility.console_log('Starting GDB...', 'orange')
        gdb_path = self.TOOLCHAIN_PREFIX + 'gdb'
        try:
            from pygdbmi.constants import GdbTimeoutError
            default_gdb_args = ['--nx', '--quiet', '--interpreter=mi2']
            gdb_command = [gdb_path] + default_gdb_args
            self.gdb = GdbController(command=gdb_command)
        except ImportError:
            # fallback for pygdbmi<0.10.0.0.
            from pygdbmi.gdbcontroller import GdbTimeoutError
            self.gdb = GdbController(gdb_path=gdb_path)

        try:
            gdb_command = self.gdb.command
        except AttributeError:
            # fallback for pygdbmi < 0.10
            gdb_command = self.gdb.cmd

        Utility.console_log('Running command: {}'.format(gdb_command), 'orange')

        for _ in range(10):
            try:
                # GdbController creates a process with subprocess.Popen(). Is it really running? It is probable that
                # an RPI under high load will get non-responsive during creating a lot of processes.
                if not hasattr(self.gdb, 'verify_valid_gdb_subprocess'):
                    # for pygdbmi >= 0.10.0.0
                    verify_valid_gdb_subprocess(self.gdb.gdb_process)
                resp = self.gdb.get_gdb_response(timeout_sec=10)  # calls verify_valid_gdb_subprocess() internally (pygdbmi < 0.10)
                # it will be interesting to look up this response if the next GDB command fails (times out)
                Utility.console_log('GDB response: {}'.format(resp), 'orange')
                break  # success
            except GdbTimeoutError:
                Utility.console_log('GDB internal error: cannot get response from the subprocess', 'orange')
            except NoGdbProcessError:
                Utility.console_log('GDB internal error: process is not running', 'red')
                break  # failure - TODO: create another GdbController
            except ValueError:
                Utility.console_log('GDB internal error: select() returned an unexpected file number', 'red')

        # pygdbmi logs to console by default, make it log to a file instead
        log_folder = self.app.get_log_folder(TEST_SUITE)
        pygdbmi_log_file_name = os.path.join(log_folder, 'pygdbmi_log_' + self.test_name + '.txt')
        pygdbmi_logger = self.gdb.logger
        pygdbmi_logger.setLevel(logging.DEBUG)
        while pygdbmi_logger.hasHandlers():
            pygdbmi_logger.removeHandler(pygdbmi_logger.handlers[0])
        log_handler = logging.FileHandler(pygdbmi_log_file_name)
        log_handler.setFormatter(logging.Formatter('%(asctime)s %(levelname)s: %(message)s'))
        pygdbmi_logger.addHandler(log_handler)

        # Set up logging for GDB remote protocol
        gdb_remotelog_file_name = os.path.join(log_folder, 'gdb_remote_log_' + self.test_name + '.txt')
        self._gdb_write('-gdb-set remotelogfile ' + gdb_remotelog_file_name)

        # Load the ELF file
        self._gdb_write('-file-exec-and-symbols {}'.format(self.app.elf_file))

        # Connect GDB to UART
        Utility.console_log('Connecting to GDB Stub...', 'orange')
        self._gdb_write('-gdb-set serial baud 115200')
        responses = self._gdb_write('-target-select remote ' + self.get_gdb_remote())

        # Make sure we get the 'stopped' notification
        stop_response = self.find_gdb_response('stopped', 'notify', responses)
        if not stop_response:
            responses = self._gdb_write('-exec-interrupt')
            stop_response = self.find_gdb_response('stopped', 'notify', responses)
            assert stop_response
        frame = stop_response['payload']['frame']
        if 'file' not in frame:
            frame['file'] = '?'
        if 'line' not in frame:
            frame['line'] = '?'
        Utility.console_log('Stopped in {func} at {addr} ({file}:{line})'.format(**frame), 'orange')

        # Drain remaining responses
        self.gdb.get_gdb_response(raise_error_on_timeout=False)

    def gdb_backtrace(self):
        """
        Returns the list of stack frames for the current thread.
        Each frame is a dictionary, refer to pygdbmi docs for the format.
        """
        assert self.gdb

        responses = self._gdb_write('-stack-list-frames')
        return self.find_gdb_response('done', 'result', responses)['payload']['stack']

    @staticmethod
    def match_backtrace(gdb_backtrace, expected_functions_list):
        """
        Returns True if the function names listed in expected_functions_list match the backtrace
        given by gdb_backtrace argument. The latter is in the same format as returned by gdb_backtrace()
        function.
        """
        return all([frame['func'] == expected_functions_list[i] for i, frame in enumerate(gdb_backtrace)])

    @staticmethod
    def find_gdb_response(message, response_type, responses):
        """
        Helper function which extracts one response from an array of GDB responses, filtering
        by message and type. Returned message is a dictionary, refer to pygdbmi docs for the format.
        """
        def match_response(response):
            return (response['message'] == message and
                    response['type'] == response_type)

        filtered_responses = [r for r in responses if match_response(r)]
        if not filtered_responses:
            return None
        return filtered_responses[0]


class ESP32PanicTestDUT(ttfw_idf.ESP32DUT, PanicTestMixin):
    def get_gdb_remote(self):
        return self.port


class ESP32S2PanicTestDUT(ttfw_idf.ESP32S2DUT, PanicTestMixin):
    def get_gdb_remote(self):
        return self.port


PANIC_TEST_DUT_DICT = {
    'ESP32': ESP32PanicTestDUT,
    'ESP32S2': ESP32S2PanicTestDUT
}


def panic_test(**kwargs):
    """ Decorator for the panic tests, sets correct App and DUT classes """
    if 'target' not in kwargs:
        kwargs['target'] = ['ESP32']

    if 'additional_duts' not in kwargs:
        kwargs['additional_duts'] = PANIC_TEST_DUT_DICT
    return ttfw_idf.idf_custom_test(app=PanicTestApp, env_tag='Example_GENERIC', **kwargs)


def get_dut(env, app_config_name, test_name, qemu_wdt_enable=False):
    dut = env.get_dut('panic', TEST_PATH, app_config_name=app_config_name, allow_dut_exception=True)
    dut.qemu_wdt_enable = qemu_wdt_enable
    """ Wrapper for getting the DUT and starting the test """
    dut.start_test(test_name)
    return dut


def run_all(filename, case_filter=[]):
    """ Helper function to run test cases defined in a file; to be called from __main__.
        case_filter is an optional list of case names to run.
        If not specified, all test cases are run.
    """
    TinyFW.set_default_config(env_config_file=None, test_suite_name=TEST_SUITE)
    test_methods = SearchCases.Search.search_test_cases(filename)
    test_methods = filter(lambda m: not m.case_info['ignore'], test_methods)
    test_cases = CaseConfig.Parser.apply_config(test_methods, None)
    tests_failed = []
    for case in test_cases:
        test_name = case.test_method.__name__
        if case_filter:
            if case_filter[0].endswith('*'):
                if not test_name.startswith(case_filter[0][:-1]):
                    continue
            else:
                if test_name not in case_filter:
                    continue
        result = case.run()
        if not result:
            tests_failed.append(case)

    if tests_failed:
        print('The following tests have failed:')
        for case in tests_failed:
            print(' - ' + case.test_method.__name__)
        raise SystemExit(1)

    print('Tests pass')
