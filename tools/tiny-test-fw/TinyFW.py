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

""" Interface for test cases. """
import sys
import os
import time
import traceback
import inspect
import functools

import xunitgen

import Env
import DUT
import App
import Utility


XUNIT_FILE_NAME = "XUNIT_RESULT.xml"
XUNIT_RECEIVER = xunitgen.EventReceiver()
XUNIT_DEFAULT_TEST_SUITE = "test-suite"


class DefaultEnvConfig(object):
    """
    default test configs. There're 3 places to set configs, priority is (high -> low):

    1. overwrite set by caller of test method
    2. values set by test_method decorator
    3. default env config get from this class
    """
    DEFAULT_CONFIG = {
        "app": App.BaseApp,
        "dut": DUT.BaseDUT,
        "env_tag": "default",
        "env_config_file": None,
        "test_suite_name": None,
    }

    @classmethod
    def set_default_config(cls, **kwargs):
        """
        :param kwargs: configs need to be updated
        :return: None
        """
        cls.DEFAULT_CONFIG.update(kwargs)

    @classmethod
    def get_default_config(cls):
        """
        :return: current default config
        """
        return cls.DEFAULT_CONFIG.copy()


set_default_config = DefaultEnvConfig.set_default_config
get_default_config = DefaultEnvConfig.get_default_config


class TestResult(object):
    TEST_RESULT = {
        "pass": [],
        "fail": [],
    }

    @classmethod
    def get_failed_cases(cls):
        """
        :return: failed test cases
        """
        return cls.TEST_RESULT["fail"]

    @classmethod
    def get_passed_cases(cls):
        """
        :return: passed test cases
        """
        return cls.TEST_RESULT["pass"]

    @classmethod
    def set_result(cls, result, case_name):
        """
        :param result: True or False
        :param case_name: test case name
        :return: None
        """
        cls.TEST_RESULT["pass" if result else "fail"].append(case_name)


get_failed_cases = TestResult.get_failed_cases
get_passed_cases = TestResult.get_passed_cases


MANDATORY_INFO = {
    "execution_time": 1,
    "env_tag": "default",
}


def test_method(**kwargs):
    """
    decorator for test case function.
    The following keyword arguments are pre-defined.
    Any other keyword arguments will be regarded as filter for the test case,
    able to access them by ``case_info`` attribute of test method.

    :keyword app: class for test app. see :doc:`App <App>` for details
    :keyword dut: class for current dut. see :doc:`DUT <DUT>` for details
    :keyword env_tag: name for test environment, used to select configs from config file
    :keyword env_config_file: test env config file. usually will not set this keyword when define case
    :keyword test_suite_name: test suite name, used for generating log folder name and adding xunit format test result.
                              usually will not set this keyword when define case
    """
    def test(test_func):
        # get test function file name
        frame = inspect.stack()
        test_func_file_name = frame[1][1]

        case_info = MANDATORY_INFO.copy()
        case_info["name"] = test_func.__name__
        case_info.update(kwargs)

        # create env instance
        env_config = DefaultEnvConfig.get_default_config()
        for key in kwargs:
            if key in env_config:
                env_config[key] = kwargs[key]

        @functools.wraps(test_func)
        def handle_test(extra_data=None, **overwrite):
            """
            create env, run test and record test results

            :param extra_data: extra data that runner or main passed to test case
            :param overwrite: args that runner or main want to overwrite
            :return: None
            """
            env_config.update(overwrite)
            env_inst = Env.Env(**env_config)
            # prepare for xunit test results
            xunit_file = os.path.join(env_inst.app_cls.get_log_folder(env_config["test_suite_name"]),
                                      XUNIT_FILE_NAME)
            XUNIT_RECEIVER.begin_case(test_func.__name__, time.time(), test_func_file_name)
            try:
                Utility.console_log("starting running test: " + test_func.__name__, color="green")
                # execute test function
                test_func(env_inst, extra_data)
                # if finish without exception, test result is True
                result = True
            except Exception as e:
                # handle all the exceptions here
                traceback.print_exc()
                result = False
                # log failure
                XUNIT_RECEIVER.failure(str(e), test_func_file_name)
            finally:
                # do close all DUTs
                env_inst.close()
            # end case and output result
            XUNIT_RECEIVER.end_case(test_func.__name__, time.time())
            with open(xunit_file, "ab+") as f:
                f.write(xunitgen.toxml(XUNIT_RECEIVER.results(),
                                       XUNIT_DEFAULT_TEST_SUITE))

            if result:
                Utility.console_log("Test Succeed: " + test_func.__name__, color="green")
            else:
                Utility.console_log(("Test Fail: " + test_func.__name__), color="red")
            TestResult.set_result(result, test_func.__name__)
            return result

        handle_test.case_info = case_info
        handle_test.test_method = True
        return handle_test
    return test
