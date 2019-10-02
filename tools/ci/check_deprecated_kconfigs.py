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

from __future__ import print_function
from __future__ import unicode_literals
import argparse
import os
import sys
from io import open

# FILES_TO_CHECK used as "startswith" pattern to match sdkconfig.defaults variants
FILES_TO_CHECK = ('sdkconfig.ci', 'sdkconfig.defaults')

# ignored directories (makes sense only when run on IDF_PATH)
# Note: IGNORE_DIRS is a tuple in order to be able to use it directly with the startswith() built-in function which
# accepts tuples but no lists.
IGNORE_DIRS = (
)


def _parse_path(path, sep=' '):
    ret = set()
    with open(path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line.startswith('#') and len(line) > 0:
                ret.add(line.split(sep)[0])
    return ret


def _valid_directory(path):
    if not os.path.isdir(path):
        raise argparse.ArgumentTypeError("{} is not a valid directory!".format(path))
    return path


def main():
    default_path = os.getenv('IDF_PATH', None)

    parser = argparse.ArgumentParser(description='Kconfig options checker')
    parser.add_argument('--directory', '-d', help='Path to directory to check recursively  '
                        '(for example $IDF_PATH)',
                        type=_valid_directory,
                        required=default_path is None,
                        default=default_path)
    args = parser.parse_args()

    # IGNORE_DIRS makes sense when the required directory is IDF_PATH
    check_ignore_dirs = default_path is not None and os.path.abspath(args.directory) == os.path.abspath(default_path)

    ignores = 0
    files_to_check = []
    deprecated_options = set()
    errors = []

    for root, dirnames, filenames in os.walk(args.directory):
        for filename in filenames:
            full_path = os.path.join(root, filename)
            path_in_idf = os.path.relpath(full_path, args.directory)

            if filename.startswith(FILES_TO_CHECK):
                if check_ignore_dirs and path_in_idf.startswith(IGNORE_DIRS):
                    print('{}: Ignored'.format(path_in_idf))
                    ignores += 1
                    continue
                files_to_check.append(full_path)
            elif filename == 'sdkconfig.rename':
                deprecated_options |= _parse_path(full_path)

    for path in files_to_check:
        used_options = _parse_path(path, '=')
        used_deprecated_options = deprecated_options & used_options
        if len(used_deprecated_options) > 0:
            errors.append('{}: The following options are deprecated: {}'.format(path,
                                                                                ', '.join(used_deprecated_options)))

    if ignores > 0:
        print('{} files have been ignored.'.format(ignores))

    if len(errors) > 0:
        print('\n\n'.join(errors))
        sys.exit(1)


if __name__ == "__main__":
    main()
