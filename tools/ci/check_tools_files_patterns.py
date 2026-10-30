#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import os
import sys
from fnmatch import fnmatchcase

import yaml
from idf_ci_utils import IDF_PATH
from idf_ci_utils import get_git_files


def _match_parts(path_parts: list[str], pattern_parts: list[str]) -> bool:
    if not pattern_parts:
        return not path_parts
    if pattern_parts[0] == '**':
        # '**' stands for zero or more directories and never consumes the file name. A trailing '**' therefore
        # matches no file, as in Path.glob() before Python 3.13; GitLab rules:changes would not cover deeper files.
        return any(_match_parts(path_parts[i:], pattern_parts[1:]) for i in range(len(path_parts)))
    return (
        bool(path_parts)
        and fnmatchcase(path_parts[0], pattern_parts[0])
        and _match_parts(path_parts[1:], pattern_parts[1:])
    )


def path_matches(path: str, pattern: str) -> bool:
    """
    Match a file path relative to IDF_PATH against a Path.glob() pattern.

    Globbing the disk walks untracked build output too, which made the check take seconds in a used checkout.
    Splitting on '/' works on Windows as well, because git ls-files and the patterns use '/' on every OS.
    fnmatchcase() keeps matching case-sensitive on Windows and macOS, as GitLab CI does.
    """
    return _match_parts(path.split('/'), pattern.split('/'))


def check(pattern_yml: str, exclude_list: str) -> tuple[set, set]:
    rules_dict = yaml.load(open(pattern_yml), Loader=yaml.FullLoader)
    rules_patterns_set = set()
    for k, v in rules_dict.items():
        if k.startswith('.pattern') and k != '.patterns-python-files' and isinstance(v, list):
            rules_patterns_set.update(v)

    exclude_patterns_set = set()
    for line in open(exclude_list).readlines():
        pat = line.split('#')[0].strip()
        if pat:
            exclude_patterns_set.add(pat)

    patterns = rules_patterns_set | exclude_patterns_set
    missing_files = set()
    for f in get_git_files(os.path.join(IDF_PATH, 'tools')):
        f = f'tools/{f}'
        if f.startswith('tools/test_apps/') or any(path_matches(f, pat) for pat in patterns):
            continue
        missing_files.add(f)

    return missing_files, rules_patterns_set.intersection(exclude_patterns_set)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='check if all tools files are in rules patterns or exclude list')
    parser.add_argument(
        '-c',
        '--pattern-yml',
        default=os.path.join(IDF_PATH, '.gitlab', 'ci', 'rules.yml'),
        help='yml file path included file patterns',
    )
    parser.add_argument(
        '-e',
        '--exclude-list',
        default=os.path.join(IDF_PATH, 'tools', 'ci', 'exclude_check_tools_files.txt'),
        help='exclude list path',
    )

    args = parser.parse_args()

    res = 0
    not_included_files, dup_patterns = check(args.pattern_yml, args.exclude_list)
    if not_included_files or dup_patterns:
        res = 1
        print(
            'This test is used for making sure of all the tools dir files are recorded in .gitlab/ci/rules.yml to '
            'trigger the related tests, except those files should be excluded.'
        )
        if not_included_files:
            print('Missing Files:')
            for file in not_included_files:
                print('\t' + str(file))
            print(
                'Please add these files or glob patterns to ".gitlab/ci/rules.yml" and put related files under '
                '".patterns-<test_group>" block to trigger related tests.\n'
                'Or add them to "tools/ci/exclude_check_tools_files.txt" to exclude them.'
            )

        if dup_patterns:
            print('Duplicated Patterns:')
            for pattern in dup_patterns:
                print('\t' + pattern)
            print('Please remove them from tools/ci/exclude_check_tools_files.txt')

    sys.exit(res)
