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

"""
Command line interface to run test cases from a given path.

* search and run test cases of a given path
* config file which support to filter test cases and passing data to test case

Use ``python Runner.py test_case_path -c config_file -e env_config_file`` to run test cases.

"""
import argparse
import os
import sys
import threading
from fnmatch import fnmatch

from tiny_test_fw.TinyFW import JunitReport, set_default_config
from tiny_test_fw.Utility import CaseConfig, SearchCases, console_log


class Runner(threading.Thread):
    """
    :param test_case: test case file or folder
    :param case_config: case config file, allow to filter test cases and pass data to test case
    :param env_config_file: env config file
    """

    def __init__(self, test_case, case_config, env_config_file=None, known_failure_cases_file=None):
        super(Runner, self).__init__()
        self.setDaemon(True)
        if case_config:
            test_suite_name = os.path.splitext(os.path.basename(case_config))[0]
        else:
            test_suite_name = "TestRunner"
        set_default_config(env_config_file=env_config_file, test_suite_name=test_suite_name)
        test_methods = SearchCases.Search.search_test_cases(test_case)
        self.test_cases = CaseConfig.Parser.apply_config(test_methods, case_config)
        self.known_failure_cases = self._get_config_cases(known_failure_cases_file)

    @staticmethod
    def _get_config_cases(config_file):
        res = set()
        if not config_file or not os.path.isfile(config_file):
            return res

        for line in open(config_file).readlines():
            if not line:
                continue
            if not line.strip():
                continue
            without_comments = line.split("#")[0].strip()
            if without_comments:
                res.add(without_comments)
        return res

    def run(self):
        for case in self.test_cases:
            case.run()

    @staticmethod
    def is_known_issue(tc_name, known_cases):
        for case in known_cases:
            if tc_name == case:
                return True
            if fnmatch(tc_name, case):
                return True
        return False

    def get_test_result(self):
        _res = True
        console_log("Test Results:")
        for tc in JunitReport.JUNIT_TEST_SUITE.test_cases:
            if tc.failures:
                if self.is_known_issue(tc.name, self.known_failure_cases):
                    console_log("  Known Failure: " + tc.name, color="orange")
                else:
                    console_log("  Test Fail: " + tc.name, color="red")
                    _res = False
            else:
                console_log("  Test Succeed: " + tc.name, color="green")

        return _res


if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("test_case",
                        help="test case folder or file")
    parser.add_argument("--case_config", "-c", default=None,
                        help="case filter/config file")
    parser.add_argument("--env_config_file", "-e", default=None,
                        help="test env config file")
    parser.add_argument("--known_failure_cases_file", default=None,
                        help="known failure cases file")
    args = parser.parse_args()

    runner = Runner(args.test_case, args.case_config, args.env_config_file, args.known_failure_cases_file)
    runner.start()

    while True:
        try:
            runner.join(1)
            if not runner.is_alive():
                break
        except KeyboardInterrupt:
            print("exit by Ctrl-C")
            break
    if not runner.get_test_result():
        sys.exit(1)
