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

import yaml

test_fw_path = os.getenv("TEST_FW_PATH")
if test_fw_path:
    sys.path.insert(0, test_fw_path)

from Utility import CaseConfig, SearchCases, GitlabCIJob


class Group(object):

    MAX_EXECUTION_TIME = 30
    MAX_CASE = 15
    SORT_KEYS = ["env_tag"]

    def __init__(self, case):
        self.execution_time = 0
        self.case_list = [case]
        self.filters = dict(zip(self.SORT_KEYS, [case.case_info[x] for x in self.SORT_KEYS]))

    def accept_new_case(self):
        """
        check if allowed to add any case to this group

        :return: True or False
        """
        max_time = (sum([x.case_info["execution_time"] for x in self.case_list]) < self.MAX_EXECUTION_TIME)
        max_case = (len(self.case_list) < self.MAX_CASE)
        return max_time and max_case

    def add_case(self, case):
        """
        add case to current group

        :param case: test case
        :return: True if add succeed, else False
        """
        added = False
        if self.accept_new_case():
            for key in self.filters:
                if case.case_info[key] != self.filters[key]:
                    break
            else:
                self.case_list.append(case)
                added = True
        return added

    def output(self):
        """
        output data for job configs

        :return: {"Filter": case filter, "CaseConfig": list of case configs for cases in this group}
        """
        output_data = {
            "Filter": self.filters,
            "CaseConfig": [{"name": x.case_info["name"]} for x in self.case_list],
        }
        return output_data


class AssignTest(object):
    """
    Auto assign tests to CI jobs.

    :param test_case: path of test case file(s)
    :param ci_config_file: path of ``.gitlab-ci.yml``
    """

    CI_TEST_JOB_PATTERN = re.compile(r"^example_test_.+")

    def __init__(self, test_case, ci_config_file):
        self.test_cases = self._search_cases(test_case)
        self.jobs = self._parse_gitlab_ci_config(ci_config_file)

    def _parse_gitlab_ci_config(self, ci_config_file):

        with open(ci_config_file, "r") as f:
            ci_config = yaml.load(f)

        job_list = list()
        for job_name in ci_config:
            if self.CI_TEST_JOB_PATTERN.search(job_name) is not None:
                job_list.append(GitlabCIJob.Job(ci_config[job_name], job_name))
        return job_list

    @staticmethod
    def _search_cases(test_case, case_filter=None):
        """
        :param test_case: path contains test case folder
        :param case_filter: filter for test cases
        :return: filtered test case list
        """
        test_methods = SearchCases.Search.search_test_cases(test_case)
        return CaseConfig.filter_test_cases(test_methods, case_filter if case_filter else dict())

    def _group_cases(self):
        """
        separate all cases into groups according group rules. each group will be executed by one CI job.

        :return: test case groups.
        """
        groups = []
        for case in self.test_cases:
            for group in groups:
                # add to current group
                if group.add_case(case):
                    break
            else:
                # create new group
                groups.append(Group(case))
        return groups

    def assign_cases(self):
        """
        separate test cases to groups and assign test cases to CI jobs.

        :raise AssertError: if failed to assign any case to CI job.
        :return: None
        """
        failed_to_assign = []
        test_groups = self._group_cases()
        for group in test_groups:
            for job in self.jobs:
                if job.match_group(group):
                    job.assign_group(group)
                    break
            else:
                failed_to_assign.append(group)
        assert not failed_to_assign

    def output_configs(self, output_path):
        """

        :param output_path: path to output config files for each CI job
        :return: None
        """
        if not os.path.exists(output_path):
            os.makedirs(output_path)
        for job in self.jobs:
            job.output_config(output_path)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("test_case",
                        help="test case folder or file")
    parser.add_argument("ci_config_file",
                        help="gitlab ci config file")
    parser.add_argument("output_path",
                        help="output path of config files")
    args = parser.parse_args()

    assign_test = AssignTest(args.test_case, args.ci_config_file)
    assign_test.assign_cases()
    assign_test.output_configs(args.output_path)
