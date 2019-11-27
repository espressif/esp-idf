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
Command line tool to assign example tests to CI test jobs.
"""

# TODO: Need to handle running examples on different chips
import os
import sys
import re
import argparse

import gitlab_api
from tiny_test_fw.Utility import CIAssignTest


class ExampleGroup(CIAssignTest.Group):
    SORT_KEYS = CI_JOB_MATCH_KEYS = ["env_tag", "chip"]


class CIExampleAssignTest(CIAssignTest.AssignTest):
    CI_TEST_JOB_PATTERN = re.compile(r"^example_test_.+")


class ArtifactFile(object):
    def __init__(self, project_id, job_name, artifact_file_path):
        self.gitlab_api = gitlab_api.Gitlab(project_id)

    def process(self):
        pass

    def output(self):
        pass


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("test_case",
                        help="test case folder or file")
    parser.add_argument("ci_config_file",
                        help="gitlab ci config file")
    parser.add_argument("output_path",
                        help="output path of config files")
    args = parser.parse_args()

    assign_test = CIExampleAssignTest(args.test_case, args.ci_config_file, case_group=ExampleGroup)
    assign_test.assign_cases()
    assign_test.output_configs(args.output_path)
