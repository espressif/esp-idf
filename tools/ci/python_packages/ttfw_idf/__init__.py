# Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import os
import re

from tiny_test_fw import TinyFW, Utility
from .IDFApp import IDFApp, Example, LoadableElfTestApp, UT, TestApp  # noqa: export all Apps for users
from .IDFDUT import IDFDUT, ESP32DUT, ESP32S2DUT, ESP8266DUT, ESP32QEMUDUT  # noqa: export DUTs for users
from .DebugUtils import OCDProcess, GDBProcess, TelnetProcess, CustomProcess  # noqa: export DebugUtils for users


def idf_example_test(app=Example, dut=IDFDUT, chip="ESP32", module="examples", execution_time=1,
                     level="example", erase_nvs=True, config_name=None, **kwargs):
    """
    decorator for testing idf examples (with default values for some keyword args).

    :param app: test application class
    :param dut: dut class
    :param chip: chip supported, string or tuple
    :param module: module, string
    :param execution_time: execution time in minutes, int
    :param level: test level, could be used to filter test cases, string
    :param erase_nvs: if need to erase_nvs in DUT.start_app()
    :param config_name: if specified, name of the app configuration
    :param kwargs: other keyword args
    :return: test method
    """
    try:
        # try to config the default behavior of erase nvs
        dut.ERASE_NVS = erase_nvs
    except AttributeError:
        pass

    original_method = TinyFW.test_method(app=app, dut=dut, chip=chip, module=module,
                                         execution_time=execution_time, level=level, **kwargs)

    def test(func):
        test_func = original_method(func)
        return test_func

    return test


def idf_unit_test(app=UT, dut=IDFDUT, chip="ESP32", module="unit-test", execution_time=1,
                  level="unit", erase_nvs=True, **kwargs):
    """
    decorator for testing idf unit tests (with default values for some keyword args).

    :param app: test application class
    :param dut: dut class
    :param chip: chip supported, string or tuple
    :param module: module, string
    :param execution_time: execution time in minutes, int
    :param level: test level, could be used to filter test cases, string
    :param erase_nvs: if need to erase_nvs in DUT.start_app()
    :param kwargs: other keyword args
    :return: test method
    """
    try:
        # try to config the default behavior of erase nvs
        dut.ERASE_NVS = erase_nvs
    except AttributeError:
        pass

    original_method = TinyFW.test_method(app=app, dut=dut, chip=chip, module=module,
                                         execution_time=execution_time, level=level, **kwargs)

    def test(func):
        test_func = original_method(func)
        return test_func

    return test


def idf_custom_test(app=TestApp, dut=IDFDUT, chip="ESP32", module="misc", execution_time=1,
                    level="integration", erase_nvs=True, config_name=None, group="test-apps", **kwargs):

    """
    decorator for idf custom tests (with default values for some keyword args).

    :param app: test application class
    :param dut: dut class
    :param chip: chip supported, string or tuple
    :param module: module, string
    :param execution_time: execution time in minutes, int
    :param level: test level, could be used to filter test cases, string
    :param erase_nvs: if need to erase_nvs in DUT.start_app()
    :param config_name: if specified, name of the app configuration
    :param group: identifier to group custom tests (unused for now, defaults to "test-apps")
    :param kwargs: other keyword args
    :return: test method
    """
    try:
        # try to config the default behavior of erase nvs
        dut.ERASE_NVS = erase_nvs
    except AttributeError:
        pass

    original_method = TinyFW.test_method(app=app, dut=dut, chip=chip, module=module,
                                         execution_time=execution_time, level=level, **kwargs)

    def test(func):
        test_func = original_method(func)
        return test_func

    return test


def log_performance(item, value):
    """
    do print performance with pre-defined format to console

    :param item: performance item name
    :param value: performance value
    """
    performance_msg = "[Performance][{}]: {}".format(item, value)
    Utility.console_log(performance_msg, "orange")
    # update to junit test report
    current_junit_case = TinyFW.JunitReport.get_current_test_case()
    current_junit_case.stdout += performance_msg + "\r\n"


def check_performance(item, value, target):
    """
    check if idf performance meet pass standard

    :param item: performance item name
    :param value: performance item value
    :param target: target chip
    :raise: AssertionError: if check fails
    """

    def _find_perf_item(path):
        with open(path, 'r') as f:
            data = f.read()
        match = re.search(r'#define\s+IDF_PERFORMANCE_(MIN|MAX)_{}\s+([\d.]+)'.format(item.upper()), data)
        return match.group(1), float(match.group(2))

    def _check_perf(op, standard_value):
        if op == 'MAX':
            ret = value <= standard_value
        else:
            ret = value >= standard_value
        if not ret:
            raise AssertionError("[Performance] {} value is {}, doesn't meet pass standard {}"
                                 .format(item, value, standard_value))

    path_prefix = os.path.join(IDFApp.get_sdk_path(), 'components', 'idf_test', 'include')
    performance_files = (os.path.join(path_prefix, target, 'idf_performance_target.h'),
                         os.path.join(path_prefix, 'idf_performance.h'))

    for performance_file in performance_files:
        try:
            op, value = _find_perf_item(performance_file)
        except (IOError, AttributeError):
            # performance file doesn't exist or match is not found in it
            continue

        _check_perf(op, value)
        # if no exception was thrown then the performance is met and no need to continue
        break
