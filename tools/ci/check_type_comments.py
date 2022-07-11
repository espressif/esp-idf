#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import subprocess
from sys import exit

try:
    from typing import List
except ImportError:
    # Only used for type annotations
    pass

IGNORE_LIST_MYPY = 'tools/ci/mypy_ignore_list.txt'


def types_valid_global_rules(file_name, ignorelisted):  # type: (str, bool) -> bool
    """
    Run Mypy check with global rules on the given file, return TRUE if Mypy check passes
    """
    output = subprocess.DEVNULL if ignorelisted else None
    mypy_exit_code = subprocess.call('mypy {}'.format(file_name), shell=True, stdout=output)

    return not bool(mypy_exit_code)


def types_valid_ignored_rules(file_name):  # type: (str) -> bool
    """
    Run Mypy check with rules for ignore list on the given file, return TRUE if Mypy check passes
    """
    mypy_exit_code = subprocess.call('mypy {} --allow-untyped-defs'.format(file_name), shell=True)
    return not bool(mypy_exit_code)


def check_files(files):  # type: (List[str]) -> List[str]
    """
    Check files for type annotatins:
    - new python file -> run Mypy check with global rules
    - existed file on ignore list -> run Mypy check with Global rules:
               global check OK ->  remove from ignore list (updates on the file system)
               global check FAILS -> run Mypy check with Ignored rules
    Global rules: Enforce type annotations for all functions in file
    Ignored rules: Do not enforce untyped functions, check only already typed
    """
    type_issues = []

    with open(IGNORE_LIST_MYPY, 'r') as f:
        ignore_list = [item.strip() for item in f.readlines()]
        updated_ignore_list = ignore_list.copy()

    for file_name in files:
        if file_name in ignore_list:
            if types_valid_global_rules(file_name, ignorelisted=True):
                updated_ignore_list.remove(file_name)
                print('\33[93m\n File {} removed from ignore list - run commit again! \n\33[0m'.format(file_name))
                continue

            if types_valid_ignored_rules(file_name):
                continue
            else:
                type_issues.append(file_name)

        else:
            if not types_valid_global_rules(file_name, ignorelisted=False):
                type_issues.append(file_name)

    if updated_ignore_list != ignore_list:
        with open(IGNORE_LIST_MYPY, 'w') as f:
            for item in updated_ignore_list:
                f.write('{}\n'.format(item))

    return type_issues


def main():  # type: () -> None
    parser = argparse.ArgumentParser()
    parser.add_argument('filenames', nargs='*', help='Filenames to check.')
    args = parser.parse_args()

    type_issues = check_files(args.filenames)

    if type_issues:
        print('mypy check failed for:')
        for file_name in type_issues:
            print('\t', file_name)
        exit(1)


if __name__ == '__main__':
    main()
