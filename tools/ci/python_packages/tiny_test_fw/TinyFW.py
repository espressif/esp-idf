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
import functools
import os
import socket
import time
from datetime import datetime

import junit_xml

from . import DUT, App, Env, Utility
from .Utility import format_case_id


class TestCaseFailed(AssertionError):
    def __init__(self, *cases):
        """
        Raise this exception if one or more test cases fail in a 'normal' way (ie the test runs but fails, no unexpected exceptions)

        This will avoid dumping the Python stack trace, because the assumption is the junit error info and full job log already has
        enough information for a developer to debug.

        'cases' argument is the names of one or more test cases
        """
        message = 'Test case{} failed: {}'.format('s' if len(cases) > 1 else '', ', '.join(str(c) for c in cases))
        super(TestCaseFailed, self).__init__(self, message)


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


MANDATORY_INFO = {
    "execution_time": 1,
    "env_tag": "default",
    "category": "function",
    "ignore": False,
}


class JunitReport(object):
    # wrapper for junit test report
    # TODO: JunitReport methods are not thread safe (although not likely to be used this way).

    JUNIT_FILE_NAME = "XUNIT_RESULT.xml"
    JUNIT_DEFAULT_TEST_SUITE = "test-suite"
    JUNIT_TEST_SUITE = junit_xml.TestSuite(JUNIT_DEFAULT_TEST_SUITE,
                                           hostname=socket.gethostname(),
                                           timestamp=datetime.utcnow().isoformat())
    JUNIT_CURRENT_TEST_CASE = None
    _TEST_CASE_CREATED_TS = 0

    @classmethod
    def output_report(cls, junit_file_path):
        """ Output current test result to file. """
        with open(os.path.join(junit_file_path, cls.JUNIT_FILE_NAME), 'w') as f:
            junit_xml.to_xml_report_file(f, [cls.JUNIT_TEST_SUITE], prettyprint=False)

    @classmethod
    def get_current_test_case(cls):
        """
        By default, the test framework will handle junit test report automatically.
        While some test case might want to update some info to test report.
        They can use this method to get current test case created by test framework.

        :return: current junit test case instance created by ``JunitTestReport.create_test_case``
        """
        return cls.JUNIT_CURRENT_TEST_CASE

    @classmethod
    def test_case_finish(cls, test_case):
        """
        Append the test case to test suite so it can be output to file.
        Execution time will be automatically updated (compared to ``create_test_case``).
        """
        test_case.elapsed_sec = time.time() - cls._TEST_CASE_CREATED_TS
        cls.JUNIT_TEST_SUITE.test_cases.append(test_case)

    @classmethod
    def create_test_case(cls, name):
        """
        Extend ``junit_xml.TestCase`` with:

        1. save create test case so it can be get by ``get_current_test_case``
        2. log create timestamp, so ``elapsed_sec`` can be auto updated in ``test_case_finish``.

        :param name: test case name
        :return: instance of ``junit_xml.TestCase``
        """
        # set stdout to empty string, so we can always append string to stdout.
        # It won't affect output logic. If stdout is empty, it won't be put to report.
        test_case = junit_xml.TestCase(name, stdout="")
        cls.JUNIT_CURRENT_TEST_CASE = test_case
        cls._TEST_CASE_CREATED_TS = time.time()
        return test_case

    @classmethod
    def update_performance(cls, performance_items):
        """
        Update performance results to ``stdout`` of current test case.

        :param performance_items: a list of performance items. each performance item is a key-value pair.
        """
        assert cls.JUNIT_CURRENT_TEST_CASE

        for item in performance_items:
            cls.JUNIT_CURRENT_TEST_CASE.stdout += "[{}]: {}\n".format(item[0], item[1])


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
    :keyword junit_report_by_case: By default the test fw will handle junit report generation.
                                   In some cases, one test function might test many test cases.
                                   If this flag is set, test case can update junit report by its own.
    """
    def test(test_func):

        case_info = MANDATORY_INFO.copy()
        case_info["name"] = case_info["ID"] = test_func.__name__
        case_info["junit_report_by_case"] = False
        case_info.update(kwargs)

        @functools.wraps(test_func)
        def handle_test(extra_data=None, **overwrite):
            """
            create env, run test and record test results

            :param extra_data: extra data that runner or main passed to test case
            :param overwrite: args that runner or main want to overwrite
            :return: None
            """
            # create env instance
            env_config = DefaultEnvConfig.get_default_config()
            for key in kwargs:
                if key in env_config:
                    env_config[key] = kwargs[key]

            env_config.update(overwrite)
            env_inst = Env.Env(**env_config)

            # prepare for xunit test results
            junit_file_path = env_inst.app_cls.get_log_folder(env_config['test_suite_name'])
            junit_test_case = JunitReport.create_test_case(format_case_id(case_info['ID'],
                                                                          target=case_info['chip'].lower()))
            result = False
            try:
                Utility.console_log("starting running test: " + test_func.__name__, color="green")
                # execute test function
                test_func(env_inst, extra_data)
                # if finish without exception, test result is True
                result = True
            except TestCaseFailed as e:
                junit_test_case.add_failure_info(str(e))
            except Exception as e:
                Utility.handle_unexpected_exception(junit_test_case, e)
            finally:
                # do close all DUTs, if result is False then print DUT debug info
                close_errors = env_inst.close(dut_debug=(not result))
                # We have a hook in DUT close, allow DUT to raise error to fail test case.
                # For example, we don't allow DUT exception (reset) during test execution.
                # We don't want to implement in exception detection in test function logic,
                # as we need to add it to every test case.
                # We can implement it in DUT receive thread,
                # and raise exception in DUT close to fail test case if reset detected.
                if close_errors:
                    for error in close_errors:
                        junit_test_case.add_failure_info(str(error))
                    result = False
                if not case_info["junit_report_by_case"]:
                    JunitReport.test_case_finish(junit_test_case)

            # end case and output result
            JunitReport.output_report(junit_file_path)

            if result:
                Utility.console_log("Test Succeed: " + test_func.__name__, color="green")
            else:
                Utility.console_log(("Test Fail: " + test_func.__name__), color="red")
            return result

        handle_test.case_info = case_info
        handle_test.test_method = True
        return handle_test
    return test
