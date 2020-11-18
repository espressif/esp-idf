#!/usr/bin/env python
#
# Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
import os
from sys import exit

from idf_ci_utils import is_executable


def _strip_each_item(iterable):
    res = []
    for item in iterable:
        if item:
            res.append(item.strip())
    return res


IDF_PATH = os.getenv('IDF_PATH', os.getcwd())
EXECUTABLE_LIST_FN = os.path.join(IDF_PATH, 'tools/ci/executable-list.txt')
known_executables = _strip_each_item(open(EXECUTABLE_LIST_FN).readlines())


def check_executable_list():
    ret = 0
    for index, fn in enumerate(known_executables):
        if not os.path.exists(os.path.join(IDF_PATH, fn)):
            print('{}:{} {} not exists. Please remove it manually'.format(EXECUTABLE_LIST_FN, index + 1, fn))
            ret = 1
    return ret


def check_executables(files):
    ret = 0
    for fn in files:
        if not is_executable(fn):
            continue
        if fn not in known_executables:
            print('"{}" is not in {}'.format(fn, EXECUTABLE_LIST_FN))
            ret = 1
    return ret


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--action', choices=['executables', 'list'], required=True,
                        help='if "executables", pass all your executables to see if it\'s in the list.'
                             'if "list", check if all items on your list exist')
    parser.add_argument('filenames', nargs='*', help='Filenames to check.')
    args = parser.parse_args()

    if args.action == 'executables':
        ret = check_executables(args.filenames)
    elif args.action == 'list':
        ret = check_executable_list()
    else:
        raise ValueError

    return ret


if __name__ == '__main__':
    exit(main())
