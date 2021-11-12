#!/usr/bin/env python
#
# Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

from __future__ import print_function, unicode_literals

import argparse
import os
import sys
from io import open

from check_kconfigs import valid_directory
from idf_ci_utils import get_submodule_dirs

# FILES_TO_CHECK used as "startswith" pattern to match sdkconfig.defaults variants
FILES_TO_CHECK = ('sdkconfig.ci', 'sdkconfig.defaults')

# ignored directories (makes sense only when run on IDF_PATH)
# Note: IGNORE_DIRS is a tuple in order to be able to use it directly with the startswith() built-in function which
# accepts tuples but no lists.
IGNORE_DIRS = (
)


def _parse_path(path, sep=None):
    ret = set()
    with open(path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line.startswith('#') and len(line) > 0:
                ret.add(line.split(sep)[0])
    return ret


def _valid_directory(path):
    if not os.path.isdir(path):
        raise argparse.ArgumentTypeError('{} is not a valid directory!'.format(path))
    return path


def main():
    parser = argparse.ArgumentParser(description='Kconfig options checker')
    parser.add_argument('files', nargs='*',
                        help='Kconfig files')
    parser.add_argument('--includes', '-d', nargs='*',
                        help='Extra paths for recursively searching Kconfig files. (for example $IDF_PATH)',
                        type=valid_directory)
    parser.add_argument('--exclude-submodules', action='store_true',
                        help='Exclude submodules')
    args = parser.parse_args()

    success_counter = 0
    failure_counter = 0
    ignore_counter = 0

    deprecated_options = set()

    ignore_dirs = IGNORE_DIRS
    if args.exclude_submodules:
        for submodule in get_submodule_dirs(full_path=True):
            ignore_dirs = ignore_dirs + tuple(submodule)

    files = [os.path.abspath(file_path) for file_path in args.files]

    if args.includes:
        for directory in args.includes:
            for root, dirnames, filenames in os.walk(directory):
                for filename in filenames:
                    full_path = os.path.join(root, filename)
                    if filename.startswith(FILES_TO_CHECK):
                        files.append(full_path)
                    elif filename == 'sdkconfig.rename':
                        deprecated_options |= _parse_path(full_path)

    for full_path in files:
        if full_path.startswith(ignore_dirs):
            print('{}: Ignored'.format(full_path))
            ignore_counter += 1
            continue
        used_options = _parse_path(full_path, '=')
        used_deprecated_options = deprecated_options & used_options
        if len(used_deprecated_options) > 0:
            print('{}: The following options are deprecated: {}'
                  .format(full_path, ', '.join(used_deprecated_options)))
            failure_counter += 1
        else:
            print('{}: OK'.format(full_path))
            success_counter += 1

    if ignore_counter > 0:
        print('{} files have been ignored.'.format(ignore_counter))
    if success_counter > 0:
        print('{} files have been successfully checked.'.format(success_counter))
    if failure_counter > 0:
        print('{} files have errors. Please take a look at the log.'.format(failure_counter))
        return 1

    if not files:
        print('WARNING: no files specified. Please specify files or use '
              '"--includes" to search Kconfig files recursively')
    return 0


if __name__ == '__main__':
    sys.exit(main())
