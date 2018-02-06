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
Common logic to assign test cases to CI jobs.

Some background knowledge about Gitlab CI and use flow in esp-idf:

* Gitlab CI jobs are static in ``.gitlab-ci.yml``. We can't dynamically create test jobs
* For test job running on DUT, we use ``tags`` to select runners with different test environment
* We have ``assign_test`` stage, will collect cases, and then assign them to correct test jobs
* ``assign_test`` will fail if failed to assign any cases
* with ``assign_test``, we can:
    * dynamically filter test case we want to test
    * alert user if they forget to add CI jobs and guide how to add test jobs
* the last step of ``assign_test`` is to output config files, then test jobs will run these cases

The Basic logic to assign test cases is as follow:

1. do search all the cases
2. do filter case (if filter is specified by @bot)
3. put cases to different groups according to rule of ``Group``
    * try to put them in existed groups
    * if failed then create a new group and add this case
4. parse and filter the test jobs from CI config file
5. try to assign all groups to jobs according to tags
6. output config files for jobs

"""

import os
import re
import json

import yaml

from Utility import (CaseConfig, SearchCases, GitlabCIJob)


class Group(object):

    MAX_EXECUTION_TIME = 30
    MAX_CASE = 15
    SORT_KEYS = ["env_tag"]

    def __init__(self, case):
        self.execution_time = 0
        self.case_list = [case]
        self.filters = dict(zip(self.SORT_KEYS, [self._get_case_attr(case, x) for x in self.SORT_KEYS]))

    @staticmethod
    def _get_case_attr(case, attr):
        # we might use different type for case (dict or test_func)
        # this method will do get attribute form cases
        return case.case_info[attr]

    def accept_new_case(self):
        """
        check if allowed to add any case to this group

        :return: True or False
        """
        max_time = (sum([self._get_case_attr(x, "execution_time") for x in self.case_list])
                    < self.MAX_EXECUTION_TIME)
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
                if self._get_case_attr(case, key) != self.filters[key]:
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
            "CaseConfig": [{"name": self._get_case_attr(x, "name")} for x in self.case_list],
        }
        return output_data


class AssignTest(object):
    """
    Auto assign tests to CI jobs.

    :param test_case_path: path of test case file(s)
    :param ci_config_file: path of ``.gitlab-ci.yml``
    """
    # subclass need to rewrite CI test job pattern, to filter all test jobs
    CI_TEST_JOB_PATTERN = re.compile(r"^test_.+")

    def __init__(self, test_case_path, ci_config_file, case_group=Group):
        self.test_case_path = test_case_path
        self.test_cases = []
        self.jobs = self._parse_gitlab_ci_config(ci_config_file)
        self.case_group = case_group

    def _parse_gitlab_ci_config(self, ci_config_file):

        with open(ci_config_file, "r") as f:
            ci_config = yaml.load(f)

        job_list = list()
        for job_name in ci_config:
            if self.CI_TEST_JOB_PATTERN.search(job_name) is not None:
                job_list.append(GitlabCIJob.Job(ci_config[job_name], job_name))
        return job_list

    @staticmethod
    def _search_cases(test_case_path, case_filter=None):
        """
        :param test_case_path: path contains test case folder
        :param case_filter: filter for test cases
        :return: filtered test case list
        """
        test_methods = SearchCases.Search.search_test_cases(test_case_path)
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
                groups.append(self.case_group(case))
        return groups

    @staticmethod
    def _apply_bot_filter():
        """
        we support customize CI test with bot.
        here we process from and return the filter which ``_search_cases`` accepts.

        :return: filter for search test cases
        """
        bot_filter = os.getenv("BOT_CASE_FILTER")
        if bot_filter:
            bot_filter = json.loads(bot_filter)
        else:
            bot_filter = dict()
        return bot_filter

    def assign_cases(self):
        """
        separate test cases to groups and assign test cases to CI jobs.

        :raise AssertError: if failed to assign any case to CI job.
        :return: None
        """
        failed_to_assign = []
        case_filter = self._apply_bot_filter()
        self.test_cases = self._search_cases(self.test_case_path, case_filter)
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
