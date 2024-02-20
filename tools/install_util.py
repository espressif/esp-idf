#!/usr/bin/env python

# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0

# This script is used from the $IDF_PATH/install.* scripts. This way the argument parsing can be done at one place and
# doesn't have to be implemented for all shells.

import argparse
import json
import os
import sys
from itertools import chain


def action_extract_features(args: str) -> None:
    """
    Command line arguments starting with "--enable-" or "--disable" are features. This function selects those, add them signs '+' or '-' and prints them.
    """
    features = ['+core']  # "core" features should be always installed

    if args:
        arg_enable_prefix = '--enable-'
        arg_disable_prefix = '--disable-'
        # features to be enabled has prefix '+', disabled has prefix '-'
        for arg in args.split():
            if arg.startswith(arg_enable_prefix):
                features.append('+' + arg[len(arg_enable_prefix):])
            elif arg.startswith(arg_disable_prefix):
                features.append('-' + arg[len(arg_disable_prefix):])
        features = list(set(features))

    print(','.join(features))


def action_extract_targets(args: str) -> None:
    """
    Command line arguments starting with "esp" are chip targets. This function selects those and prints them.
    """
    target_sep = ','
    targets = []

    if args:
        target_args = (arg for arg in args.split() if arg.lower().startswith('esp'))
        # target_args can be comma-separated lists of chip targets

        targets = list(chain.from_iterable(commalist.split(target_sep) for commalist in target_args))

    print(target_sep.join(targets or ['all']))


def action_print_help(script_extension: str) -> None:
    """
    This function prints a help message explaining how to use the install scripts.
    """

    # extract the list of features from ./requirements.json
    thisdir = os.path.dirname(os.path.realpath(__file__))
    with open(f'{thisdir}/requirements.json', 'r') as f:
        json_data = json.load(f)
    features = [feat['name'] for feat in json_data['features']]

    if script_extension == 'bat':
        help_opts = '/?, -h, --help'
    elif script_extension == 'ps1':
        help_opts = '-h            '
    else:
        help_opts = '-h, --help    '

    print(f"""usage: .{os.path.sep}install.{script_extension} [-h] [targets-to-install] [--enable-*] [--disable-*]

optional arguments:
  targets-to-install  'all', a single target (e.g. 'esp32s2'), or a comma-separated list of targets (e.g. 'esp32,esp32c3,esp32h2')
  --enable-*          a specific feature to enable (e.g. '--enable-pytest' will enable feature pytest)
  --disable-*         a specific feature to disable (e.g. '--disable-pytest' will disable feature pytest)
                      supported features: {', '.join(features)}
  {help_opts}      show this help message and exit

For more information, please see https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/tools/idf-tools.html#install-scripts
  """)


def main() -> None:
    parser = argparse.ArgumentParser()

    subparsers = parser.add_subparsers(dest='action', required=True)
    extract = subparsers.add_parser('extract', help='Process arguments and extract part of it')

    extract.add_argument('type', choices=['targets', 'features'])
    extract.add_argument('str-to-parse', nargs='?')

    if 'print_help' in sys.argv:  # keep this option hidden until used
        print_help = subparsers.add_parser('print_help', help='Show install script help')
        print_help.add_argument('extension', choices=['sh', 'fish', 'ps1', 'bat'])

    args, unknown_args = parser.parse_known_args()
    # standalone "--enable-" won't be included into str-to-parse

    if args.action == 'print_help':
        action_print_help(args.extension)
        sys.exit(0)

    action_func = globals()['action_{}_{}'.format(args.action, args.type)]
    str_to_parse = vars(args)['str-to-parse'] or ''
    action_func(' '.join(chain([str_to_parse], unknown_args)))


if __name__ == '__main__':
    main()
