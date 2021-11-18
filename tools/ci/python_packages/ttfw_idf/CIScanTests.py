import argparse
import errno
import json
import logging
import os
from collections import defaultdict
from copy import deepcopy

try:
    from typing import Any
except ImportError:
    # Only used for type annotations
    pass
from find_apps import find_apps
from find_build_apps import BUILD_SYSTEM_CMAKE, BUILD_SYSTEMS
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
]


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
    for target in SUPPORTED_TARGETS:
        apps = []
        for app_dir in scan_info_dict[target]['test_case_apps']:
            apps.append({
                'app_dir': app_dir,
                'build_system': args.build_system,
                'target': target,
                'preserve': args.preserve_all or test_case_apps_preserve_default
            })
        for app_dir in scan_info_dict[target]['standalone_apps']:
            apps.append({
                'app_dir': app_dir,
                'build_system': args.build_system,
                'target': target,
                'preserve': args.preserve_all
            })
        output_path = os.path.join(args.output_path, 'scan_{}_{}.json'.format(target.lower(), build_system))
        with open(output_path, 'w') as fw:
            fw.writelines([json.dumps(app) + '\n' for app in apps])


if __name__ == '__main__':
    main()
