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
from .IDFApp import IDFApp, Example, UT  # noqa: export all Apps for users
from .IDFDUT import IDFDUT  # noqa: export DUTs for users


def format_case_id(chip, case_name):
    return "{}.{}".format(chip, case_name)


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
        test_func.case_info["ID"] = format_case_id(chip, test_func.case_info["name"])
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
        test_func.case_info["ID"] = format_case_id(chip, test_func.case_info["name"])
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


def check_performance(item, value):
    """
    check if idf performance meet pass standard

    :param item: performance item name
    :param value: performance item value
    :raise: AssertionError: if check fails
    """
    ret = True
    standard_value = 0

    idf_path = IDFApp.get_sdk_path()
    performance_file = os.path.join(idf_path, "components", "idf_test", "include", "idf_performance.h")

    if os.path.exists(performance_file):
        with open(performance_file, "r") as f:
            data = f.read()
        match = re.search(r"#define\s+IDF_PERFORMANCE_(MIN|MAX)_{}\s+([\d.]+)".format(item.upper()), data)
        if match:
            op = match.group(1)
            standard_value = float(match.group(2))
            if op == "MAX":
                ret = value <= standard_value
            else:
                ret = value >= standard_value
    if not ret:
        raise AssertionError("[Performance] {} value is {}, doesn't meet pass standard {}"
                             .format(item, value, standard_value))
