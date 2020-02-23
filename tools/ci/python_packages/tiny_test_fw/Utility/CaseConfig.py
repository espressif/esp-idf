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
Processing case config files.
This is mainly designed for CI, we need to auto create and assign test jobs.

Template Config File::

    TestConfig:
      app:
        package: ttfw_idf
        class: Example
      dut:
        path:
        class:
      config_file: /somewhere/config_file_for_runner
      test_name: CI_test_job_1

    Filter:
      chip: ESP32
      env_tag: default

    CaseConfig:
      - name: test_examples_protocol_https_request
        # optional
        extra_data: some extra data passed to case with kwarg extra_data
        overwrite:  # overwrite test configs
          app:
            package: ttfw_idf
            class: Example
      - name: xxx
"""
import importlib

import yaml
try:
    from yaml import CLoader as Loader
except ImportError:
    from yaml import Loader as Loader

from . import TestCase


def _convert_to_lower_case_bytes(item):
    """
    bot filter is always lower case string.
    this function will convert to all string to lower case.
    Note: Unicode strings are converted to bytes.
    """
    if isinstance(item, (tuple, list)):
        output = [_convert_to_lower_case_bytes(v) for v in item]
    elif isinstance(item, type(b'')):
        output = item.lower()
    elif isinstance(item, type(u'')):
        output = item.encode().lower()
    else:
        output = item
    return output


def _filter_one_case(test_method, case_filter):
    """ Apply filter for one case (the filter logic is the same as described in ``filter_test_cases``) """
    filter_result = True
    # filter keys are lower case. Do map lower case keys with original keys.
    key_mapping = {x.lower(): x for x in test_method.case_info.keys()}

    for orig_key in case_filter:
        key = key_mapping[orig_key]
        if key in test_method.case_info:
            # the filter key is both in case and filter
            # we need to check if they match
            filter_item = _convert_to_lower_case_bytes(case_filter[orig_key])
            accepted_item = _convert_to_lower_case_bytes(test_method.case_info[key])

            if isinstance(filter_item, (tuple, list)) \
                    and isinstance(accepted_item, (tuple, list)):
                # both list/tuple, check if they have common item
                filter_result = True if set(filter_item) & set(accepted_item) else False
            elif isinstance(filter_item, (tuple, list)):
                # filter item list/tuple, check if case accepted value in filter item list/tuple
                filter_result = True if accepted_item in filter_item else False
            elif isinstance(accepted_item, (tuple, list)):
                # accepted item list/tuple, check if case filter value is in accept item list/tuple
                filter_result = True if filter_item in accepted_item else False
            else:
                if type(filter_item) != type(accepted_item):
                    # This will catch silent ignores of test cases when Unicode and bytes are compared
                    raise AssertionError(filter_item, '!=', accepted_item)
                # both string/int, just do string compare
                filter_result = (filter_item == accepted_item)
        else:
            # key in filter only, which means the case supports all values for this filter key, match succeed
            pass
        if not filter_result:
            # match failed
            break
    return filter_result


def filter_test_cases(test_methods, case_filter):
    """
    filter test case. filter logic:

        1. if filter key both in case attribute and filter:
            * if both value is string/int, then directly compare
            * if one is list/tuple, the other one is string/int, then check if string/int is in list/tuple
            * if both are list/tuple, then check if they have common item
        2. if only case attribute or filter have the key, filter succeed
        3. will do case insensitive compare for string

    for example, the following are match succeed scenarios
    (the rule is symmetric, result is same if exchange values for user filter and case attribute):

        * user case filter is ``chip: ["esp32", "esp32c"]``, case doesn't have ``chip`` attribute
        * user case filter is ``chip: ["esp32", "esp32c"]``, case attribute is ``chip: "esp32"``
        * user case filter is ``chip: "esp32"``, case attribute is ``chip: "esp32"``


    :param test_methods: a list of test methods functions
    :param case_filter: case filter
    :return: filtered test methods
    """
    filtered_test_methods = []
    for test_method in test_methods:
        if _filter_one_case(test_method, case_filter):
            filtered_test_methods.append(test_method)
    return filtered_test_methods


class Parser(object):
    DEFAULT_CONFIG = {
        "TestConfig": dict(),
        "Filter": dict(),
        "CaseConfig": [{"extra_data": None}],
    }

    @classmethod
    def parse_config_file(cls, config_file):
        """
        parse from config file and then update to default config.

        :param config_file: config file path
        :return: configs
        """
        configs = cls.DEFAULT_CONFIG.copy()
        if config_file:
            with open(config_file, "r") as f:
                configs.update(yaml.load(f, Loader=Loader))
        return configs

    @classmethod
    def handle_overwrite_args(cls, overwrite):
        """
        handle overwrite configs. import module from path and then get the required class.

        :param overwrite: overwrite args
        :return: dict of (original key: class)
        """
        output = dict()
        for key in overwrite:
            module = importlib.import_module(overwrite[key]["package"])
            output[key] = module.__getattribute__(overwrite[key]["class"])
        return output

    @classmethod
    def apply_config(cls, test_methods, config_file):
        """
        apply config for test methods

        :param test_methods: a list of test methods functions
        :param config_file: case filter file
        :return: filtered cases
        """
        configs = cls.parse_config_file(config_file)
        test_case_list = []
        for _config in configs["CaseConfig"]:
            _filter = configs["Filter"].copy()
            _overwrite = cls.handle_overwrite_args(_config.pop("overwrite", dict()))
            _extra_data = _config.pop("extra_data", None)
            _filter.update(_config)
            for test_method in test_methods:
                if _filter_one_case(test_method, _filter):
                    test_case_list.append(TestCase.TestCase(test_method, _extra_data, **_overwrite))
        return test_case_list


class Generator(object):
    """ Case config file generator """

    def __init__(self):
        self.default_config = {
            "TestConfig": dict(),
            "Filter": dict(),
        }

    def set_default_configs(self, test_config, case_filter):
        """
        :param test_config: "TestConfig" value
        :param case_filter: "Filter" value
        :return: None
        """
        self.default_config = {"TestConfig": test_config, "Filter": case_filter}

    def generate_config(self, case_configs, output_file):
        """
        :param case_configs: "CaseConfig" value
        :param output_file: output file path
        :return: None
        """
        config = self.default_config.copy()
        config.update({"CaseConfig": case_configs})
        with open(output_file, "w") as f:
            yaml.dump(config, f)
