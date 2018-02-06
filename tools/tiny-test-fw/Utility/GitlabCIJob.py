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

import yaml


class Job(dict):
    """
    Gitlab CI job

    :param job: job data loaded from .gitlab-ci.yml
    :param job_name: job name
    """
    def __init__(self, job, job_name):
        super(Job, self).__init__(job)
        self["name"] = job_name

    def match_group(self, group):
        """
        Match group by tags of job.
        All filters values of group should be included in tags.

        :param group: case group to match
        :return: True or False
        """
        match_result = False
        for _ in range(1):
            if "case group" in self:
                # this job is already assigned
                break
            for value in group.filters.values():
                if value not in self["tags"]:
                    break
            else:
                continue
            break
        else:
            match_result = True
        return match_result

    def assign_group(self, group):
        """
        assign a case group to a test job.

        :param group: the case group to assign
        """
        self["case group"] = group

    def output_config(self, file_path):
        """
        output test config to the given path.
        file name will be job_name.yml

        :param file_path: output file path
        :return: None
        """
        file_name = os.path.join(file_path, self["name"] + ".yml")
        if "case group" in self:
            with open(file_name, "w") as f:
                yaml.dump(self["case group"].output(), f, default_flow_style=False)
