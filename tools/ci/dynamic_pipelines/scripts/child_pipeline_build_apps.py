# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import sys

import __init__  # noqa: F401 # inject the system path
from dynamic_pipelines.constants import TEST_RELATED_APPS_FILENAME
from idf_build_apps import build_apps
from idf_build_apps import setup_logging
from idf_build_apps.utils import semicolon_separated_str_to_list
from idf_ci.app import import_apps_from_txt
from idf_pytest.constants import DEFAULT_IGNORE_WARNING_FILEPATH

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Build Apps for Dynamic Pipeline')
    parser.add_argument('app_list_file', default=TEST_RELATED_APPS_FILENAME, help='List of apps to build')
    parser.add_argument(
        '--build-verbose',
        action='store_true',
        help='Enable verbose output from build system.',
    )
    parser.add_argument('--parallel-count', default=1, type=int, help='Number of parallel build jobs.')
    parser.add_argument(
        '--parallel-index',
        default=1,
        type=int,
        help='Index (1-based) of the job, out of the number specified by --parallel-count.',
    )
    parser.add_argument(
        '--ignore-warning-file',
        default=DEFAULT_IGNORE_WARNING_FILEPATH,
        type=argparse.FileType('r'),
        help='Ignore the warning strings in the specified file. Each line should be a regex string.',
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
        '--collect-app-info',
        default='list_job_@p.txt',
        help='If specified, the test case name and app info json will be written to this file',
    )
    parser.add_argument(
        '--junitxml',
        default='build_summary_@p.xml',
        help='Path to the junitxml file. If specified, the junitxml file will be generated',
    )

    args = parser.parse_args()

    setup_logging(verbose=1)

    sys.exit(
        build_apps(
            import_apps_from_txt(args.app_list_file),
            build_verbose=args.build_verbose,
            keep_going=True,
            ignore_warning_file=args.ignore_warning_file,
            modified_components=args.modified_components,
            check_app_dependencies=True,
            parallel_count=args.parallel_count,
            parallel_index=args.parallel_index,
            collect_size_info='size_info_@p.txt',
            collect_app_info=args.collect_app_info,
            junitxml=args.junitxml,
            copy_sdkconfig=True,
        )
    )
