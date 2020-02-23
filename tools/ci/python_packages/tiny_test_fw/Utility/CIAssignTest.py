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
try:
    from yaml import CLoader as Loader
except ImportError:
    from yaml import Loader as Loader

from . import (CaseConfig, SearchCases, GitlabCIJob, console_log)


class Group(object):

    MAX_EXECUTION_TIME = 30
    MAX_CASE = 15
    SORT_KEYS = ["env_tag"]
    # Matching CI job rules could be different from the way we want to group test cases.
    # For example, when assign unit test cases, different test cases need to use different test functions.
    # We need to put them into different groups.
    # But these groups can be assigned to jobs with same tags, as they use the same test environment.
    CI_JOB_MATCH_KEYS = SORT_KEYS

    def __init__(self, case):
        self.execution_time = 0
        self.case_list = [case]
        self.filters = dict(zip(self.SORT_KEYS, [self._get_case_attr(case, x) for x in self.SORT_KEYS]))
        # we use ci_job_match_keys to match CI job tags. It's a set of required tags.
        self.ci_job_match_keys = set([self._get_case_attr(case, x) for x in self.CI_JOB_MATCH_KEYS])

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

    def add_extra_case(self, case):
        """
        By default (``add_case`` method), cases will only be added when have equal values of all filters with group.
        But in some cases, we also want to add cases which are not best fit.
        For example, one group has can run cases require (A, B). It can also accept cases require (A, ) and (B, ).
        When assign failed by best fit, we will use this method to try if we can assign all failed cases.

        If subclass want to retry, they need to overwrite this method.
        Logic can be applied to handle such scenario could be different for different cases.

        :return: True if accepted else False
        """
        pass

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
    # by default we only run function in CI, as other tests could take long time
    DEFAULT_FILTER = {
        "category": "function",
        "ignore": False,
    }

    def __init__(self, test_case_path, ci_config_file, case_group=Group):
        self.test_case_path = test_case_path
        self.test_case_file_pattern = None
        self.test_cases = []
        self.jobs = self._parse_gitlab_ci_config(ci_config_file)
        self.case_group = case_group

    @staticmethod
    def _handle_parallel_attribute(job_name, job):
        jobs_out = []
        try:
            for i in range(job["parallel"]):
                jobs_out.append(GitlabCIJob.Job(job, job_name + "_{}".format(i + 1)))
        except KeyError:
            # Gitlab don't allow to set parallel to 1.
            # to make test job name same ($CI_JOB_NAME_$CI_NODE_INDEX),
            # we append "_" to jobs don't have parallel attribute
            jobs_out.append(GitlabCIJob.Job(job, job_name + "_"))
        return jobs_out

    def _parse_gitlab_ci_config(self, ci_config_file):

        with open(ci_config_file, "r") as f:
            ci_config = yaml.load(f, Loader=Loader)

        job_list = list()
        for job_name in ci_config:
            if self.CI_TEST_JOB_PATTERN.search(job_name) is not None:
                job_list.extend(self._handle_parallel_attribute(job_name, ci_config[job_name]))
        job_list.sort(key=lambda x: x["name"])
        return job_list

    def _search_cases(self, test_case_path, case_filter=None, test_case_file_pattern=None):
        """
        :param test_case_path: path contains test case folder
        :param case_filter: filter for test cases. the filter to use is default filter updated with case_filter param.
        :return: filtered test case list
        """
        _case_filter = self.DEFAULT_FILTER.copy()
        if case_filter:
            _case_filter.update(case_filter)
        test_methods = SearchCases.Search.search_test_cases(test_case_path, test_case_file_pattern)
        return CaseConfig.filter_test_cases(test_methods, _case_filter)

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

    def _assign_failed_cases(self, assigned_groups, failed_groups):
        """ try to assign failed cases to already assigned test groups """
        still_failed_groups = []
        failed_cases = []
        for group in failed_groups:
            failed_cases.extend(group.case_list)
        for case in failed_cases:
            # first try to assign to already assigned groups
            for group in assigned_groups:
                if group.add_extra_case(case):
                    break
            else:
                # if failed, group the failed cases
                for group in still_failed_groups:
                    if group.add_case(case):
                        break
                else:
                    still_failed_groups.append(self.case_group(case))
        return still_failed_groups

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

    def _apply_bot_test_count(self):
        """
        Bot could also pass test count.
        If filtered cases need to be tested for several times, then we do duplicate them here.
        """
        test_count = os.getenv("BOT_TEST_COUNT")
        if test_count:
            test_count = int(test_count)
            self.test_cases *= test_count

    @staticmethod
    def _count_groups_by_keys(test_groups):
        """
        Count the number of test groups by job match keys.
        It's an important information to update CI config file.
        """
        group_count = dict()
        for group in test_groups:
            key = ",".join(group.ci_job_match_keys)
            try:
                group_count[key] += 1
            except KeyError:
                group_count[key] = 1
        return group_count

    def assign_cases(self):
        """
        separate test cases to groups and assign test cases to CI jobs.

        :raise AssertError: if failed to assign any case to CI job.
        :return: None
        """
        failed_to_assign = []
        assigned_groups = []
        case_filter = self._apply_bot_filter()
        self.test_cases = self._search_cases(self.test_case_path, case_filter, self.test_case_file_pattern)
        self._apply_bot_test_count()
        test_groups = self._group_cases()

        for group in test_groups:
            for job in self.jobs:
                if job.match_group(group):
                    job.assign_group(group)
                    assigned_groups.append(group)
                    break
            else:
                failed_to_assign.append(group)

        if failed_to_assign:
            failed_to_assign = self._assign_failed_cases(assigned_groups, failed_to_assign)

        # print debug info
        # total requirement of current pipeline
        required_group_count = self._count_groups_by_keys(test_groups)
        console_log("Required job count by tags:")
        for tags in required_group_count:
            console_log("\t{}: {}".format(tags, required_group_count[tags]))

        # number of unused jobs
        not_used_jobs = [job for job in self.jobs if "case group" not in job]
        if not_used_jobs:
            console_log("{} jobs not used. Please check if you define too much jobs".format(len(not_used_jobs)), "O")
        for job in not_used_jobs:
            console_log("\t{}".format(job["name"]), "O")

        # failures
        if failed_to_assign:
            console_log("Too many test cases vs jobs to run. "
                        "Please increase parallel count in tools/ci/config/target-test.yml "
                        "for jobs with specific tags:", "R")
            failed_group_count = self._count_groups_by_keys(failed_to_assign)
            for tags in failed_group_count:
                console_log("\t{}: {}".format(tags, failed_group_count[tags]), "R")
            raise RuntimeError("Failed to assign test case to CI jobs")

    def output_configs(self, output_path):
        """
        :param output_path: path to output config files for each CI job
        :return: None
        """
        if not os.path.exists(output_path):
            os.makedirs(output_path)
        for job in self.jobs:
            job.output_config(output_path)
