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
import re
import argparse
import json

import gitlab_api
from tiny_test_fw.Utility import CIAssignTest

IDF_PATH_FROM_ENV = os.getenv("IDF_PATH")


class ExampleGroup(CIAssignTest.Group):
    SORT_KEYS = CI_JOB_MATCH_KEYS = ["env_tag", "chip"]
    BUILD_LOCAL_DIR = "build_examples"
    BUILD_JOB_NAMES = ["build_examples_cmake_esp32", "build_examples_cmake_esp32s2"]


class TestAppsGroup(ExampleGroup):
    BUILD_LOCAL_DIR = "build_test_apps"
    BUILD_JOB_NAMES = ["build_test_apps_esp32", "build_test_apps_esp32s2"]


class CIExampleAssignTest(CIAssignTest.AssignTest):
    CI_TEST_JOB_PATTERN = re.compile(r"^example_test_.+")


def get_artifact_index_file(case_group=ExampleGroup):
    if IDF_PATH_FROM_ENV:
        artifact_index_file = os.path.join(IDF_PATH_FROM_ENV,
                                           case_group.BUILD_LOCAL_DIR, "artifact_index.json")
    else:
        artifact_index_file = "artifact_index.json"
    return artifact_index_file


def create_artifact_index_file(project_id=None, pipeline_id=None, case_group=ExampleGroup):
    if project_id is None:
        project_id = os.getenv("CI_PROJECT_ID")
    if pipeline_id is None:
        pipeline_id = os.getenv("CI_PIPELINE_ID")
    gitlab_inst = gitlab_api.Gitlab(project_id)
    artifact_index_list = []

    def format_build_log_path():
        parallel = job_info["parallel_num"]    # Could be None if "parallel_num" not defined for the job
        return "{}/list_job_{}.json".format(case_group.BUILD_LOCAL_DIR, parallel or 1)

    for build_job_name in case_group.BUILD_JOB_NAMES:
        job_info_list = gitlab_inst.find_job_id(build_job_name, pipeline_id=pipeline_id)
        for job_info in job_info_list:
            raw_data = gitlab_inst.download_artifact(job_info["id"], [format_build_log_path()])[0]
            build_info_list = [json.loads(line) for line in raw_data.decode().splitlines()]
            for build_info in build_info_list:
                build_info["ci_job_id"] = job_info["id"]
                artifact_index_list.append(build_info)
    artifact_index_file = get_artifact_index_file(case_group=case_group)
    try:
        os.makedirs(os.path.dirname(artifact_index_file))
    except OSError:
        # already created
        pass

    with open(artifact_index_file, "w") as f:
        json.dump(artifact_index_list, f)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("test_case",
                        help="test case folder or file")
    parser.add_argument("ci_config_file",
                        help="gitlab ci config file")
    parser.add_argument("output_path",
                        help="output path of config files")
    parser.add_argument("--pipeline_id", "-p", type=int, default=None,
                        help="pipeline_id")
    parser.add_argument("--job-prefix",
                        help="prefix of the test job name in CI yml file")
    parser.add_argument("--test-case-file-pattern",
                        help="file name pattern used to find Python test case files")
    parser.add_argument('--custom-group',
                        help='select custom-group for the test cases, if other than ExampleTest',
                        choices=['example','test-apps'], default='example')

    args = parser.parse_args()

    if args.job_prefix:
        CIExampleAssignTest.CI_TEST_JOB_PATTERN = re.compile(r"^{}.+".format(args.job_prefix))

    case_group = ExampleGroup if args.custom_group == 'example' else TestAppsGroup

    assign_test = CIExampleAssignTest(args.test_case, args.ci_config_file, case_group=case_group)
    assign_test.assign_cases()
    assign_test.output_configs(args.output_path)
    create_artifact_index_file(case_group=case_group)
