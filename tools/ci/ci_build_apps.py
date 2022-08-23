# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""
This file is used in CI generate binary files for different kinds of apps
"""

import argparse
import os
import sys
from collections import defaultdict
from pathlib import Path
from typing import List, Set

from idf_build_apps import LOGGER, App, build_apps, find_apps, setup_logging
from idf_ci_utils import IDF_PATH, get_pytest_app_paths, get_pytest_cases, get_ttfw_app_paths


def get_pytest_apps(
    paths: List[str],
    target: str,
    config_rules_str: List[str],
    marker_expr: str,
    preserve_all: bool = False,
) -> List[App]:
    pytest_cases = get_pytest_cases(paths, target, marker_expr)

    _paths: Set[str] = set()
    test_related_app_configs = defaultdict(set)
    for case in pytest_cases:
        for app in case.apps:
            _paths.add(app.path)

            if os.getenv('INCLUDE_NIGHTLY_RUN') == '1':
                test_related_app_configs[app.path].add(app.config)
            elif os.getenv('NIGHTLY_RUN') == '1':
                if case.nightly_run:
                    test_related_app_configs[app.path].add(app.config)
            else:
                if not case.nightly_run:
                    test_related_app_configs[app.path].add(app.config)

    app_dirs = list(_paths)
    if not app_dirs:
        raise RuntimeError('No apps found')

    LOGGER.info(f'Found {len(app_dirs)} apps')
    app_dirs.sort()

    apps = find_apps(
        app_dirs,
        target=target,
        build_dir='build_@t_@w',
        config_rules_str=config_rules_str,
        build_log_path='build_log.txt',
        size_json_path='size.json',
        check_warnings=True,
        manifest_files=[
            str(p) for p in Path(IDF_PATH).glob('**/.build-test-rules.yml')
        ],
    )

    for app in apps:
        is_test_related = app.config_name in test_related_app_configs[app.app_dir]
        if not preserve_all and not is_test_related:
            app.preserve = False

    return apps  # type: ignore


def get_cmake_apps(
    paths: List[str],
    target: str,
    config_rules_str: List[str],
    preserve_all: bool = False,
) -> List[App]:
    ttfw_app_dirs = get_ttfw_app_paths(paths, target)
    apps = find_apps(
        paths,
        recursive=True,
        target=target,
        build_dir='build_@t_@w',
        config_rules_str=config_rules_str,
        build_log_path='build_log.txt',
        size_json_path='size.json',
        check_warnings=True,
        preserve=False,
        manifest_files=[
            str(p) for p in Path(IDF_PATH).glob('**/.build-test-rules.yml')
        ],
    )

    apps_for_build = []
    pytest_app_dirs = get_pytest_app_paths(paths, target)
    for app in apps:
        if preserve_all or app.app_dir in ttfw_app_dirs:  # relpath
            app.preserve = True

        if os.path.realpath(app.app_dir) in pytest_app_dirs:
            LOGGER.debug('Skipping build app with pytest scripts: %s', app)
            continue

        apps_for_build.append(app)

    return apps_for_build


APPS_BUILD_PER_JOB = 30


def main(args: argparse.Namespace) -> None:
    if args.pytest_apps:
        LOGGER.info('Only build apps with pytest scripts')
        apps = get_pytest_apps(
            args.paths, args.target, args.config, args.marker_expr, args.preserve_all
        )
    else:
        LOGGER.info('build apps. will skip pytest apps with pytest scripts')
        apps = get_cmake_apps(args.paths, args.target, args.config, args.preserve_all)

    LOGGER.info('Found %d apps after filtering', len(apps))
    LOGGER.info(
        'Suggest setting the parallel count to %d for this build job',
        len(apps) // APPS_BUILD_PER_JOB + 1,
    )

    if args.extra_preserve_dirs:
        for app in apps:
            if app.preserve:
                continue
            for extra_preserve_dir in args.extra_preserve_dirs:
                abs_extra_preserve_dir = Path(extra_preserve_dir).resolve()
                abs_app_dir = Path(app.app_dir).resolve()
                if (
                    abs_extra_preserve_dir == abs_app_dir
                    or abs_extra_preserve_dir in abs_app_dir.parents
                ):
                    app.preserve = True

    ret_code = build_apps(
        apps,
        parallel_count=args.parallel_count,
        parallel_index=args.parallel_index,
        dry_run=False,
        build_verbose=args.build_verbose,
        keep_going=True,
        collect_size_info=args.collect_size_info,
        collect_app_info=args.collect_app_info,
        ignore_warning_strs=args.ignore_warning_str,
        ignore_warning_file=args.ignore_warning_file,
        copy_sdkconfig=args.copy_sdkconfig,
    )

    sys.exit(ret_code)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Build all the apps for different test types. Will auto remove those non-test apps binaries',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument('paths', nargs='+', help='Paths to the apps to build.')
    parser.add_argument(
        '-t',
        '--target',
        required=True,
        help='Build apps for given target. could pass "all" to get apps for all targets',
    )
    parser.add_argument(
        '--config',
        default=['sdkconfig.ci=default', 'sdkconfig.ci.*=', '=default'],
        action='append',
        help='Adds configurations (sdkconfig file names) to build. This can either be '
        'FILENAME[=NAME] or FILEPATTERN. FILENAME is the name of the sdkconfig file, '
        'relative to the project directory, to be used. Optional NAME can be specified, '
        'which can be used as a name of this configuration. FILEPATTERN is the name of '
        'the sdkconfig file, relative to the project directory, with at most one wildcard. '
        'The part captured by the wildcard is used as the name of the configuration.',
    )
    parser.add_argument(
        '-v',
        '--verbose',
        action='count',
        help='Increase the LOGGER level of the script. Can be specified multiple times.',
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
        '--parallel-count', default=1, type=int, help='Number of parallel build jobs.'
    )
    parser.add_argument(
        '--parallel-index',
        default=1,
        type=int,
        help='Index (1-based) of the job, out of the number specified by --parallel-count.',
    )
    parser.add_argument(
        '--collect-size-info',
        type=argparse.FileType('w'),
        help='If specified, the test case name and size info json will be written to this file',
    )
    parser.add_argument(
        '--collect-app-info',
        type=argparse.FileType('w'),
        help='If specified, the test case name and app info json will be written to this file',
    )
    parser.add_argument(
        '--ignore-warning-str',
        action='append',
        help='Ignore the warning string that match the specified regex in the build output. '
        'Can be specified multiple times.',
    )
    parser.add_argument(
        '--ignore-warning-file',
        default=os.path.join(IDF_PATH, 'tools', 'ci', 'ignore_build_warnings.txt'),
        type=argparse.FileType('r'),
        help='Ignore the warning strings in the specified file. Each line should be a regex string.',
    )
    parser.add_argument(
        '--copy-sdkconfig',
        action='store_true',
        help='Copy the sdkconfig file to the build directory.',
    )
    parser.add_argument(
        '--extra-preserve-dirs',
        nargs='+',
        help='also preserve binaries of the apps under the specified dirs',
    )

    parser.add_argument(
        '--pytest-apps',
        action='store_true',
        help='Only build apps with pytest scripts. Will build apps without pytest scripts if this flag is unspecified.',
    )
    parser.add_argument(
        '-m',
        '--marker-expr',
        default='not host_test',  # host_test apps would be built and tested under the same job
        help='only build tests matching given mark expression. For example: -m "host_test and generic". Works only'
        'for pytest',
    )

    arguments = parser.parse_args()

    setup_logging(arguments.verbose)
    main(arguments)
