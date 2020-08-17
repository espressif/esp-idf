import argparse
import errno
import json
import logging
import os
import re
from collections import defaultdict

from find_apps import find_apps
from find_build_apps import BUILD_SYSTEMS, BUILD_SYSTEM_CMAKE
from ttfw_idf.CIAssignExampleTest import CIExampleAssignTest, TestAppsGroup, ExampleGroup

VALID_TARGETS = [
    'esp32',
    'esp32s2',
]

TEST_LABELS = {
    'example_test': 'BOT_LABEL_EXAMPLE_TEST',
    'test_apps': 'BOT_LABEL_CUSTOM_TEST',
}

BUILD_ALL_LABELS = [
    'BOT_LABEL_BUILD_ALL_APPS',
    'BOT_LABEL_REGULAR_TEST',
]


def _has_build_all_label():
    for label in BUILD_ALL_LABELS:
        if os.getenv(label):
            return True
    return False


def _judge_build_or_not(action, build_all):  # type: (str, bool) -> (bool, bool)
    """
    :return: (build_or_not_for_test_related_apps, build_or_not_for_non_related_apps)
    """
    if build_all or _has_build_all_label() or (not os.getenv('BOT_TRIGGER_WITH_LABEL')):
        logging.info('Build all apps')
        return True, True

    if os.getenv(TEST_LABELS[action]):
        logging.info('Build test cases apps')
        return True, False
    else:
        logging.info('Skip all')
        return False, False


def output_json(apps_dict_list, target, build_system, output_dir):
    output_path = os.path.join(output_dir, 'scan_{}_{}.json'.format(target.lower(), build_system))
    with open(output_path, 'w') as fw:
        fw.writelines([json.dumps(app) + '\n' for app in apps_dict_list])


def main():
    parser = argparse.ArgumentParser(description='Scan the required build tests')
    parser.add_argument('test_type',
                        choices=TEST_LABELS.keys(),
                        help='Scan test type')
    parser.add_argument('paths',
                        nargs='+',
                        help='One or more app paths')
    parser.add_argument('-b', '--build-system',
                        choices=BUILD_SYSTEMS.keys(),
                        default=BUILD_SYSTEM_CMAKE)
    parser.add_argument('-c', '--ci-config-file',
                        required=True,
                        help="gitlab ci config target-test file")
    parser.add_argument('-o', '--output-path',
                        required=True,
                        help="output path of the scan result")
    parser.add_argument("--exclude",
                        action="append",
                        help='Ignore specified directory. Can be used multiple times.')
    parser.add_argument('--preserve', action="store_true",
                        help='add this flag to preserve artifacts for all apps')
    parser.add_argument('--build-all', action="store_true",
                        help='add this flag to build all apps')

    args = parser.parse_args()
    build_test_case_apps, build_standalone_apps = _judge_build_or_not(args.test_type, args.build_all)

    if not os.path.exists(args.output_path):
        try:
            os.makedirs(args.output_path)
        except OSError as e:
            if e.errno != errno.EEXIST:
                raise e

    if (not build_standalone_apps) and (not build_test_case_apps):
        for target in VALID_TARGETS:
            output_json([], target, args.build_system, args.output_path)
            SystemExit(0)

    test_cases = []
    for path in set(args.paths):
        if args.test_type == 'example_test':
            assign = CIExampleAssignTest(path, args.ci_config_file, ExampleGroup)
        elif args.test_type == 'test_apps':
            CIExampleAssignTest.CI_TEST_JOB_PATTERN = re.compile(r'^test_app_test_.+')
            assign = CIExampleAssignTest(path, args.ci_config_file, TestAppsGroup)
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
    scan_info_dict = defaultdict(dict)
    # store the test cases dir, exclude these folders when scan for standalone apps
    default_exclude = args.exclude if args.exclude else []
    exclude_apps = default_exclude

    build_system = args.build_system.lower()
    build_system_class = BUILD_SYSTEMS[build_system]

    if build_test_case_apps:
        for target in VALID_TARGETS:
            target_dict = scan_info_dict[target]
            test_case_apps = target_dict['test_case_apps'] = set()
            for case in test_cases:
                app_dir = case.case_info['app_dir']
                app_target = case.case_info['target']
                if app_target.lower() != target.lower():
                    continue
                test_case_apps.update(find_apps(build_system_class, app_dir, True, default_exclude, target.lower()))
                exclude_apps.append(app_dir)
    else:
        for target in VALID_TARGETS:
            scan_info_dict[target]['test_case_apps'] = set()

    if build_standalone_apps:
        for target in VALID_TARGETS:
            target_dict = scan_info_dict[target]
            standalone_apps = target_dict['standalone_apps'] = set()
            for path in args.paths:
                standalone_apps.update(find_apps(build_system_class, path, True, exclude_apps, target.lower()))
    else:
        for target in VALID_TARGETS:
            scan_info_dict[target]['standalone_apps'] = set()

    test_case_apps_preserve_default = True if build_system == 'cmake' else False
    for target in VALID_TARGETS:
        apps = []
        for app_dir in scan_info_dict[target]['test_case_apps']:
            apps.append({
                'app_dir': app_dir,
                'build_system': args.build_system,
                'target': target,
                'preserve': args.preserve or test_case_apps_preserve_default
            })
        for app_dir in scan_info_dict[target]['standalone_apps']:
            apps.append({
                'app_dir': app_dir,
                'build_system': args.build_system,
                'target': target,
                'preserve': args.preserve
            })
        output_path = os.path.join(args.output_path, 'scan_{}_{}.json'.format(target.lower(), build_system))
        with open(output_path, 'w') as fw:
            fw.writelines([json.dumps(app) + '\n' for app in apps])


if __name__ == '__main__':
    main()
