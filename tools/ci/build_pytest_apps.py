# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""
This file is used to generate binary files for the given path.
"""

import argparse
import copy
import logging
import os
import sys
from collections import defaultdict
from typing import List

from idf_ci_utils import IDF_PATH, get_pytest_cases

try:
    from build_apps import build_apps
    from find_apps import find_builds_for_app
    from find_build_apps import BuildItem, config_rules_from_str, setup_logging
except ImportError:
    sys.path.append(os.path.join(IDF_PATH, 'tools'))

    from build_apps import build_apps
    from find_apps import find_builds_for_app
    from find_build_apps import BuildItem, config_rules_from_str, setup_logging


def main(args: argparse.Namespace) -> None:
    pytest_cases = []
    for path in args.paths:
        pytest_cases += get_pytest_cases(path, args.target)

    paths = set()
    app_configs = defaultdict(set)
    for case in pytest_cases:
        paths.add(case.app_path)
        app_configs[case.app_path].add(case.config)

    app_dirs = list(paths)
    if not app_dirs:
        raise RuntimeError('No apps found')

    logging.info(f'Found {len(app_dirs)} apps')
    app_dirs.sort()

    # Find compatible configurations of each app, collect them as BuildItems
    build_items: List[BuildItem] = []
    config_rules = config_rules_from_str(args.config or [])
    for app_dir in app_dirs:
        app_dir = os.path.realpath(app_dir)
        build_items += find_builds_for_app(
            app_path=app_dir,
            work_dir=app_dir,
            build_dir='build_@t_@w',
            build_log=f'{app_dir}/build_@t_@w/build.log',
            target_arg=args.target,
            build_system='cmake',
            config_rules=config_rules,
        )

    modified_build_items = []
    # auto clean up the binaries if no flag --preserve-all
    for item in build_items:
        is_test_related = item.config_name in app_configs[item.app_dir]
        if args.test_only and not is_test_related:
            logging.info(f'Skipping non-test app: {item}')
            continue

        copied_item = copy.deepcopy(item)
        if not args.preserve_all and not is_test_related:
            copied_item.preserve = False
        modified_build_items.append(copied_item)

    logging.info(f'Found {len(modified_build_items)} builds')
    modified_build_items.sort(key=lambda x: x.build_path)  # type: ignore

    build_apps(
        build_items=modified_build_items,
        parallel_count=args.parallel_count,
        parallel_index=args.parallel_index,
        dry_run=False,
        build_verbose=args.build_verbose,
        keep_going=True,
        output_build_list=None,
        size_info=args.size_info,
    )


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Build all the pytest apps under specified paths. Will auto remove those non-test apps binaries'
    )
    parser.add_argument('--target', required=True, help='Build apps for given target.')
    parser.add_argument(
        '--config',
        default=['sdkconfig.ci=default', 'sdkconfig.ci.*=', '=default'],
        action='append',
        help='Adds configurations (sdkconfig file names) to build. This can either be '
        + 'FILENAME[=NAME] or FILEPATTERN. FILENAME is the name of the sdkconfig file, '
        + 'relative to the project directory, to be used. Optional NAME can be specified, '
        + 'which can be used as a name of this configuration. FILEPATTERN is the name of '
        + 'the sdkconfig file, relative to the project directory, with at most one wildcard. '
        + 'The part captured by the wildcard is used as the name of the configuration.',
    )
    parser.add_argument(
        'paths',
        nargs='+',
        help='One or more app paths. Will use the current path if not specified.',
    )
    parser.add_argument(
        '--parallel-count', default=1, type=int, help='Number of parallel build jobs.'
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
        help='If specified, the test case name and size info json will be written to this file',
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
    parser.add_argument(
        '--preserve-all',
        action='store_true',
        help='Preserve the binaries for all apps when specified.',
    )
    parser.add_argument(
        '--test-only',
        action='store_true',
        help='Build only test related app when specified.',
    )
    arguments = parser.parse_args()
    setup_logging(arguments)
    main(arguments)
