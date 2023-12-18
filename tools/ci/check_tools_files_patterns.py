#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import os
import sys
from pathlib import Path
from typing import Set, Tuple

import yaml
from idf_ci_utils import IDF_PATH, get_git_files


def check(pattern_yml: str, exclude_list: str) -> Tuple[Set, Set]:
    rules_dict = yaml.load(open(pattern_yml), Loader=yaml.FullLoader)
    rules_patterns_set = set()
    for k, v in rules_dict.items():
        if k.startswith('.pattern') and k != '.patterns-python-files' and isinstance(v, list):
            rules_patterns_set.update(v)

    rules_files_set: Set = set()
    idf_path = Path(IDF_PATH)
    for pat in rules_patterns_set:
        rules_files_set.update(idf_path.glob(pat))

    exclude_patterns_set = set()
    for line in open(exclude_list).readlines():
        pat = line.split('#')[0].strip()
        if pat:
            exclude_patterns_set.add(pat)

    exclude_files_set: Set = set()
    for pat in exclude_patterns_set:
        exclude_files_set.update(idf_path.glob(pat))

    missing_files = set()
    git_files = get_git_files(os.path.join(IDF_PATH, 'tools'), full_path=True)
    for f in git_files:
        f = Path(f)
        if f in rules_files_set or f in exclude_files_set or str(f).startswith(os.path.join(IDF_PATH, 'tools', 'test_apps')):
            continue
        missing_files.add(os.path.relpath(f, IDF_PATH))

    return missing_files, rules_patterns_set.intersection(exclude_patterns_set)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='check if all tools files are in rules patterns or exclude list')
    parser.add_argument('-c', '--pattern-yml',
                        default=os.path.join(IDF_PATH, '.gitlab', 'ci', 'rules.yml'),
                        help='yml file path included file patterns')
    parser.add_argument('-e', '--exclude-list',
                        default=os.path.join(IDF_PATH, 'tools', 'ci', 'exclude_check_tools_files.txt'),
                        help='exclude list path')

    args = parser.parse_args()

    res = 0
    not_included_files, dup_patterns = check(args.pattern_yml, args.exclude_list)
    if not_included_files or dup_patterns:
        res = 1
        print('This test is used for making sure of all the tools dir files are recorded in .gitlab/ci/rules.yml to '
              'trigger the related tests, except those files should be excluded.')
        if not_included_files:
            print('Missing Files:')
            for file in not_included_files:
                print('\t' + str(file))
            print('Please add these files or glob patterns to ".gitlab/ci/rules.yml" and put related files under '
                  '".patterns-<test_group>" block to trigger related tests.\n'
                  'Or add them to "tools/ci/exclude_check_tools_files.txt" to exclude them.')

        if dup_patterns:
            print('Duplicated Patterns:')
            for pattern in dup_patterns:
                print('\t' + pattern)
            print('Please remove them from tools/ci/exclude_check_tools_files.txt')

    sys.exit(res)
