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

import argparse
import fnmatch
import glob
import os
import sys

import yaml
from idf_ci_utils import IDF_PATH, get_git_files, magic_check, magic_check_bytes, translate

# Monkey patch starts
# glob.glob will ignore all files starts with ``.``
# don't ignore them here
# need to keep the same argument as glob._ishidden


def _ishidden(path):  # pylint: disable=W0613
    return False


fnmatch.translate = translate

glob.magic_check = magic_check
glob.magic_check_bytes = magic_check_bytes
glob._ishidden = _ishidden  # pylint: disable=W0212
# ends here


def check(pattern_yml, exclude_list):
    rules_dict = yaml.load(open(pattern_yml), Loader=yaml.FullLoader)
    rules_patterns_set = set()
    for k, v in rules_dict.items():
        if k.startswith('.pattern') and isinstance(v, list):
            rules_patterns_set.update(v)
    rules_files_set = set()
    for pat in rules_patterns_set:
        rules_files_set.update(glob.glob(os.path.join(IDF_PATH, pat), recursive=True))

    exclude_patterns_set = set()
    exclude_patterns_set.update([path.split('#')[0].strip() for path in open(exclude_list).readlines() if path])
    exclude_files_set = set()
    for pat in exclude_patterns_set:
        exclude_files_set.update(glob.glob(os.path.join(IDF_PATH, pat), recursive=True))

    missing_files = set()
    git_files = get_git_files(os.path.join(IDF_PATH, 'tools'), full_path=True)
    for f in git_files:
        if f in rules_files_set or f in exclude_files_set:
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
    if not_included_files:
        print('Missing Files: (please add to tools/ci/exclude_check_tools_files.txt')
        for file in not_included_files:
            print(file)
        res = 1
    if dup_patterns:
        print('Duplicated Patterns: (please check .gitlab/ci/rules.yml and tools/ci/exclude_check_tools_files.txt')
        for pattern in dup_patterns:
            print(pattern)
        res = 1

    sys.exit(res)
