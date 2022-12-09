#!/usr/bin/env python
#
# Copyright 2019 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import print_function
from __future__ import unicode_literals
from io import open
import argparse
import json
import sys


def _prepare_source_files(env_dict):
    """
    Prepares source files which are sourced from the main Kconfig because upstream kconfiglib doesn't support sourcing
    a file list. The inputs are the same environment variables which are used by kconfiglib:
        - COMPONENT_KCONFIGS,
        - COMPONENT_KCONFIGS_SOURCE_FILE,
        - COMPONENT_KCONFIGS_PROJBUILD,
        - COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE.

    The outputs are written into files pointed by the value of
        - COMPONENT_KCONFIGS_SOURCE_FILE,
        - COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE,

    After running this function, COMPONENT_KCONFIGS_SOURCE_FILE and COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE will
    contain a list of source statements based on the content of COMPONENT_KCONFIGS and COMPONENT_KCONFIGS_PROJBUILD,
    respectively. For example, if COMPONENT_KCONFIGS="var1 var2 var3" and
    COMPONENT_KCONFIGS_SOURCE_FILE="/path/file.txt" then the content of file /path/file.txt will be:
        source "var1"
        source "var2"
        source "var3"
    """

    def _dequote(var):
        return var[1:-1] if len(var) > 0 and (var[0], var[-1]) == ('"',) * 2 else var

    def _write_source_file(config_var, config_file):
        new_content = '\n'.join(['source "{}"'.format(path) for path in _dequote(config_var).split()])
        try:
            with open(config_file, 'r', encoding='utf-8') as f:
                old_content = f.read()
        except Exception:
            # File doesn't exist or other issue
            old_content = None
            # "None" ensures that it won't be equal to new_content when it is empty string because files need to be
            # created for empty environment variables as well

        if new_content != old_content:
            # write or rewrite file only if it is necessary
            with open(config_file, 'w', encoding='utf-8') as f:
                f.write(new_content)

    try:
        _write_source_file(env_dict['COMPONENT_KCONFIGS'], env_dict['COMPONENT_KCONFIGS_SOURCE_FILE'])
        _write_source_file(env_dict['COMPONENT_KCONFIGS_PROJBUILD'], env_dict['COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE'])
    except KeyError as e:
        print('Error:', e, 'is not defined!')
        sys.exit(1)


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='Kconfig Source File Generator')

    parser.add_argument('--env', action='append', default=[],
                        help='Environment value', metavar='NAME=VAL')

    parser.add_argument('--env-file', type=argparse.FileType('r'),
                        help='Optional file to load environment variables from. Contents '
                             'should be a JSON object where each key/value pair is a variable.')

    args = parser.parse_args()

    try:
        env = dict([(name, value) for (name, value) in (e.split("=", 1) for e in args.env)])
    except ValueError:
        print("--env arguments must each contain =.")
        sys.exit(1)

    if args.env_file is not None:
        env.update(json.load(args.env_file))

    _prepare_source_files(env)
