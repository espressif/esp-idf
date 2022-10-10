#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""
Check if all rules in rules.yml used or not in CI yaml files.
"""

import argparse
import os
import re
import sys
from copy import deepcopy
from typing import Any, Dict, List, Optional, Set, Union

import yaml
from idf_ci_utils import IDF_PATH

ROOT_YML_FP = os.path.join(IDF_PATH, '.gitlab-ci.yml')


def load_yaml(file_path: str) -> Any:
    return yaml.load(open(file_path), Loader=yaml.FullLoader)


class YMLConfig:
    def __init__(self, root_yml_file_path: str) -> None:
        self._config: Optional[Dict] = None
        self._all_extends: Optional[Set] = None

        self.root_yml = load_yaml(root_yml_file_path)
        assert self.root_yml

    @staticmethod
    def _list(str_or_list: Union[str, List]) -> List:
        if isinstance(str_or_list, str):
            return [str_or_list]
        if isinstance(str_or_list, list):
            return str_or_list
        raise ValueError(
            'Wrong type: {}. Only supports str or list.'.format(type(str_or_list))
        )

    @property
    def config(self) -> Dict:
        if self._config:
            return self._config

        all_config = dict()
        for item in self.root_yml['include']:
            all_config.update(load_yaml(os.path.join(IDF_PATH, item)))
        self._config = all_config
        return self._config

    @property
    def all_extends(self) -> Set:
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

    def exists(self, key: str) -> bool:
        if key in self.all_extends:
            return True
        return False


YML_CONFIG = YMLConfig(ROOT_YML_FP)


def get_needed_rules() -> Set[str]:
    return deepcopy(YML_CONFIG.all_extends)


def validate_needed_rules(rules_yml: 'os.PathLike[str]') -> int:
    res = 0
    needed_rules = deepcopy(YML_CONFIG.all_extends)
    with open(rules_yml) as fr:
        for index, line in enumerate(fr):
            if line.startswith('.rules:'):
                key = line.strip().rsplit(':', 1)[0]
                if not YML_CONFIG.exists(key):
                    print(
                        '{}:{}:WARNING:rule "{}" unused'.format(rules_yml, index, key)
                    )
                else:
                    needed_rules.remove(key)

    if needed_rules:
        for item in needed_rules:
            print('ERROR: missing rule: "{}"'.format(item))
        res = 1

    if res == 0:
        print('Pass')
    return res


def parse_submodule_paths(
    gitsubmodules: str = os.path.join(IDF_PATH, '.gitmodules')
) -> List[str]:
    path_regex = re.compile(r'^\s+path = (.+)$', re.MULTILINE)
    with open(gitsubmodules, 'r') as f:
        data = f.read()

    res = []
    for item in path_regex.finditer(data):
        res.append(item.group(1))

    return res


def validate_submodule_patterns() -> int:
    submodule_paths = sorted(['.gitmodules'] + parse_submodule_paths())
    submodule_paths_in_patterns = sorted(
        YML_CONFIG.config.get('.patterns-submodule', [])
    )

    res = 0
    if submodule_paths != submodule_paths_in_patterns:
        res = 1
        print('please update the pattern ".patterns-submodule"')
        should_remove = set(submodule_paths_in_patterns) - set(submodule_paths)
        if should_remove:
            print(f'- should remove: {should_remove}')
        should_add = set(submodule_paths) - set(submodule_paths_in_patterns)
        if should_add:
            print(f'- should add: {should_add}')

    return res


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        'rules_yml',
        nargs='?',
        default=os.path.join(IDF_PATH, '.gitlab', 'ci', 'rules.yml'),
        help='rules.yml file path',
    )
    args = parser.parse_args()

    exit_code = 0
    if validate_needed_rules(args.rules_yml):
        exit_code = 1
    if validate_submodule_patterns():
        exit_code = 1

    sys.exit(exit_code)
