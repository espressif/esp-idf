# SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""
This file is used in CI generate binary files for different kinds of apps
"""

import argparse
import os
import sys
import typing as t
import unittest
from collections import defaultdict
from pathlib import Path

import yaml
from idf_build_apps import LOGGER, App, build_apps, find_apps, setup_logging
from idf_build_apps.constants import SUPPORTED_TARGETS
from idf_ci_utils import IDF_PATH

CI_ENV_VARS = {
    'EXTRA_CFLAGS': '-Werror -Werror=deprecated-declarations -Werror=unused-variable '
    '-Werror=unused-but-set-variable -Werror=unused-function -Wstrict-prototypes',
    'EXTRA_CXXFLAGS': '-Werror -Werror=deprecated-declarations -Werror=unused-variable '
    '-Werror=unused-but-set-variable -Werror=unused-function',
    'LDGEN_CHECK_MAPPING': '1',
}


def get_pytest_apps(
    paths: t.List[str],
    target: str,
    config_rules_str: t.List[str],
    marker_expr: str,
    filter_expr: str,
    preserve_all: bool = False,
    extra_default_build_targets: t.Optional[t.List[str]] = None,
    modified_components: t.Optional[t.List[str]] = None,
    modified_files: t.Optional[t.List[str]] = None,
    ignore_app_dependencies_filepatterns: t.Optional[t.List[str]] = None,
) -> t.List[App]:
    from idf_pytest.script import get_pytest_cases

    pytest_cases = get_pytest_cases(paths, target, marker_expr, filter_expr)

    _paths: t.Set[str] = set()
    test_related_app_configs = defaultdict(set)
    for case in pytest_cases:
        for app in case.apps:
            _paths.add(app.path)
            test_related_app_configs[app.path].add(app.config)

    if not extra_default_build_targets:
        extra_default_build_targets = []

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
        manifest_rootpath=IDF_PATH,
        manifest_files=[str(p) for p in Path(IDF_PATH).glob('**/.build-test-rules.yml')],
        default_build_targets=SUPPORTED_TARGETS + extra_default_build_targets,
        modified_components=modified_components,
        modified_files=modified_files,
        ignore_app_dependencies_filepatterns=ignore_app_dependencies_filepatterns,
    )

    for app in apps:
        is_test_related = app.config_name in test_related_app_configs[app.app_dir]
        if not preserve_all and not is_test_related:
            app.preserve = False

        if app.target == 'linux':
            app._size_json_path = None  # no esp_idf_size for linux target

    return apps  # type: ignore


def get_cmake_apps(
    paths: t.List[str],
    target: str,
    config_rules_str: t.List[str],
    preserve_all: bool = False,
    extra_default_build_targets: t.Optional[t.List[str]] = None,
    modified_components: t.Optional[t.List[str]] = None,
    modified_files: t.Optional[t.List[str]] = None,
    ignore_app_dependencies_filepatterns: t.Optional[t.List[str]] = None,
) -> t.List[App]:
    from idf_pytest.constants import PytestApp
    from idf_pytest.script import get_pytest_cases

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
        manifest_rootpath=IDF_PATH,
        manifest_files=[str(p) for p in Path(IDF_PATH).glob('**/.build-test-rules.yml')],
        default_build_targets=SUPPORTED_TARGETS + extra_default_build_targets,
        modified_components=modified_components,
        modified_files=modified_files,
        ignore_app_dependencies_filepatterns=ignore_app_dependencies_filepatterns,
    )

    apps_for_build = []
    pytest_cases_apps = [app for case in get_pytest_cases(paths, target) for app in case.apps]
    for app in apps:
        if preserve_all:  # relpath
            app.preserve = True

        if PytestApp(os.path.realpath(app.app_dir), app.target, app.config_name) in pytest_cases_apps:
            LOGGER.debug('Skipping build app with pytest scripts: %s', app)
            continue

        if app.target == 'linux':
            app._size_json_path = None  # no esp_idf_size for linux target

        apps_for_build.append(app)

    return apps_for_build


APPS_BUILD_PER_JOB = 30


def main(args: argparse.Namespace) -> None:
    extra_default_build_targets: t.List[str] = []
    if args.default_build_test_rules:
        with open(args.default_build_test_rules) as fr:
            configs = yaml.safe_load(fr)

        if configs:
            extra_default_build_targets = configs.get('extra_default_build_targets') or []

    if args.pytest_apps:
        LOGGER.info('Only build apps with pytest scripts')
        apps = get_pytest_apps(
            args.paths,
            args.target,
            args.config,
            args.marker_expr,
            args.filter_expr,
            args.preserve_all,
            extra_default_build_targets,
            args.modified_components,
            args.modified_files,
            args.ignore_app_dependencies_filepatterns,
        )
    else:
        LOGGER.info('build apps. will skip pytest apps with pytest scripts')
        apps = get_cmake_apps(
            args.paths,
            args.target,
            args.config,
            args.preserve_all,
            extra_default_build_targets,
            args.modified_components,
            args.modified_files,
            args.ignore_app_dependencies_filepatterns,
        )

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
                if abs_extra_preserve_dir == abs_app_dir or abs_extra_preserve_dir in abs_app_dir.parents:
                    app.preserve = True

    res = build_apps(
        apps,
        parallel_count=args.parallel_count,
        parallel_index=args.parallel_index,
        dry_run=False,
        build_verbose=args.build_verbose,
        keep_going=True,
        collect_size_info='size_info.txt',
        collect_app_info=args.collect_app_info,
        ignore_warning_strs=args.ignore_warning_str,
        ignore_warning_file=args.ignore_warning_file,
        copy_sdkconfig=args.copy_sdkconfig,
        modified_components=args.modified_components,
        modified_files=args.modified_files,
        ignore_app_dependencies_filepatterns=args.ignore_app_dependencies_filepatterns,
    )

    if isinstance(res, tuple):
        sys.exit(res[0])
    else:
        sys.exit(res)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Build all the apps for different test types. Will auto remove those non-test apps binaries',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument('paths', nargs='+', help='Paths to the apps to build.')
    parser.add_argument(
        '-t',
        '--target',
        default='all',
        help='Build apps for given target',
    )
    parser.add_argument(
        '--config',
        default=['sdkconfig.ci=default', 'sdkconfig.ci.*=', '=default'],
        nargs='+',
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
    parser.add_argument('--parallel-count', default=1, type=int, help='Number of parallel build jobs.')
    parser.add_argument(
        '--parallel-index',
        default=1,
        type=int,
        help='Index (1-based) of the job, out of the number specified by --parallel-count.',
    )
    parser.add_argument(
        '--collect-app-info',
        default='list_job_@p.txt',
        help='If specified, the test case name and app info json will be written to this file',
    )
    parser.add_argument(
        '--ignore-warning-str',
        nargs='+',
        help='Ignore the warning string that match the specified regex in the build output. space-separated list',
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
    parser.add_argument(
        '-k',
        '--filter-expr',
        help='only build tests matching given filter expression. For example: -k "test_hello_world". Works only'
        'for pytest',
    )
    parser.add_argument(
        '--default-build-test-rules',
        default=os.path.join(IDF_PATH, '.gitlab', 'ci', 'default-build-test-rules.yml'),
        help='default build test rules config file',
    )
    parser.add_argument(
        '--skip-setting-flags',
        action='store_true',
        help='by default this script would set the build flags exactly the same as the CI ones. '
        'Set this flag to use your local build flags.',
    )
    parser.add_argument(
        '--modified-components',
        nargs='*',
        default=None,
        help='space-separated list which specifies the modified components. app with `depends_components` set in the '
        'corresponding manifest files would only be built if depends on any of the specified components.',
    )
    parser.add_argument(
        '--modified-files',
        nargs='*',
        default=None,
        help='space-separated list which specifies the modified files. app with `depends_filepatterns` set in the '
        'corresponding manifest files would only be built if any of the specified file pattern matches any of the '
        'specified modified files.',
    )
    parser.add_argument(
        '-if',
        '--ignore-app-dependencies-filepatterns',
        nargs='*',
        default=None,
        help='space-separated list which specifies the file patterns used for ignoring checking the app dependencies. '
        'The `depends_components` and `depends_filepatterns` set in the manifest files will be ignored when any of the '
        'specified file patterns matches any of the modified files. Must be used together with --modified-files',
    )

    arguments = parser.parse_args()

    setup_logging(arguments.verbose)

    # skip setting flags in CI
    if not arguments.skip_setting_flags and not os.getenv('CI_JOB_ID'):
        for _k, _v in CI_ENV_VARS.items():
            os.environ[_k] = _v
            LOGGER.info(f'env var {_k} set to "{_v}"')

    if os.getenv('IS_MR_PIPELINE') == '0' or os.getenv('BUILD_AND_TEST_ALL_APPS') == '1':
        # if it's not MR pipeline or env var BUILD_AND_TEST_ALL_APPS=1,
        # remove component dependency related arguments
        if 'modified_components' in arguments:
            arguments.modified_components = None
        if 'modified_files' in arguments:
            arguments.modified_files = None

    # file patterns to tigger full build
    if 'modified_components' in arguments and not arguments.ignore_app_dependencies_filepatterns:
        arguments.ignore_app_dependencies_filepatterns = [
            # tools
            'tools/cmake/**/*',
            'tools/tools.json',
            # components
            'components/cxx/**/*',
            'components/esp_common/**/*',
            'components/esp_hw_support/**/*',
            'components/esp_rom/**/*',
            'components/esp_system/**/*',
            'components/esp_timer/**/*',
            'components/freertos/**/*',
            'components/hal/**/*',
            'components/heap/**/*',
            'components/log/**/*',
            'components/newlib/**/*',
            'components/riscv/**/*',
            'components/soc/**/*',
            'components/xtensa/**/*',
        ]

    main(arguments)


class TestParsingShellScript(unittest.TestCase):
    """
    This test case is run in CI jobs to make sure the CI build flags is the same as the ones recorded in CI_ENV_VARS
    """

    def test_parse_result(self) -> None:
        for k, v in CI_ENV_VARS.items():
            self.assertEqual(os.getenv(k), v)
