#!/usr/bin/env python
#
# Copyright 2021 Espressif Systems (Shanghai) CO LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
Check if all rules in rules.yml used or not in CI yaml files.
"""

import argparse
import os
import sys
from copy import deepcopy

import yaml
from idf_ci_utils import IDF_PATH

ROOT_YML_FP = os.path.join(IDF_PATH, '.gitlab-ci.yml')


def load_yaml(file_path):
    return yaml.load(open(file_path), Loader=yaml.FullLoader)


class YMLConfig:
    def __init__(self, root_yml_file_path):
        self._config = None
        self._all_extends = None

        self.root_yml = load_yaml(root_yml_file_path)
        assert self.root_yml

    @staticmethod
    def _list(str_or_list):
        if isinstance(str_or_list, str):
            return [str_or_list]
        if isinstance(str_or_list, list):
            return str_or_list
        raise ValueError('Wrong type: {}. Only supports str or list.'.format(type(str_or_list)))

    @property
    def config(self):
        if self._config:
            return self._config

        all_config = dict()
        for item in self.root_yml['include']:
            if not item.endswith('rules.yml'):
                all_config.update(load_yaml(os.path.join(IDF_PATH, item)))
        self._config = all_config
        return self._config

    @property
    def all_extends(self):
        if self._all_extends:
            return self._all_extends

        res = set([])
        for v in self.config.values():
            if 'extends' in v:
                for item in self._list(v['extends']):
                    if item.startswith('.rules:'):
                        res.add(item)
        self._all_extends = res
        return self._all_extends

    def exists(self, key):
        if key in self.all_extends:
            return True
        return False


def validate(rules_yml):
    yml_config = YMLConfig(ROOT_YML_FP)
    res = 0
    needed_rules = deepcopy(yml_config.all_extends)
    with open(rules_yml) as fr:
        for index, line in enumerate(fr):
            if line.startswith('.rules:'):
                key = line.strip().rsplit(':', 1)[0]
                if not yml_config.exists(key):
                    print('{}:{}:WARNING:rule "{}" unused'.format(rules_yml, index, key))
                else:
                    needed_rules.remove(key)

    if needed_rules:
        for item in needed_rules:
            print('ERROR: missing rule: "{}"'.format(item))
        res = 1

    if res == 0:
        print('Pass')
    return res


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('rules_yml', nargs='?', default=os.path.join(IDF_PATH, '.gitlab', 'ci', 'rules.yml'),
                        help='rules.yml file path')
    args = parser.parse_args()

    sys.exit(validate(args.rules_yml))
