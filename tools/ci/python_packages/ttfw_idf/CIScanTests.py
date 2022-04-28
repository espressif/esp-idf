import argparse
import errno
import json
import logging
import os
from collections import defaultdict
from copy import deepcopy
from typing import Any

from ci.idf_ci_utils import get_pytest_app_paths
from find_apps import find_apps, find_builds_for_app
from find_build_apps import BUILD_SYSTEM_CMAKE, BUILD_SYSTEMS, config_rules_from_str
from idf_py_actions.constants import PREVIEW_TARGETS, SUPPORTED_TARGETS
from ttfw_idf.IDFAssignTest import ExampleAssignTest, TestAppsAssignTest

TEST_LABELS = {
    'example_test': 'BOT_LABEL_EXAMPLE_TEST',
    'test_apps': 'BOT_LABEL_CUSTOM_TEST',
    'component_ut': ['BOT_LABEL_UNIT_TEST',
                     'BOT_LABEL_UNIT_TEST_32',
                     'BOT_LABEL_UNIT_TEST_S2',
                     'BOT_LABEL_UNIT_TEST_C3'],
}

BUILD_ALL_LABELS = [
    'BOT_LABEL_BUILD',
    'BOT_LABEL_BUILD_ALL_APPS',
    'BOT_LABEL_REGULAR_TEST',
    'BOT_LABEL_WEEKEND_TEST',
    'NIGHTLY_RUN',
    'BOT_LABEL_NIGHTLY_RUN',
]

BUILD_PER_JOB = 30  # each build takes 1 mins around


def _has_build_all_label():  # type: () -> bool
    for label in BUILD_ALL_LABELS:
        if os.getenv(label):
            return True
    return False


def _judge_build_or_not(action, build_all):  # type: (str, bool) -> tuple[bool, bool]
    """
    :return: (build_or_not_for_test_related_apps, build_or_not_for_non_related_apps)
    """
    if build_all or _has_build_all_label() or (not os.getenv('BOT_TRIGGER_WITH_LABEL')):
        logging.info('Build all apps')
        return True, True

    labels = TEST_LABELS[action]
    if not isinstance(labels, list):
        labels = [labels]  # type: ignore

    for label in labels:
        if os.getenv(label):
            logging.info('Build only test cases apps')
            return True, False
    logging.info('Skip all')
    return False, False


def output_json(apps_dict_list, target, build_system, output_dir):  # type: (list, str, str, str) -> None
    output_path = os.path.join(output_dir, 'scan_{}_{}.json'.format(target.lower(), build_system))
    with open(output_path, 'w') as fw:
        fw.writelines([json.dumps(app) + '\n' for app in apps_dict_list])


# we might need artifacts to run test cases locally.
# So we need to save artifacts which have test case not executed by CI.
class _ExampleAssignTest(ExampleAssignTest):
    DEFAULT_FILTER = {}  # type: dict[str, Any]


class _TestAppsAssignTest(TestAppsAssignTest):
    DEFAULT_FILTER = {}  # type: dict[str, Any]


def main():  # type: () -> None
    parser = argparse.ArgumentParser(description='Scan the required build tests')
    parser.add_argument('test_type',
                        choices=TEST_LABELS.keys(),
                        help='Scan test type')
    parser.add_argument('paths', nargs='+',
                        help='One or more app paths')
    parser.add_argument('-b', '--build-system',
                        choices=BUILD_SYSTEMS.keys(),
                        default=BUILD_SYSTEM_CMAKE)
    parser.add_argument('-c', '--ci-config-file',
                        required=True,
                        help='gitlab ci config target-test file')
    parser.add_argument('-o', '--output-path',
                        required=True,
                        help='output path of the scan result')
    parser.add_argument('--exclude', nargs='*',
                        help='Ignore specified directory. Can be used multiple times.')
    parser.add_argument('--extra_test_dirs', nargs='*',
                        help='Additional directories to preserve artifacts for local tests')
    parser.add_argument('--preserve_all', action='store_true',
                        help='add this flag to preserve artifacts for all apps')
    parser.add_argument('--build-all', action='store_true',
                        help='add this flag to build all apps')
    parser.add_argument('--combine-all-targets', action='store_true',
                        help='add this flag to combine all target jsons into one')
    parser.add_argument('--except-targets', nargs='+',
                        help='only useful when "--combine-all-targets". Specified targets would be skipped.')
    parser.add_argument(
        '--config',
        action='append',
        help='Only useful when "--evaluate-parallel-count" is flagged.'
             'Adds configurations (sdkconfig file names) to build. This can either be ' +
             'FILENAME[=NAME] or FILEPATTERN. FILENAME is the name of the sdkconfig file, ' +
             'relative to the project directory, to be used. Optional NAME can be specified, ' +
             'which can be used as a name of this configuration. FILEPATTERN is the name of ' +
             'the sdkconfig file, relative to the project directory, with at most one wildcard. ' +
             'The part captured by the wildcard is used as the name of the configuration.',
    )
    parser.add_argument('--evaluate-parallel-count', action='store_true',
                        help='suggest parallel count according to build items')

    args = parser.parse_args()
    build_test_case_apps, build_standalone_apps = _judge_build_or_not(args.test_type, args.build_all)

    if not os.path.exists(args.output_path):
        try:
            os.makedirs(args.output_path)
        except OSError as e:
            if e.errno != errno.EEXIST:
                raise e

    SUPPORTED_TARGETS.extend(PREVIEW_TARGETS)

    if (not build_standalone_apps) and (not build_test_case_apps):
        for target in SUPPORTED_TARGETS:
            output_json([], target, args.build_system, args.output_path)
            SystemExit(0)

    idf_path = str(os.getenv('IDF_PATH'))
    paths = set([os.path.join(idf_path, path) if not os.path.isabs(path) else path for path in args.paths])

    test_cases = []
    for path in paths:
        if args.test_type == 'example_test':
            assign = _ExampleAssignTest(path, args.ci_config_file)
        elif args.test_type in ['test_apps', 'component_ut']:
            assign = _TestAppsAssignTest(path, args.ci_config_file)
        else:
            raise SystemExit(1)  # which is impossible
        test_cases.extend(assign.search_cases())

    '''
    {
        <target>: {
            'test_case_apps': [<app_dir>],   # which is used in target tests
            'standalone_apps': [<app_dir>],  # which is not
        },
        ...
    }
    '''
    scan_info_dict = defaultdict(dict)  # type: dict[str, dict]
    # store the test cases dir, exclude these folders when scan for standalone apps
    default_exclude = args.exclude if args.exclude else []

    build_system = args.build_system.lower()
    build_system_class = BUILD_SYSTEMS[build_system]

    for target in SUPPORTED_TARGETS:
        exclude_apps = deepcopy(default_exclude)

        if build_test_case_apps:
            scan_info_dict[target]['test_case_apps'] = set()
            test_dirs = args.extra_test_dirs if args.extra_test_dirs else []
            for case in test_cases:
                if case.case_info['target'].lower() == target.lower():
                    test_dirs.append(case.case_info['app_dir'])
            for app_dir in test_dirs:
                app_dir = os.path.join(idf_path, app_dir) if not os.path.isabs(app_dir) else app_dir
                _apps = find_apps(build_system_class, app_dir, True, exclude_apps, target.lower())
                if _apps:
                    scan_info_dict[target]['test_case_apps'].update(_apps)
                    exclude_apps.extend(_apps)
        else:
            scan_info_dict[target]['test_case_apps'] = set()

        if build_standalone_apps:
            scan_info_dict[target]['standalone_apps'] = set()
            for path in paths:
                scan_info_dict[target]['standalone_apps'].update(
                    find_apps(build_system_class, path, True, exclude_apps, target.lower()))
        else:
            scan_info_dict[target]['standalone_apps'] = set()
    test_case_apps_preserve_default = True if build_system == 'cmake' else False
    output_files = []
    build_items_total_count = 0
    for target in SUPPORTED_TARGETS:
        # get pytest apps paths
        pytest_app_paths = set()
        for path in paths:
            pytest_app_paths.update(get_pytest_app_paths(path, target))

        apps = []
        for app_dir in scan_info_dict[target]['test_case_apps']:
            if app_dir in pytest_app_paths:
                print(f'WARNING: has pytest script: {app_dir}')
                continue
            apps.append({
                'app_dir': app_dir,
                'build_system': args.build_system,
                'target': target,
                'preserve': args.preserve_all or test_case_apps_preserve_default
            })
        for app_dir in scan_info_dict[target]['standalone_apps']:
            if app_dir in pytest_app_paths:
                print(f'Skipping pytest app: {app_dir}')
                continue
            apps.append({
                'app_dir': app_dir,
                'build_system': args.build_system,
                'target': target,
                'preserve': args.preserve_all
            })
        output_path = os.path.join(args.output_path, 'scan_{}_{}.json'.format(target.lower(), build_system))
        with open(output_path, 'w') as fw:
            if args.evaluate_parallel_count:
                build_items = []
                config_rules = config_rules_from_str(args.config or [])
                for app in apps:
                    build_items += find_builds_for_app(
                        app['app_dir'],
                        app['app_dir'],
                        'build',
                        '',
                        app['target'],
                        app['build_system'],
                        config_rules,
                        app['preserve'],
                    )
                print('Found {} builds'.format(len(build_items)))
                if args.combine_all_targets:
                    if (args.except_targets and target not in [t.lower() for t in args.except_targets]) \
                            or (not args.except_targets):
                        build_items_total_count += len(build_items)
                else:
                    print(f'suggest set parallel count for target {target} to {len(build_items) // BUILD_PER_JOB + 1}')
            fw.writelines([json.dumps(app) + '\n' for app in apps])

            if args.combine_all_targets:
                if (args.except_targets and target not in [t.lower() for t in args.except_targets]) \
                        or (not args.except_targets):
                    output_files.append(output_path)
                else:
                    print(f'skipping combining target {target}')

    if args.combine_all_targets:
        scan_all_json = os.path.join(args.output_path, f'scan_all_{build_system}.json')
        lines = []
        for file in output_files:
            with open(file) as fr:
                lines.extend([line for line in fr.readlines() if line.strip()])
        with open(scan_all_json, 'w') as fw:
            fw.writelines(lines)
        print(f'combined into file: {scan_all_json}')

        if args.evaluate_parallel_count:
            print(f'Total build: {build_items_total_count}. Suggest set parallel count for all target to {build_items_total_count // BUILD_PER_JOB + 1}')


if __name__ == '__main__':
    main()
