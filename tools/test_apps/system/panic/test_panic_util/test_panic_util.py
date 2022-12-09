import os
import sys
import re
import subprocess
import ttfw_idf
from tiny_test_fw import Utility, TinyFW, DUT
from tiny_test_fw.Utility import SearchCases, CaseConfig


# hard-coded to the path one level above - only intended to be used from the panic test app
TEST_PATH = os.path.relpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."), os.getenv("IDF_PATH"))
TEST_SUITE = "Panic"


def ok(data):
    """ Helper function used with dut.expect_any """
    pass


def unexpected(data):
    """ Helper function used with dut.expect_any """
    raise AssertionError("Unexpected: {}".format(data))


class PanicTestApp(ttfw_idf.TestApp):
    pass


class PanicTestMixin(object):
    """ Provides custom functionality for the panic test DUT """
    BOOT_CMD_ADDR = 0x9000
    BOOT_CMD_SIZE = 0x1000
    DEFAULT_EXPECT_TIMEOUT = 10
    COREDUMP_UART_START = "================= CORE DUMP START ================="
    COREDUMP_UART_END = "================= CORE DUMP END ================="

    def start_test(self, test_name):
        """ Starts the app and sends it the test name """
        self.test_name = test_name
        # Start the app and verify that it has started up correctly
        self.start_capture_raw_data()
        self.start_app()
        self.expect("Enter test name: ")
        Utility.console_log("Setting boot command: " + test_name)
        self.write(test_name)
        self.expect("Got test name: " + test_name)

    def expect_none(self, *patterns, **timeout_args):
        """ like dut.expect_all, but with an inverse logic """
        found_data = []
        if "timeout" not in timeout_args:
            timeout_args["timeout"] = 1

        def found(data):
            raise AssertionError("Unexpected: {}".format(data))
            found_data.append(data)
        try:
            expect_items = [(pattern, found) for pattern in patterns]
            self.expect_any(*expect_items, **timeout_args)
            raise AssertionError("Unexpected: {}".format(found_data))
        except DUT.ExpectTimeout:
            return True

    def expect_gme(self, reason):
        """ Expect method for Guru Meditation Errors """
        self.expect(r"Guru Meditation Error: Core  0 panic'ed (%s)" % reason)

    def expect_reg_dump(self, core=0):
        """ Expect method for the register dump """
        self.expect(re.compile(r"Core\s+%d register dump:" % core))

    def expect_elf_sha256(self):
        """ Expect method for ELF SHA256 line """
        elf_sha256 = self.app.get_elf_sha256()
        sdkconfig = self.app.get_sdkconfig()
        elf_sha256_len = int(sdkconfig.get("CONFIG_APP_RETRIEVE_LEN_ELF_SHA", "16"))
        self.expect("ELF file SHA256: " + elf_sha256[0:elf_sha256_len])

    def expect_backtrace(self):
        self.expect("Backtrace:")
        self.expect_none("CORRUPTED")

    def __enter__(self):
        self._raw_data = None
        return self

    def __exit__(self, type, value, traceback):
        log_folder = self.app.get_log_folder(TEST_SUITE)
        with open(os.path.join(log_folder, "log_" + self.test_name + ".txt"), "w") as log_file:
            Utility.console_log("Writing output of {} to {}".format(self.test_name, log_file.name))
            log_file.write(self.get_raw_data())

        self.close()

    def get_raw_data(self):
        if not self._raw_data:
            self._raw_data = self.stop_capture_raw_data()
        return self._raw_data

    def _call_espcoredump(self, extra_args, coredump_file_name, output_file_name):
        # no "with" here, since we need the file to be open for later inspection by the test case
        self.coredump_output = open(output_file_name, "w")
        espcoredump_script = os.path.join(os.environ["IDF_PATH"], "components", "espcoredump", "espcoredump.py")
        espcoredump_args = [
            sys.executable,
            espcoredump_script,
            "info_corefile",
            "--core", coredump_file_name,
        ]
        espcoredump_args += extra_args
        espcoredump_args.append(self.app.elf_file)
        Utility.console_log("Running " + " ".join(espcoredump_args))
        Utility.console_log("espcoredump output is written to " + self.coredump_output.name)

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
        with open(os.path.join(log_folder, "coredump_data_" + self.test_name + ".b64"), "w") as coredump_file:
            Utility.console_log("Writing UART base64 core dump to " + coredump_file.name)
            coredump_file.write(coredump_base64)

        output_file_name = os.path.join(log_folder, "coredump_uart_result_" + self.test_name + ".txt")
        self._call_espcoredump(["--core-format", "b64"], coredump_file.name, output_file_name)

    def process_coredump_flash(self):
        """ Extract the core dump from flash, run espcoredump on it """
        log_folder = self.app.get_log_folder(TEST_SUITE)
        coredump_file_name = os.path.join(log_folder, "coredump_data_" + self.test_name + ".bin")
        Utility.console_log("Writing flash binary core dump to " + coredump_file_name)
        self.dump_flush(coredump_file_name, partition="coredump")

        output_file_name = os.path.join(log_folder, "coredump_flash_result_" + self.test_name + ".txt")
        self._call_espcoredump(["--core-format", "raw"], coredump_file_name, output_file_name)


class ESP32PanicTestDUT(ttfw_idf.ESP32DUT, PanicTestMixin):
    def get_gdb_remote(self):
        return self.port


def panic_test(**kwargs):
    """ Decorator for the panic tests, sets correct App and DUT classes """
    return ttfw_idf.idf_custom_test(app=PanicTestApp, dut=ESP32PanicTestDUT, env_tag="test_jtag_arm", **kwargs)


def get_dut(env, app_config_name, test_name, qemu_wdt_enable=False):
    dut = env.get_dut("panic", TEST_PATH, app_config_name=app_config_name, allow_dut_exception=True)
    dut.qemu_wdt_enable = qemu_wdt_enable
    """ Wrapper for getting the DUT and starting the test """
    dut.start_test(test_name)
    return dut


def run_all(filename):
    """ Helper function to run all test cases defined in a file; to be called from __main__. """
    TinyFW.set_default_config(env_config_file=None, test_suite_name=TEST_SUITE)
    test_methods = SearchCases.Search.search_test_cases(filename)
    test_methods = filter(lambda m: not m.case_info["ignore"], test_methods)
    test_cases = CaseConfig.Parser.apply_config(test_methods, None)
    tests_failed = []
    for case in test_cases:
        result = case.run()
        if not result:
            tests_failed.append(case)

    if tests_failed:
        print("The following tests have failed:")
        for case in tests_failed:
            print(" - " + case.test_method.__name__)
        raise SystemExit(1)

    print("Tests pass")
