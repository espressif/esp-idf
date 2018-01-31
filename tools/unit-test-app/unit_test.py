"""
Test script for unit test case.
"""

import re
import os
import sys
import time

import threading

# if we want to run test case outside `tiny-test-fw` folder,
# we need to insert tiny-test-fw path into sys path
test_fw_path = os.getenv("TEST_FW_PATH")
if test_fw_path and test_fw_path not in sys.path:
    sys.path.insert(0, test_fw_path)

import TinyFW
import IDF
import Utility
from DUT import ExpectTimeout
from IDF.IDFApp import UT


UT_APP_BOOT_UP_DONE = "Press ENTER to see the list of tests."
UT_TIMEOUT = 30

def format_test_case_config(test_case_data):
    """
    convert the test case data to unified format.
    We need to following info to run unit test cases:

    1. unit test app config
    2. test case name
    3. test case reset info

    the formatted case config is a dict, with ut app config as keys. The value is a list of test cases.
    Each test case is a dict with "name" and "reset" as keys. For example::

    case_config = {
        "default": [{"name": "restart from PRO CPU", "reset": "SW_CPU_RESET"}, {...}],
        "psram": [{"name": "restart from PRO CPU", "reset": "SW_CPU_RESET"}],
    }

    If config is not specified for test case, then

    :param test_case_data: string, list, or a dictionary list
    :return: formatted data
    """

    case_config = dict()

    def parse_case(one_case_data):
        """ parse and format one case """

        def process_reset_list(reset_list):
            # strip space and remove white space only items
            _output = list()
            for _r in reset_list:
                _data = _r.strip(" ")
                if _data:
                    _output.append(_data)
            return _output

        _case = dict()
        if isinstance(one_case_data, str):
            _temp = one_case_data.split(" [reset=")
            _case["name"] = _temp[0]
            try:
                _case["reset"] = process_reset_list(_temp[1][0:-1].split(","))
            except IndexError:
                _case["reset"] = list()
        elif isinstance(one_case_data, dict):
            _case = one_case_data.copy()
            assert "name" in _case
            if "reset" not in _case:
                _case["reset"] = list()
            else:
                if isinstance(_case["reset"], str):
                    _case["reset"] = process_reset_list(_case["reset"].split(","))
        else:
            raise TypeError("Not supported type during parsing unit test case")

        if "config" not in _case:
            _case["config"] = "default"

        return _case

    if not isinstance(test_case_data, list):
        test_case_data = [test_case_data]

    for case_data in test_case_data:
        parsed_case = parse_case(case_data)
        try:
            case_config[parsed_case["config"]].append(parsed_case)
        except KeyError:
            case_config[parsed_case["config"]] = [parsed_case]

    return case_config


@TinyFW.test_method(app=UT, dut=IDF.IDFDUT, chip="ESP32", module="unit_test",
                    execution_time=1, env_tag="UT_T1_1")
def test_unit_test_case(env, extra_data):
    """
    extra_data can be three types of value
    1. as string:
               1. "case_name"
               2. "case_name [reset=RESET_REASON]"
    2. as dict:
               1. with key like {"name": "Intr_alloc test, shared ints"}
               2. with key like {"name": "restart from PRO CPU", "reset": "SW_CPU_RESET", "config": "psram"}
    3. as list of string or dict:
               [case1, case2, case3, {"name": "restart from PRO CPU", "reset": "SW_CPU_RESET"}, ...]

    :param extra_data: the case name or case list or case dictionary
    :return: None
    """

    case_config = format_test_case_config(extra_data)

    # compile the patterns for expect only once
    reset_pattern = re.compile(r"(ets [\w]{3}\s+[\d]{1,2} [\d]{4} [\d]{2}:[\d]{2}:[\d]{2}[^()]*\([\w].*?\))")
    exception_pattern = re.compile(r"(Guru Meditation Error: Core\s+\d panic'ed \([\w].*?\))")
    abort_pattern = re.compile(r"(abort\(\) was called at PC 0x[a-eA-E\d]{8} on core \d)")
    finish_pattern = re.compile(r"1 Tests (\d) Failures (\d) Ignored")

    # we don't want stop on failed case (unless some special scenarios we can't handle)
    # this flag is used to log if any of the case failed during executing
    # Before exit test function this flag is used to log if the case fails
    failed_cases = []

    for ut_config in case_config:
        dut = env.get_dut("unit-test-app", app_path=ut_config)
        dut.start_app()

        for one_case in case_config[ut_config]:
            dut.reset()
            # esptool ``run`` cmd takes quite long time.
            # before reset finish, serial port is closed. therefore DUT could already bootup before serial port opened.
            # this could cause checking bootup print failed.
            # now we input cmd `-`, and check either bootup print or test history,
            # to determine if DUT is ready to test.
            dut.write("-", flush=False)
            dut.expect_any(UT_APP_BOOT_UP_DONE,
                           "0 Tests 0 Failures 0 Ignored")

            # run test case
            dut.write("\"{}\"".format(one_case["name"]))
            dut.expect("Running " + one_case["name"] + "...")

            exception_reset_list = []

            # we want to set this flag in callbacks (inner functions)
            # use list here so we can use append to set this flag
            test_finish = list()

            # expect callbacks
            def one_case_finish(result):
                """ one test finished, let expect loop break and log result """
                test_finish.append(True)
                if result:
                    Utility.console_log("Success: " + one_case["name"], color="green")
                else:
                    failed_cases.append(one_case["name"])
                    Utility.console_log("Failed: " + one_case["name"], color="red")

            def handle_exception_reset(data):
                """
                just append data to exception list.
                exception list will be checked in ``handle_reset_finish``, once reset finished.
                """
                exception_reset_list.append(data[0])

            def handle_test_finish(data):
                """ test finished without reset """
                # in this scenario reset should not happen
                assert not exception_reset_list
                if int(data[1]):
                    # case ignored
                    Utility.console_log("Ignored: " + one_case["name"], color="orange")
                one_case_finish(not int(data[0]))

            def handle_reset_finish(data):
                """ reset happened and reboot finished """
                assert exception_reset_list  # reboot but no exception/reset logged. should never happen
                result = False
                if len(one_case["reset"]) == len(exception_reset_list):
                    for i, exception in enumerate(exception_reset_list):
                        if one_case["reset"][i] not in exception:
                            break
                    else:
                        result = True
                if not result:
                    Utility.console_log("""Reset Check Failed: \r\n\tExpected: {}\r\n\tGet: {}"""
                                        .format(one_case["reset"], exception_reset_list),
                                        color="orange")
                one_case_finish(result)

            while not test_finish:
                try:
                    dut.expect_any((reset_pattern, handle_exception_reset),  # reset pattern
                                   (exception_pattern, handle_exception_reset),  # exception pattern
                                   (abort_pattern, handle_exception_reset),  # abort pattern
                                   (finish_pattern, handle_test_finish),  # test finish pattern
                                   (UT_APP_BOOT_UP_DONE, handle_reset_finish),  # reboot finish pattern
                                   timeout=UT_TIMEOUT)
                except ExpectTimeout:
                    Utility.console_log("Timeout in expect", color="orange")
                    one_case_finish(False)
                    break

    # raise exception if any case fails
    if failed_cases:
        Utility.console_log("Failed Cases:", color="red")
        for _case_name in failed_cases:
            Utility.console_log("\t" + _case_name, color="red")
        raise AssertionError("Unit Test Failed")


class Handler(threading.Thread):

    WAIT_SIGNAL_PATTERN = re.compile(r'Waiting for signal: \[(.+)\]!')
    SEND_SIGNAL_PATTERN = re.compile(r'Send signal: \[(.+)\]!')
    FINISH_PATTERN = re.compile(r"1 Tests (\d) Failures (\d) Ignored")

    def __init__(self, dut, sent_signal_list, lock, parent_case_name, child_case_index, timeout=30):
        self.dut = dut
        self.sent_signal_list = sent_signal_list
        self.lock = lock
        self.parent_case_name = parent_case_name
        self.child_case_name = ""
        self.child_case_index = child_case_index + 1
        self.finish = False
        self.result = False
        self.fail_name = None
        self.timeout = timeout
        threading.Thread.__init__(self, name="{} Handler".format(dut))

    def run(self):
        def get_child_case_name(data):
            self.child_case_name = data[0]
            time.sleep(1)
            self.dut.write(str(self.child_case_index))

        def one_device_case_finish(result):
            """ one test finished, let expect loop break and log result """
            self.finish = True
            self.result = result
            if not result:
                self.fail_name = self.child_case_name

        def device_wait_action(data):
            start_time = time.time()
            expected_signal = data[0]
            while 1:
                if time.time() > start_time + self.timeout:
                    Utility.console_log("Timeout in device for function: %s"%self.child_case_name, color="orange")
                    break
                with self.lock:
                    if expected_signal in self.sent_signal_list:
                        self.dut.write(" ")
                        self.sent_signal_list.remove(expected_signal)
                        break
                time.sleep(0.01)

        def device_send_action(data):
            with self.lock:
                self.sent_signal_list.append(data[0].encode('utf-8'))

        def handle_device_test_finish(data):
            """ test finished without reset """
            # in this scenario reset should not happen
            if int(data[1]):
                # case ignored
                Utility.console_log("Ignored: " + self.child_case_name, color="orange")
            one_device_case_finish(not int(data[0]))

        self.dut.reset()
        self.dut.write("-", flush=False)
        self.dut.expect_any(UT_APP_BOOT_UP_DONE, "0 Tests 0 Failures 0 Ignored")
        time.sleep(1)
        self.dut.write("\"{}\"".format(self.parent_case_name))
        self.dut.expect("Running " + self.parent_case_name + "...")

        while not self.finish:
            try:
                self.dut.expect_any((re.compile('\(' + str(self.child_case_index) + '\)\s"(\w+)"'), get_child_case_name),
                                    (self.WAIT_SIGNAL_PATTERN, device_wait_action),  # wait signal pattern
                                    (self.SEND_SIGNAL_PATTERN, device_send_action),  # send signal pattern
                                    (self.FINISH_PATTERN, handle_device_test_finish),  # test finish pattern
                                    timeout=UT_TIMEOUT)
            except ExpectTimeout:
                Utility.console_log("Timeout in expect", color="orange")
                one_device_case_finish(False)
                break


def get_case_info(one_case):
    parent_case = one_case["name"]
    child_case_num = one_case["child case num"]
    return parent_case, child_case_num


def get_dut(duts, env, name, ut_config):
    if name in duts:
        dut = duts[name]
    else:
        dut = env.get_dut(name, app_path=ut_config)
        duts[name] = dut
        dut.start_app()
    return dut


def case_run(duts, ut_config, env, one_case, failed_cases):
    lock = threading.RLock()
    threads = []
    send_signal_list = []
    failed_device = []
    result = True
    parent_case, case_num = get_case_info(one_case)
    for i in range(case_num):
        dut = get_dut(duts, env, "dut%d" % i, ut_config)
        threads.append(Handler(dut, send_signal_list, lock,
                               parent_case, i))
    for thread in threads:
        thread.setDaemon(True)
        thread.start()
    for thread in threads:
        thread.join()
        result = result and thread.result
        if not thread.result:
            failed_device.append(thread.fail_name)
    if result:
        Utility.console_log("Success: " + one_case["name"], color="green")
    else:
        failed_cases.append(one_case["name"])
        Utility.console_log("Failed: " + one_case["name"], color="red")


@TinyFW.test_method(app=UT, dut=IDF.IDFDUT, chip="ESP32", module="master_slave_test_case", execution_time=1,
                    env_tag="UT_T2_1")
def multiple_devices_case(env, extra_data):
    """
     extra_data can be two types of value
     1. as dict:
            e.g.
                {"name":  "gpio master/slave test example",
                "child case num": 2,
                "config": "release",
                "env_tag": "UT_T2_1"}
     2. as list dict:
            e.g.
               [{"name":  "gpio master/slave test example1",
                "child case num": 2,
                "config": "release",
                "env_tag": "UT_T2_1"},
               {"name":  "gpio master/slave test example2",
                "child case num": 2,
                "config": "release",
                "env_tag": "UT_T2_1"}]

    """
    failed_cases = []
    case_config = format_test_case_config(extra_data)
    DUTS = {}
    for ut_config in case_config:
        for one_case in case_config[ut_config]:
            case_run(DUTS, ut_config, env, one_case, failed_cases)

    if failed_cases:
        Utility.console_log("Failed Cases:", color="red")
        for _case_name in failed_cases:
            Utility.console_log("\t" + _case_name, color="red")
        raise AssertionError("Unit Test Failed")

if __name__ == '__main__':
    multiple_devices_case(extra_data={"name":  "gpio master/slave test example",
                                      "child case num": 2,
                                      "config": "release",
                                      "env_tag": "UT_T2_1"})



