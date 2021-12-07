# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""
This file is used to generate binary files for the given path.
"""

import argparse
import logging
import os
import sys
from typing import List

from idf_ci_utils import IDF_PATH, get_pytest_dirs

try:
    from build_apps import build_apps
    from find_apps import find_apps, find_builds_for_app
    from find_build_apps import BuildItem, CMakeBuildSystem, config_rules_from_str, setup_logging
except ImportError:
    sys.path.append(os.path.join(IDF_PATH, 'tools'))

    from build_apps import build_apps
    from find_apps import find_apps, find_builds_for_app
    from find_build_apps import BuildItem, CMakeBuildSystem, config_rules_from_str, setup_logging


def main(args: argparse.Namespace) -> None:
    if args.all_pytest_apps:
        paths = get_pytest_dirs(IDF_PATH, args.under_dir)
        args.recursive = True
    elif args.paths is None:
        paths = [os.getcwd()]
    else:
        paths = args.paths

    app_dirs = []
    for path in paths:
        app_dirs += find_apps(CMakeBuildSystem, path, args.recursive, [], args.target)
    if not app_dirs:
        logging.error('No apps found')
        sys.exit(1)

    logging.info('Found {} apps'.format(len(app_dirs)))
    app_dirs.sort()

    # Find compatible configurations of each app, collect them as BuildItems
    build_items: List[BuildItem] = []
    config_rules = config_rules_from_str(args.config or [])
    for app_dir in app_dirs:
        app_dir = os.path.realpath(app_dir)
        build_items += find_builds_for_app(
            app_dir,
            app_dir,
            'build_@t_@w',
            f'{app_dir}/build_@t_@w/build.log',
            args.target,
            'cmake',
            config_rules,
            True,
        )
    logging.info('Found {} builds'.format(len(build_items)))
    build_items.sort(key=lambda x: x.build_path)  # type: ignore

    build_apps(build_items, args.parallel_count, args.parallel_index, False, args.build_verbose, True, None,
               args.size_info)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Tool to generate build steps for IDF apps')
    parser.add_argument(
        '--recursive',
        action='store_true',
        help='Look for apps in the specified directories recursively.',
    )
    parser.add_argument('--target', required=True, help='Build apps for given target.')
    parser.add_argument(
        '--config',
        default=['sdkconfig.ci=default', 'sdkconfig.ci.*=', '=default'],
        action='append',
        help='Adds configurations (sdkconfig file names) to build. This can either be ' +
             'FILENAME[=NAME] or FILEPATTERN. FILENAME is the name of the sdkconfig file, ' +
             'relative to the project directory, to be used. Optional NAME can be specified, ' +
             'which can be used as a name of this configuration. FILEPATTERN is the name of ' +
             'the sdkconfig file, relative to the project directory, with at most one wildcard. ' +
             'The part captured by the wildcard is used as the name of the configuration.',
    )
    parser.add_argument(
        '-p', '--paths',
        nargs='*',
        help='One or more app paths. Will use the current path if not specified.'
    )
    parser.add_argument(
        '--all-pytest-apps',
        action='store_true',
        help='Look for all pytest apps. "--paths" would be ignored if specify this flag.'
    )
    parser.add_argument(
        '--under-dir',
        help='Build only the pytest apps under this directory if specified. '
             'Would be ignored if "--all-pytest-apps" is unflagged.'
    )
    parser.add_argument(
        '--parallel-count',
        default=1,
        type=int,
        help='Number of parallel build jobs.'
    )
    parser.add_argument(
        '--parallel-index',
        default=1,
        type=int,
        help='Index (1-based) of the job, out of the number specified by --parallel-count.',
    )
    parser.add_argument(
        '--size-info',
        type=argparse.FileType('a'),
        help='If specified, the test case name and size info json will be written to this file'
    )
    parser.add_argument(
        '-v',
        '--verbose',
        action='count',
        help='Increase the logging level of the script. Can be specified multiple times.',
    )
    parser.add_argument(
        '--build-verbose',
        action='store_true',
        help='Enable verbose output from build system.',
    )
    arguments = parser.parse_args()
    setup_logging(arguments)
    main(arguments)
