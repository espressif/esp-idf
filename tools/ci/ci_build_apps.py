# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
This file is used in CI generate binary files for different kinds of apps
"""
import argparse
import os
import sys
import typing as t
import unittest
from pathlib import Path

import yaml
from dynamic_pipelines.constants import DEFAULT_TEST_PATHS
from idf_build_apps import build_apps
from idf_build_apps import setup_logging
from idf_build_apps.utils import semicolon_separated_str_to_list
from idf_pytest.constants import DEFAULT_BUILD_TEST_RULES_FILEPATH
from idf_pytest.constants import DEFAULT_CONFIG_RULES_STR
from idf_pytest.constants import DEFAULT_FULL_BUILD_TEST_FILEPATTERNS
from idf_pytest.constants import DEFAULT_IGNORE_WARNING_FILEPATH
from idf_pytest.script import get_all_apps

CI_ENV_VARS = {
    'EXTRA_CFLAGS': '-Werror -Werror=deprecated-declarations -Werror=unused-variable '
    '-Werror=unused-but-set-variable -Werror=unused-function -Wstrict-prototypes',
    'EXTRA_CXXFLAGS': '-Werror -Werror=deprecated-declarations -Werror=unused-variable '
    '-Werror=unused-but-set-variable -Werror=unused-function',
    'LDGEN_CHECK_MAPPING': '1',
    'IDF_CI_BUILD': '1',
}


def main(args: argparse.Namespace) -> None:
    extra_default_build_targets: t.List[str] = []
    if args.default_build_test_rules:
        with open(args.default_build_test_rules) as fr:
            configs = yaml.safe_load(fr)

        if configs:
            extra_default_build_targets = configs.get('extra_default_build_targets') or []

    test_related_apps, non_test_related_apps = get_all_apps(
        args.paths,
        args.target,
        config_rules_str=args.config,
        marker_expr=args.marker_expr,
        filter_expr=args.filter_expr,
        preserve_all=args.preserve_all,
        extra_default_build_targets=extra_default_build_targets,
        modified_files=args.modified_components,
        modified_components=args.modified_files,
        ignore_app_dependencies_filepatterns=args.ignore_app_dependencies_filepatterns,
    )

    if args.pytest_apps:
        apps = test_related_apps
    else:
        apps = non_test_related_apps

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
        sorted(apps),
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
        junitxml=args.junitxml,
    )

    sys.exit(res)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Build all the apps for different test types. Will auto remove those non-test apps binaries',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument('paths', nargs='*', help='Paths to the apps to build.')
    parser.add_argument(
        '-t',
        '--target',
        default='all',
        help='Build apps for given target',
    )
    parser.add_argument(
        '--config',
        default=DEFAULT_CONFIG_RULES_STR,
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
        default=DEFAULT_IGNORE_WARNING_FILEPATH,
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
        help='Only build apps required by pytest scripts. '
        'Will build non-test-related apps if this flag is unspecified.',
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
        default=DEFAULT_BUILD_TEST_RULES_FILEPATH,
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
        type=semicolon_separated_str_to_list,
        help='semicolon-separated string which specifies the modified components. '
        'app with `depends_components` set in the corresponding manifest files would only be built '
        'if depends on any of the specified components. '
        'If set to "", the value would be considered as None. '
        'If set to ";", the value would be considered as an empty list',
    )
    parser.add_argument(
        '--modified-files',
        type=semicolon_separated_str_to_list,
        help='semicolon-separated string which specifies the modified files. '
        'app with `depends_filepatterns` set in the corresponding manifest files would only be built '
        'if any of the specified file pattern matches any of the specified modified files. '
        'If set to "", the value would be considered as None. '
        'If set to ";", the value would be considered as an empty list',
    )
    parser.add_argument(
        '-if',
        '--ignore-app-dependencies-filepatterns',
        type=semicolon_separated_str_to_list,
        help='semicolon-separated string which specifies the file patterns used for '
        'ignoring checking the app dependencies. '
        'The `depends_components` and `depends_filepatterns` set in the manifest files '
        'will be ignored when any of the specified file patterns matches any of the modified files. '
        'Must be used together with --modified-files. '
        'If set to "", the value would be considered as None. '
        'If set to ";", the value would be considered as an empty list',
    )
    parser.add_argument(
        '--junitxml',
        default='build_summary_@p.xml',
        help='Path to the junitxml file. If specified, the junitxml file will be generated',
    )

    arguments = parser.parse_args()

    setup_logging(arguments.verbose)

    # set default paths
    if not arguments.paths:
        arguments.paths = DEFAULT_TEST_PATHS

    # skip setting flags in CI
    if not arguments.skip_setting_flags and not os.getenv('CI_JOB_ID'):
        for _k, _v in CI_ENV_VARS.items():
            os.environ[_k] = _v  # type: ignore
            print(f'env var {_k} set to "{_v}"')

    if os.getenv('IS_MR_PIPELINE') == '0' or os.getenv('BUILD_AND_TEST_ALL_APPS') == '1':
        # if it's not MR pipeline or env var BUILD_AND_TEST_ALL_APPS=1,
        # remove component dependency related arguments
        arguments.modified_components = None
        arguments.modified_files = None
        arguments.ignore_app_dependencies_filepatterns = None

    # default file patterns to tigger full build
    if arguments.modified_files is not None and arguments.ignore_app_dependencies_filepatterns is None:
        arguments.ignore_app_dependencies_filepatterns = DEFAULT_FULL_BUILD_TEST_FILEPATTERNS

    main(arguments)


class TestParsingShellScript(unittest.TestCase):
    """
    This test case is run in CI jobs to make sure the CI build flags is the same as the ones recorded in CI_ENV_VARS
    """

    def test_parse_result(self) -> None:
        for k, v in CI_ENV_VARS.items():
            self.assertEqual(os.getenv(k), v)
