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
import re
import subprocess
from sys import exit

try:
    from typing import List
except ImportError:
    # Only used for type annotations
    pass

IGNORE_LIST_MYPY = 'tools/ci/mypy_ignore_list.txt'
COMMENT_RE = re.compile(r'#\s+type:\s+\(', re.MULTILINE)  # REGEX: "# type: ("


def type_comments_in_file(file_name):  # type: (str) -> bool
    """ Check if type annotations (type comments) are present in the file """
    with open(file_name, 'r') as f:
        return bool(COMMENT_RE.search(f.read()))


def types_valid(file_name):  # type: (str) -> bool
    """  Run Mypy check on the given file, return TRUE if Mypy check passes """
    mypy_exit_code = subprocess.call(('mypy %s' % file_name), shell=True)
    return not bool(mypy_exit_code)


def check_files(files):  # type: (List[str]) -> List[str]
    """
    Check files for type annotatins:
    - new python file -> run Mypy check
    - existed file updated by type annotations -> run Mypy check, remove from ignore list
    - existed file updated, but no type annotations added -> skip Mypy check on file
    """
    type_issues = []

    with open(IGNORE_LIST_MYPY, 'r') as f:
        ignore_list = [item.strip() for item in f.readlines()]
        updated_list = ignore_list.copy()

    for file_name in files:
        if file_name in ignore_list:
            if type_comments_in_file(file_name):
                if types_valid(file_name):
                    updated_list.remove(file_name)
                    print('\33[93m\n File %s removed automatically from ignore list - run commit again! \n\33[0m' % file_name)
                else:
                    type_issues.append(file_name)
        else:
            if not types_valid(file_name):
                type_issues.append(file_name)

    if updated_list != ignore_list:
        with open(IGNORE_LIST_MYPY, 'w') as f:
            for item in updated_list:
                f.write('%s\n' % item)
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
