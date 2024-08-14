# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""This file is used for generating the child pipeline for build jobs."""
import argparse
import os
import typing as t

import __init__  # noqa: F401 # inject the system path
import yaml
from dynamic_pipelines.constants import DEFAULT_APPS_BUILD_PER_JOB
from dynamic_pipelines.constants import DEFAULT_BUILD_CHILD_PIPELINE_FILEPATH
from dynamic_pipelines.constants import DEFAULT_BUILD_CHILD_PIPELINE_NAME
from dynamic_pipelines.constants import DEFAULT_TEST_PATHS
from dynamic_pipelines.constants import NON_TEST_RELATED_APPS_FILENAME
from dynamic_pipelines.constants import NON_TEST_RELATED_BUILD_JOB_NAME
from dynamic_pipelines.constants import TEST_RELATED_APPS_FILENAME
from dynamic_pipelines.constants import TEST_RELATED_BUILD_JOB_NAME
from dynamic_pipelines.models import BuildJob
from dynamic_pipelines.models import EmptyJob
from dynamic_pipelines.utils import dump_jobs_to_yaml
from idf_build_apps.utils import semicolon_separated_str_to_list
from idf_ci.app import dump_apps_to_txt
from idf_ci_utils import IDF_PATH
from idf_pytest.constants import CollectMode
from idf_pytest.constants import DEFAULT_CONFIG_RULES_STR
from idf_pytest.constants import DEFAULT_FULL_BUILD_TEST_COMPONENTS
from idf_pytest.constants import DEFAULT_FULL_BUILD_TEST_FILEPATTERNS
from idf_pytest.script import get_all_apps


def _separate_str_to_list(s: str) -> t.List[str]:
    """
    Gitlab env file will escape the doublequotes in the env file, so we need to remove them

    For example,

    in pipeline.env file we have

    MR_MODIFIED_COMPONENTS="app1;app2"
    MR_MODIFIED_FILES="main/app1.c;main/app2.c"

    gitlab will load the doublequotes as well, so we need to remove the doublequotes
    """
    return semicolon_separated_str_to_list(s.strip('"'))  # type: ignore


def main(arguments: argparse.Namespace) -> None:
    # load from default build test rules config file
    extra_default_build_targets: t.List[str] = []
    if arguments.default_build_test_rules:
        with open(arguments.default_build_test_rules) as fr:
            configs = yaml.safe_load(fr)

        if configs:
            extra_default_build_targets = configs.get('extra_default_build_targets') or []

    build_jobs = []
    ###########################################
    # special case with -k, ignore other args #
    ###########################################
    if arguments.filter_expr:
        # build only test related apps
        test_related_apps, _ = get_all_apps(
            arguments.paths,
            target=CollectMode.ALL,
            config_rules_str=DEFAULT_CONFIG_RULES_STR,
            filter_expr=arguments.filter_expr,
            marker_expr='not host_test',
            extra_default_build_targets=extra_default_build_targets,
        )
        dump_apps_to_txt(sorted(test_related_apps), TEST_RELATED_APPS_FILENAME)
        print(f'Generate test related apps file {TEST_RELATED_APPS_FILENAME} with {len(test_related_apps)} apps')

        test_apps_build_job = BuildJob(
            name=TEST_RELATED_BUILD_JOB_NAME,
            parallel=len(test_related_apps) // DEFAULT_APPS_BUILD_PER_JOB + 1,
            variables={
                'APP_LIST_FILE': TEST_RELATED_APPS_FILENAME,
            },
        )

        build_jobs.append(test_apps_build_job)
    else:
        #############
        # all cases #
        #############
        test_related_apps, non_test_related_apps = get_all_apps(
            arguments.paths,
            CollectMode.ALL,
            marker_expr='not host_test',
            config_rules_str=DEFAULT_CONFIG_RULES_STR,
            extra_default_build_targets=extra_default_build_targets,
            modified_components=arguments.modified_components,
            modified_files=arguments.modified_files,
            ignore_app_dependencies_components=arguments.ignore_app_dependencies_components,
            ignore_app_dependencies_filepatterns=arguments.ignore_app_dependencies_filepatterns,
        )

        dump_apps_to_txt(sorted(test_related_apps), TEST_RELATED_APPS_FILENAME)
        print(f'Generate test related apps file {TEST_RELATED_APPS_FILENAME} with {len(test_related_apps)} apps')
        dump_apps_to_txt(sorted(non_test_related_apps), NON_TEST_RELATED_APPS_FILENAME)
        print(
            f'Generate non-test related apps file {NON_TEST_RELATED_APPS_FILENAME} with {len(non_test_related_apps)} apps'
        )

        if test_related_apps:
            test_apps_build_job = BuildJob(
                name=TEST_RELATED_BUILD_JOB_NAME,
                parallel=len(test_related_apps) // DEFAULT_APPS_BUILD_PER_JOB + 1,
                variables={
                    'APP_LIST_FILE': TEST_RELATED_APPS_FILENAME,
                },
            )
            build_jobs.append(test_apps_build_job)

        if non_test_related_apps:
            non_test_apps_build_job = BuildJob(
                name=NON_TEST_RELATED_BUILD_JOB_NAME,
                parallel=len(non_test_related_apps) // DEFAULT_APPS_BUILD_PER_JOB + 1,
                variables={
                    'APP_LIST_FILE': NON_TEST_RELATED_APPS_FILENAME,
                },
            )
            build_jobs.append(non_test_apps_build_job)

    if mr_labels := os.getenv('CI_MERGE_REQUEST_LABELS'):
        print(f'MR labels: {mr_labels}')

    # check if there's no jobs
    if not build_jobs:
        print('No apps need to be built. Create one empty job instead')
        build_jobs.append(EmptyJob())
        extra_include_yml = []
    else:
        extra_include_yml = ['tools/ci/dynamic_pipelines/templates/test_child_pipeline.yml']

    dump_jobs_to_yaml(build_jobs, arguments.yaml_output, DEFAULT_BUILD_CHILD_PIPELINE_NAME, extra_include_yml)
    print(f'Generate child pipeline yaml file {arguments.yaml_output} with {sum(j.parallel for j in build_jobs)} jobs')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Generate build child pipeline',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        '-o',
        '--yaml-output',
        default=DEFAULT_BUILD_CHILD_PIPELINE_FILEPATH,
        help='Output YAML path',
    )
    parser.add_argument(
        '-p',
        '--paths',
        nargs='+',
        default=DEFAULT_TEST_PATHS,
        help='Paths to the apps to build.',
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
        '--modified-components',
        type=_separate_str_to_list,
        default=os.getenv('MR_MODIFIED_COMPONENTS'),
        help='semicolon-separated string which specifies the modified components. '
        'app with `depends_components` set in the corresponding manifest files would only be built '
        'if depends on any of the specified components. '
        'If set to "", the value would be considered as None. '
        'If set to ";", the value would be considered as an empty list',
    )
    parser.add_argument(
        '--modified-files',
        type=_separate_str_to_list,
        default=os.getenv('MR_MODIFIED_FILES'),
        help='semicolon-separated string which specifies the modified files. '
        'app with `depends_filepatterns` set in the corresponding manifest files would only be built '
        'if any of the specified file pattern matches any of the specified modified files. '
        'If set to "", the value would be considered as None. '
        'If set to ";", the value would be considered as an empty list',
    )
    parser.add_argument(
        '-ic',
        '--ignore-app-dependencies-components',
        type=_separate_str_to_list,
        help='semicolon-separated string which specifies the modified components used for '
        'ignoring checking the app dependencies. '
        'The `depends_components` and `depends_filepatterns` set in the manifest files will be ignored '
        'when any of the specified components matches any of the modified components. '
        'Must be used together with --modified-components. '
        'If set to "", the value would be considered as None. '
        'If set to ";", the value would be considered as an empty list',
    )
    parser.add_argument(
        '-if',
        '--ignore-app-dependencies-filepatterns',
        type=_separate_str_to_list,
        help='semicolon-separated string which specifies the file patterns used for '
        'ignoring checking the app dependencies. '
        'The `depends_components` and `depends_filepatterns` set in the manifest files will be ignored '
        'when any of the specified file patterns matches any of the modified files. '
        'Must be used together with --modified-files. '
        'If set to "", the value would be considered as None. '
        'If set to ";", the value would be considered as an empty list',
    )

    args = parser.parse_args()

    if test_case_filters := os.getenv('TEST_CASE_FILTERS', None):
        args.filter_expr = test_case_filters

    if os.getenv('IS_MR_PIPELINE') == '0' or os.getenv('BUILD_AND_TEST_ALL_APPS') == '1':
        print('Build and run all test cases, and compile all cmake apps')
        args.modified_components = None
        args.modified_files = None
        args.ignore_app_dependencies_components = None
        args.ignore_app_dependencies_filepatterns = None
    elif args.filter_expr is not None:
        print('Build and run only test cases matching "%s"' % args.filter_expr)
        args.modified_components = None
        args.modified_files = None
        args.ignore_app_dependencies_components = None
        args.ignore_app_dependencies_filepatterns = None
    else:
        print(
            f'Build and run only test cases matching:\n'
            f'- modified components: {args.modified_components}\n'
            f'- modified files: {args.modified_files}'
        )

        if args.modified_components is not None and not args.ignore_app_dependencies_components:
            # setting default values
            args.ignore_app_dependencies_components = DEFAULT_FULL_BUILD_TEST_COMPONENTS

        if args.modified_files is not None and not args.ignore_app_dependencies_filepatterns:
            # setting default values
            args.ignore_app_dependencies_filepatterns = DEFAULT_FULL_BUILD_TEST_FILEPATTERNS

    main(args)
