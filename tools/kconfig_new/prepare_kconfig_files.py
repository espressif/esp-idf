#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import print_function, unicode_literals

import argparse
import json
import sys
from io import open


def _prepare_source_files(env_dict, list_separator):
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
    respectively. For example, if COMPONENT_KCONFIGS="var1;var2;var3" and
    COMPONENT_KCONFIGS_SOURCE_FILE="/path/file.txt" then the content of file /path/file.txt will be:
        source "var1"
        source "var2"
        source "var3"

    The character used to delimit paths in COMPONENT_KCONFIGS* variables is set using --list-separator option.
    Space separated lists are currently only used by the documentation build system (esp-docs).
    """

    def _dequote(var):
        return var[1:-1] if len(var) > 0 and (var[0], var[-1]) == ('"',) * 2 else var

    def _write_source_file(config_var, config_file):
        dequoted_var = _dequote(config_var)
        if dequoted_var:
            new_content = '\n'.join(['source "{}"'.format(path) for path in dequoted_var.split(list_separator)])
        else:
            new_content = ''

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


def main():
    parser = argparse.ArgumentParser(description='Kconfig Source File Generator')

    parser.add_argument('--env', action='append', default=[],
                        help='Environment value', metavar='NAME=VAL')

    parser.add_argument('--env-file', type=argparse.FileType('r'),
                        help='Optional file to load environment variables from. Contents '
                             'should be a JSON object where each key/value pair is a variable.')

    parser.add_argument('--list-separator', choices=['space', 'semicolon'],
                        default='space',
                        help='Separator used in environment list variables (COMPONENT_KCONFIGS, COMPONENT_KCONFIGS_PROJBUILD)')

    args = parser.parse_args()

    try:
        env = dict([(name, value) for (name, value) in (e.split('=', 1) for e in args.env)])
    except ValueError:
        print('--env arguments must each contain =.')
        sys.exit(1)

    if args.env_file is not None:
        env.update(json.load(args.env_file))

    list_separator = ';' if args.list_separator == 'semicolon' else ' '

    _prepare_source_files(env, list_separator)


if __name__ == '__main__':
    main()
