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
import os
import sys
import argparse
import threading

import TinyFW
from Utility import SearchCases, CaseConfig


class Runner(threading.Thread):
    """
    :param test_case: test case file or folder
    :param case_config: case config file, allow to filter test cases and pass data to test case
    :param env_config_file: env config file
    """

    def __init__(self, test_case, case_config, env_config_file=None):
        super(Runner, self).__init__()
        self.setDaemon(True)
        test_methods = SearchCases.Search.search_test_cases(test_case)
        self.test_cases = CaseConfig.Parser.apply_config(test_methods, case_config)
        self.test_result = True
        if case_config:
            test_suite_name = os.path.splitext(os.path.basename(case_config))[0]
        else:
            test_suite_name = "TestRunner"
        TinyFW.set_default_config(env_config_file=env_config_file, test_suite_name=test_suite_name)

    def run(self):
        for case in self.test_cases:
            self.test_result = self.test_result and case.run()


if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument("test_case",
                        help="test case folder or file")
    parser.add_argument("--case_config", "-c", default=None,
                        help="case filter/config file")
    parser.add_argument("--env_config_file", "-e", default=None,
                        help="test env config file")
    args = parser.parse_args()

    runner = Runner(args.test_case, args.case_config, args.env_config_file)
    runner.start()

    while True:
        try:
            runner.join(1)
            if not runner.isAlive():
                break
        except KeyboardInterrupt:
            print("exit by Ctrl-C")
            break
    if not runner.test_result:
        sys.exit(1)
