# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
Command line tool to assign tests to CI test jobs.
"""
import argparse
import json
import os
import re
import sys
from copy import deepcopy

import yaml

try:
    from yaml import CLoader as Loader
except ImportError:
    from yaml import Loader as Loader  # type: ignore

import gitlab_api
from tiny_test_fw.Utility import CIAssignTest

try:
    from idf_py_actions.constants import PREVIEW_TARGETS, SUPPORTED_TARGETS
except ImportError:
    sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..'))

    from idf_py_actions.constants import PREVIEW_TARGETS, SUPPORTED_TARGETS

IDF_PATH_FROM_ENV = os.getenv('IDF_PATH', '')


class IDFCaseGroup(CIAssignTest.Group):
    BUILD_JOB_NAMES = None

    @classmethod
    def get_artifact_index_file(cls):
        if IDF_PATH_FROM_ENV:
            artifact_index_file = os.path.join(IDF_PATH_FROM_ENV, 'artifact_index.json')
        else:
            artifact_index_file = 'artifact_index.json'
        return artifact_index_file


class IDFAssignTest(CIAssignTest.AssignTest):
    DEFAULT_FILTER = {
        'category': 'function',
        'ignore': False,
        'supported_in_ci': True,
        'nightly_run': False,
    }

    def __init__(self, test_case_path, ci_config_file, case_group=IDFCaseGroup):
        super(IDFAssignTest, self).__init__(test_case_path, ci_config_file, case_group)

    def format_build_log_path(self, parallel_num):
        return 'list_job_{}.json'.format(parallel_num)

    def create_artifact_index_file(self, project_id=None, pipeline_id=None):
        if project_id is None:
            project_id = os.getenv('CI_PROJECT_ID')
        if pipeline_id is None:
            pipeline_id = os.getenv('CI_PIPELINE_ID')
        gitlab_inst = gitlab_api.Gitlab(project_id)

        artifact_index_list = []
        for build_job_name in self.case_group.BUILD_JOB_NAMES:
            job_info_list = gitlab_inst.find_job_id(build_job_name, pipeline_id=pipeline_id)
            for job_info in job_info_list:
                parallel_num = job_info['parallel_num'] or 1  # Could be None if "parallel_num" not defined for the job
                raw_data = gitlab_inst.download_artifact(job_info['id'],
                                                         [self.format_build_log_path(parallel_num)])[0]
                build_info_list = [json.loads(line) for line in raw_data.decode().splitlines()]
                for build_info in build_info_list:
                    build_info['ci_job_id'] = job_info['id']
                    artifact_index_list.append(build_info)
        artifact_index_file = self.case_group.get_artifact_index_file()
        with open(artifact_index_file, 'w') as f:
            json.dump(artifact_index_list, f)

    def search_cases(self, case_filter=None):
        _filter = deepcopy(case_filter) if case_filter else {}
        if 'NIGHTLY_RUN' in os.environ or 'BOT_LABEL_NIGHTLY_RUN' in os.environ:
            _filter.update({'nightly_run': True})
        return super().search_cases(_filter)


class ExampleGroup(IDFCaseGroup):
    SORT_KEYS = CI_JOB_MATCH_KEYS = ['env_tag', 'target']

    EXAMPLE_TARGETS = SUPPORTED_TARGETS + PREVIEW_TARGETS
    BUILD_JOB_NAMES = ['build_examples_cmake_{}'.format(target) for target in EXAMPLE_TARGETS]  # type: ignore


class TestAppsGroup(ExampleGroup):
    TEST_APP_TARGETS = SUPPORTED_TARGETS + PREVIEW_TARGETS
    BUILD_JOB_NAMES = ['build_test_apps_{}'.format(target) for target in TEST_APP_TARGETS]  # type: ignore


class ComponentUTGroup(TestAppsGroup):
    UNIT_TEST_TARGETS = SUPPORTED_TARGETS + PREVIEW_TARGETS
    BUILD_JOB_NAMES = ['build_component_ut_{}'.format(target) for target in UNIT_TEST_TARGETS]  # type: ignore


class UnitTestGroup(IDFCaseGroup):
    SORT_KEYS = ['test environment', 'tags', 'chip_target']
    CI_JOB_MATCH_KEYS = ['test environment']

    UNIT_TEST_TARGETS = SUPPORTED_TARGETS + PREVIEW_TARGETS
    BUILD_JOB_NAMES = ['build_esp_idf_tests_cmake_{}'.format(target) for target in UNIT_TEST_TARGETS]  # type: ignore

    MAX_CASE = 50
    ATTR_CONVERT_TABLE = {
        'execution_time': 'execution time'
    }
    DUT_CLS_NAME = {
        'esp32': 'ESP32DUT',
        'esp32s2': 'ESP32S2DUT',
        'esp32s3': 'ESP32S3DUT',
        'esp32c2': 'ESP32C2DUT',
        'esp32c3': 'ESP32C3DUT',
        'esp32c6': 'ESP32C6DUT',
        'esp32h2': 'ESP32H2DUT',
        'esp8266': 'ESP8266DUT',
    }

    def __init__(self, case):
        super(UnitTestGroup, self).__init__(case)
        for tag in self._get_case_attr(case, 'tags'):
            self.ci_job_match_keys.add(tag)

    @staticmethod
    def _get_case_attr(case, attr):
        if attr in UnitTestGroup.ATTR_CONVERT_TABLE:
            attr = UnitTestGroup.ATTR_CONVERT_TABLE[attr]
        return case[attr]

    def add_extra_case(self, case):
        """ If current group contains all tags required by case, then add succeed """
        added = False
        if self.accept_new_case():
            for key in self.filters:
                if self._get_case_attr(case, key) != self.filters[key]:
                    if key == 'tags':
                        if set(self._get_case_attr(case, key)).issubset(set(self.filters[key])):
                            continue
                    break
            else:
                self.case_list.append(case)
                added = True
        return added

    def _create_extra_data(self, test_cases, test_function):
        """
        For unit test case, we need to copy some attributes of test cases into config file.
        So unit test function knows how to run the case.
        """
        case_data = []
        for case in test_cases:
            one_case_data = {
                'config': self._get_case_attr(case, 'config'),
                'name': self._get_case_attr(case, 'summary'),
                'reset': self._get_case_attr(case, 'reset'),
                'timeout': self._get_case_attr(case, 'timeout'),
            }

            if test_function in ['run_multiple_devices_cases', 'run_multiple_stage_cases']:
                try:
                    one_case_data['child case num'] = self._get_case_attr(case, 'child case num')
                except KeyError as e:
                    print('multiple devices/stages cases must contains at least two test functions')
                    print('case name: {}'.format(one_case_data['name']))
                    raise e

            case_data.append(one_case_data)
        return case_data

    def _divide_case_by_test_function(self):
        """
        divide cases of current test group by test function they need to use

        :return: dict of list of cases for each test functions
        """
        case_by_test_function = {
            'run_multiple_devices_cases': [],
            'run_multiple_stage_cases': [],
            'run_unit_test_cases': [],
        }

        for case in self.case_list:
            if case['multi_device'] == 'Yes':
                case_by_test_function['run_multiple_devices_cases'].append(case)
            elif case['multi_stage'] == 'Yes':
                case_by_test_function['run_multiple_stage_cases'].append(case)
            else:
                case_by_test_function['run_unit_test_cases'].append(case)
        return case_by_test_function

    def output(self):
        """
        output data for job configs

        :return: {"Filter": case filter, "CaseConfig": list of case configs for cases in this group}
        """

        target = self._get_case_attr(self.case_list[0], 'chip_target')
        if target:
            overwrite = {
                'dut': {
                    'package': 'ttfw_idf',
                    'class': self.DUT_CLS_NAME[target],
                }
            }
        else:
            overwrite = dict()

        case_by_test_function = self._divide_case_by_test_function()

        output_data = {
            # we don't need filter for test function, as UT uses a few test functions for all cases
            'CaseConfig': [
                {
                    'name': test_function,
                    'extra_data': self._create_extra_data(test_cases, test_function),
                    'overwrite': overwrite,
                } for test_function, test_cases in case_by_test_function.items() if test_cases
            ],
        }
        return output_data


class ExampleAssignTest(IDFAssignTest):
    CI_TEST_JOB_PATTERN = re.compile(r'^example_test_.+')

    def __init__(self, test_case_path, ci_config_file):
        super(ExampleAssignTest, self).__init__(test_case_path, ci_config_file, case_group=ExampleGroup)


class TestAppsAssignTest(IDFAssignTest):
    CI_TEST_JOB_PATTERN = re.compile(r'^test_app_test_.+')

    def __init__(self, test_case_path, ci_config_file):
        super(TestAppsAssignTest, self).__init__(test_case_path, ci_config_file, case_group=TestAppsGroup)


class ComponentUTAssignTest(IDFAssignTest):
    CI_TEST_JOB_PATTERN = re.compile(r'^component_ut_test_.+')

    def __init__(self, test_case_path, ci_config_file):
        super(ComponentUTAssignTest, self).__init__(test_case_path, ci_config_file, case_group=ComponentUTGroup)


class UnitTestAssignTest(IDFAssignTest):
    CI_TEST_JOB_PATTERN = re.compile(r'^UT_.+')

    def __init__(self, test_case_path, ci_config_file):
        super(UnitTestAssignTest, self).__init__(test_case_path, ci_config_file, case_group=UnitTestGroup)

    def search_cases(self, case_filter=None):
        """
        For unit test case, we don't search for test functions.
        The unit test cases is stored in a yaml file which is created in job build-idf-test.
        """

        def find_by_suffix(suffix, path):
            res = []
            for root, _, files in os.walk(path):
                for file in files:
                    if file.endswith(suffix):
                        res.append(os.path.join(root, file))
            return res

        def get_test_cases_from_yml(yml_file):
            try:
                with open(yml_file) as fr:
                    raw_data = yaml.load(fr, Loader=Loader)
                test_cases = raw_data['test cases']
            except (IOError, KeyError):
                return []
            else:
                return test_cases

        test_cases = []
        for path in self.test_case_paths:
            if os.path.isdir(path):
                for yml_file in find_by_suffix('.yml', path):
                    test_cases.extend(get_test_cases_from_yml(yml_file))
            elif os.path.isfile(path) and path.endswith('.yml'):
                test_cases.extend(get_test_cases_from_yml(path))
            else:
                print('Test case path is invalid. Should only happen when use @bot to skip unit test.')

        # filter keys are lower case. Do map lower case keys with original keys.
        try:
            key_mapping = {x.lower(): x for x in test_cases[0].keys()}
        except IndexError:
            key_mapping = dict()
        if case_filter:
            for key in case_filter:
                filtered_cases = []
                for case in test_cases:
                    try:
                        mapped_key = key_mapping[key]
                        # bot converts string to lower case
                        if isinstance(case[mapped_key], str):
                            _value = case[mapped_key].lower()
                        else:
                            _value = case[mapped_key]
                        if _value in case_filter[key]:
                            filtered_cases.append(case)
                    except KeyError:
                        # case don't have this key, regard as filter success
                        filtered_cases.append(case)
                test_cases = filtered_cases
        # sort cases with configs and test functions
        # in later stage cases with similar attributes are more likely to be assigned to the same job
        # it will reduce the count of flash DUT operations
        test_cases.sort(key=lambda x: x['config'] + x['multi_stage'] + x['multi_device'])
        return test_cases


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('case_group', choices=['example_test', 'custom_test', 'unit_test', 'component_ut'])
    parser.add_argument('test_case_paths', nargs='+', help='test case folder or file')
    parser.add_argument('-c', '--config', default=os.path.join(IDF_PATH_FROM_ENV, '.gitlab', 'ci', 'target-test.yml'),
                        help='gitlab ci config file')
    parser.add_argument('-o', '--output', help='output path of config files')
    parser.add_argument('--pipeline_id', '-p', type=int, default=None, help='pipeline_id')
    parser.add_argument('--test-case-file-pattern', help='file name pattern used to find Python test case files')
    args = parser.parse_args()

    SUPPORTED_TARGETS.extend(PREVIEW_TARGETS)

    test_case_paths = [os.path.join(IDF_PATH_FROM_ENV, path) if not os.path.isabs(path) else path for path in
                       args.test_case_paths]  # type: ignore
    args_list = [test_case_paths, args.config]
    if args.case_group == 'example_test':
        assigner = ExampleAssignTest(*args_list)
    elif args.case_group == 'custom_test':
        assigner = TestAppsAssignTest(*args_list)
    elif args.case_group == 'unit_test':
        assigner = UnitTestAssignTest(*args_list)
    elif args.case_group == 'component_ut':
        assigner = ComponentUTAssignTest(*args_list)
    else:
        raise SystemExit(1)  # which is impossible

    if args.test_case_file_pattern:
        assigner.CI_TEST_JOB_PATTERN = re.compile(r'{}'.format(args.test_case_file_pattern))

    assigner.assign_cases()
    assigner.output_configs(args.output)
    assigner.create_artifact_index_file()
