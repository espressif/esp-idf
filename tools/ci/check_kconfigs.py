#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import print_function, unicode_literals

import argparse
import os
import re
import subprocess
import sys

from idf_ci_utils import IDF_PATH, get_submodule_dirs

# regular expression for matching Kconfig files
RE_KCONFIG = r'^Kconfig(\.projbuild)?(\.in)?$'

# ignored directories (makes sense only when run on IDF_PATH)
# Note: ignore_dirs is a tuple in order to be able to use it directly with the startswith() built-in function which
# accepts tuples but no lists.
IGNORE_DIRS: tuple = (
    # Kconfigs from submodules need to be ignored:
    os.path.join(IDF_PATH, 'components', 'mqtt', 'esp-mqtt'),
    # Test Kconfigs are also ignored
    os.path.join(IDF_PATH, 'tools', 'ldgen', 'test', 'data'),
    os.path.join(IDF_PATH, 'tools', 'kconfig_new', 'test'),
)

ignore_dirs: tuple = IGNORE_DIRS


def valid_directory(path:str) -> str:
    if not os.path.isdir(path):
        raise argparse.ArgumentTypeError('{} is not a valid directory!'.format(path))
    return str(path)


parser = argparse.ArgumentParser(description='Kconfig style checker')
parser.add_argument(
    'files',
    nargs='*',
    help='Kconfig files to check (full paths separated by space)',
)
parser.add_argument(
    '--exclude-submodules',
    action='store_true',
    help='Exclude submodules',
)
parser.add_argument(
    '--includes',
    '-d',
    nargs='*',
    help='Extra paths for recursively searching Kconfig files. (for example $IDF_PATH)',
    type=valid_directory
)

args, unknown_args = parser.parse_known_args()

# if the deprecated argument '--exclude-submodules' is used
if args.exclude_submodules:
    ignore_dirs = ignore_dirs + tuple(get_submodule_dirs(full_path=True))

files_to_check: list = args.files

# if the deprecated argument '--includes' is used all valid paths are checked for KConfigs
# except IGNORE_DIRS and submodules (if exclude is given)
# paths to KConfig files are passed to esp-idf-kconfig kconfcheck tool
if args.includes:
        for directory in args.includes:
            for root, dirnames, filenames in os.walk(directory):
                for filename in filenames:
                    full_path = os.path.join(root, filename)
                    if full_path.startswith(ignore_dirs):
                        continue
                    if re.search(RE_KCONFIG, filename):
                        files_to_check.append(f'{full_path}')
                    elif re.search(RE_KCONFIG, filename, re.IGNORECASE):
                        # On Windows Kconfig files are working with different cases!
                        print(
                            '{}: Incorrect filename. The case should be "Kconfig"!'.format(
                                full_path
                            )
                        )


if __name__ == '__main__':
    sys.exit(subprocess.run([sys.executable, '-m', 'kconfcheck'] + files_to_check + unknown_args).returncode)
