#!/usr/bin/env python
# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import difflib
import inspect
import os
import re
import sys
import typing as t
from collections import defaultdict
from pathlib import Path

import yaml
from idf_ci_utils import IDF_PATH
from idf_ci_utils import idf_relpath

# | Supported Target | ... |
# | ---------------- | --- |
SUPPORTED_TARGETS_TABLE_REGEX = re.compile(r'^\|\s*Supported Targets.+$\n^\|(?:\s*|-).+$\n?', re.MULTILINE)

USUAL_TO_FORMAL = {
    'esp32': 'ESP32',
    'esp32s2': 'ESP32-S2',
    'esp32s3': 'ESP32-S3',
    'esp32c3': 'ESP32-C3',
    'esp32c2': 'ESP32-C2',
    'esp32c6': 'ESP32-C6',
    'esp32c5': 'ESP32-C5',
    'esp32h2': 'ESP32-H2',
    'esp32p4': 'ESP32-P4',
    'esp32c61': 'ESP32-C61',
    'esp32h21': 'ESP32-H21',
    'esp32h4': 'ESP32-H4',
    'linux': 'Linux',
}

FORMAL_TO_USUAL = {v: k for k, v in USUAL_TO_FORMAL.items()}


def diff_lists(
    list1: t.List[str], list2: t.List[str], title1: str, title2: str, excluded: t.Optional[t.List[str]] = None
) -> None:
    """
    Compare two lists and print the differences.
    """
    diff = difflib.ndiff(list1, list2)
    if not diff:
        return

    print(f'Difference between {title1} and {title2}:')
    for line in diff:
        last_part = line.split(' ', 1)[-1]
        if excluded and last_part in excluded:
            print(line + ' ' + '(excluded)')
        else:
            print(line)


def check_readme(
    paths: t.List[str],
    exclude_dirs: t.Optional[t.List[str]] = None,
    extra_default_build_targets: t.Optional[t.List[str]] = None,
) -> None:
    from idf_build_apps import App
    from idf_build_apps import find_apps
    from idf_build_apps.constants import SUPPORTED_TARGETS

    def get_readme_path(_app_dir: str) -> t.Optional[str]:
        _readme_path = os.path.join(_app_dir, 'README.md')

        if not os.path.isfile(_readme_path):
            _readme_path = os.path.join(_app_dir, '..', 'README.md')

            if not os.path.isfile(_readme_path):
                _readme_path = None  # type: ignore

        return _readme_path

    def _generate_new_support_table_str(_app_dir: str, _manifest_supported_targets: t.List[str]) -> str:
        # extra space here
        table_headers = [f'{USUAL_TO_FORMAL[target]}' for target in _manifest_supported_targets]
        table_headers = ['Supported Targets'] + table_headers

        res = '| ' + ' | '.join(table_headers) + ' |\n'
        res += '| ' + ' | '.join(['-' * len(item) for item in table_headers]) + ' |'

        return res

    def _parse_existing_support_table_str(_app_dir: str) -> t.Tuple[t.Optional[str], t.List[str]]:
        _readme_path = get_readme_path(_app_dir)
        if not _readme_path:
            return None, SUPPORTED_TARGETS

        with open(_readme_path, encoding='utf8') as _fr:
            _readme_str = _fr.read()

        support_string = SUPPORTED_TARGETS_TABLE_REGEX.findall(_readme_str)
        if not support_string:
            return None, SUPPORTED_TARGETS

        # old style
        parts = [part.strip() for part in support_string[0].split('\n', 1)[0].split('|') if part.strip()]
        return support_string[0].strip(), [FORMAL_TO_USUAL[part] for part in parts[1:] if part in FORMAL_TO_USUAL]

    def check_enable_build(
        _app_dir: str, _manifest_supported_targets: t.List[str], _old_supported_targets: t.List[str]
    ) -> bool:
        if _manifest_supported_targets == sorted(_old_supported_targets):
            return True

        _readme_path = get_readme_path(_app_dir)
        diff_lists(
            sorted(_manifest_supported_targets),
            sorted(_old_supported_targets),
            'manifest-enabled targets',
            f'supported targets table in {_readme_path}',
        )
        print(
            inspect.cleandoc(
                f"""
            To enable/disable build targets, please modify your manifest file:
                {App.MANIFEST.most_suitable_rule(app_dir).by_manifest_file}

            Please refer to https://docs.espressif.com/projects/idf-build-apps/en/latest/references/manifest.html#enable-disable-rules
            for more details.
            """
            )
        )

        return False

    apps = sorted(
        find_apps(
            paths,
            'all',
            exclude_list=exclude_dirs or [],
            default_build_targets=SUPPORTED_TARGETS + extra_default_build_targets,
        )
    )
    exit_code = 0
    apps_grouped: t.Dict[str, t.List[App]] = defaultdict(list)
    for app in apps:
        apps_grouped[app.app_dir].append(app)

    for app_dir in apps_grouped:
        replace_str, old_supported_targets = _parse_existing_support_table_str(app_dir)

        # manifest defined ones
        manifest_defined_targets = sorted(
            {
                target
                for app in apps_grouped[app_dir]
                for target in (
                    App.MANIFEST.enable_build_targets(app_dir)
                    + App.MANIFEST.enable_build_targets(app_dir, config_name=app.config_name)
                )
            }
        )

        success = check_enable_build(app_dir, manifest_defined_targets, old_supported_targets)
        if not success:
            print(f'check_enable_build failed for app: {app_dir}')
            print('-' * 80)
            exit_code = 1

        readme_path = get_readme_path(app_dir)
        new_readme_str = _generate_new_support_table_str(app_dir, manifest_defined_targets)
        # no readme, create a new file
        if not readme_path:
            with open(os.path.join(app_dir, 'README.md'), 'w') as fw:
                fw.write(new_readme_str + '\n')
            print(f'Added new README file: {os.path.join(app_dir, "README.md")}')
            print('-' * 80)
            exit_code = 1
        # has old table, but different string
        elif replace_str and replace_str != new_readme_str:
            with open(readme_path) as fr:
                readme_str = fr.read()

            with open(readme_path, 'w') as fw:
                fw.write(readme_str.replace(replace_str, new_readme_str))
                print(f'Modified README file: {readme_path}')
            print('-' * 80)
            exit_code = 1
        # does not have old table
        elif not replace_str:
            with open(readme_path) as fr:
                readme_str = fr.read()

            with open(readme_path, 'w') as fw:
                fw.write(new_readme_str + '\n\n' + readme_str)  # extra new line

            print(f'Modified README file: {readme_path}')
            print('-' * 80)
            exit_code = 1

    sys.exit(exit_code)


def check_test_scripts(
    paths: t.List[str],
    exclude_dirs: t.Optional[t.List[str]] = None,
    bypass_check_test_targets: t.Optional[t.List[str]] = None,
    extra_default_build_targets: t.Optional[t.List[str]] = None,
) -> None:
    from idf_build_apps import App
    from idf_build_apps import find_apps
    from idf_build_apps.constants import SUPPORTED_TARGETS
    from idf_ci import get_pytest_cases

    # takes long time, run only in CI
    # dict:
    # {
    #      app_dir: {
    #          'script_paths': {'path/to/script1', 'path/to/script2'},
    #          'targets': {'esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c2', 'linux'},
    #      }
    # }
    def check_enable_test(
        _app_dir: str,
        _manifest_verified_targets: t.List[str],
        _pytest_app_dir_targets_dict: t.Dict[str, t.Dict[str, t.Set[str]]],
    ) -> bool:
        if _app_dir in _pytest_app_dir_targets_dict:
            test_script_paths = _pytest_app_dir_targets_dict[_app_dir]['script_paths']
            actual_verified_targets = sorted(set(_pytest_app_dir_targets_dict[_app_dir]['targets']))
        else:
            return True  # no test case

        if _manifest_verified_targets == actual_verified_targets:
            return True
        elif not (set(_manifest_verified_targets) - set(actual_verified_targets + (bypass_check_test_targets or []))):
            return True

        _title2 = 'pytest enabled targets in test scripts: \n'
        for script_path in test_script_paths:
            _title2 += f'  - {script_path}\n'

        diff_lists(
            _manifest_verified_targets,
            actual_verified_targets,
            'manifest-enabled targets',
            _title2.rstrip(),
            excluded=bypass_check_test_targets or [],
        )
        print(
            inspect.cleandoc(
                f"""
            To enable/disable test targets, please modify your manifest file:
                {App.MANIFEST.most_suitable_rule(app_dir).by_manifest_file}

            To understand how to enable/disable test targets, please refer to:
            https://docs.espressif.com/projects/pytest-embedded/en/latest/usages/markers.html#idf-parametrize

            """
            )
        )
        return False

    apps = sorted(
        find_apps(
            paths,
            'all',
            exclude_list=exclude_dirs or [],
            default_build_targets=SUPPORTED_TARGETS + extra_default_build_targets,
        )
    )
    apps_grouped: t.Dict[str, t.List[App]] = defaultdict(list)
    for app in apps:
        apps_grouped[app.app_dir].append(app)

    exit_code = 0

    pytest_cases = get_pytest_cases(
        paths=paths,
        marker_expr=None,  # don't filter host_test
    )

    pytest_app_dir_targets_dict = {}
    for case in pytest_cases:
        for pytest_app in case.apps:
            app_dir = idf_relpath(pytest_app.path)
            if app_dir not in pytest_app_dir_targets_dict:
                pytest_app_dir_targets_dict[app_dir] = {
                    'script_paths': {case.path},
                    'targets': {pytest_app.target},
                }
            else:
                pytest_app_dir_targets_dict[app_dir]['script_paths'].add(case.path)
                pytest_app_dir_targets_dict[app_dir]['targets'].add(pytest_app.target)

    for app_dir in apps_grouped:
        # manifest defined ones
        manifest_defined_targets = sorted(
            {
                target
                for app in apps_grouped[app_dir]
                for target in (
                    App.MANIFEST.enable_test_targets(app_dir)
                    + App.MANIFEST.enable_test_targets(app_dir, config_name=app.config_name)
                )
            }
        )

        success = check_enable_test(app_dir, manifest_defined_targets, pytest_app_dir_targets_dict)
        if not success:
            print(f'check_enable_test failed for app: {app_dir}')
            print('-' * 80)
            exit_code = 1
            continue

    sys.exit(exit_code)


if __name__ == '__main__':
    if 'CI_JOB_ID' not in os.environ:
        os.environ['CI_JOB_ID'] = 'fake'  # this is a CI script

    parser = argparse.ArgumentParser(description='ESP-IDF apps build/test checker')
    action = parser.add_subparsers(dest='action')

    _check_readme = action.add_parser('check-readmes')
    _check_readme.add_argument('paths', nargs='+', help='check under paths')
    _check_readme.add_argument(
        '-c',
        '--config',
        default=os.path.join(IDF_PATH, '.gitlab', 'ci', 'default-build-test-rules.yml'),
        help='default build test rules config file',
    )

    _check_test_scripts = action.add_parser('check-test-scripts')
    _check_test_scripts.add_argument('paths', nargs='+', help='check under paths')
    _check_test_scripts.add_argument(
        '-c',
        '--config',
        default=os.path.join(IDF_PATH, '.gitlab', 'ci', 'default-build-test-rules.yml'),
        help='default build test rules config file',
    )

    _check_exist = action.add_parser('check-exist')

    arg = parser.parse_args()

    # Since this script is executed from the pre-commit hook environment, make sure IDF_PATH is set
    os.environ['IDF_PATH'] = os.path.realpath(os.path.join(os.path.dirname(__file__), '..', '..'))

    check_dirs = set()

    # check if *_caps.h files changed
    check_all = False
    soc_caps_header_files = list((Path(IDF_PATH) / 'components' / 'soc').glob('**/*_caps.h'))
    for p in arg.paths:
        if Path(p).resolve() in soc_caps_header_files:
            check_all = True
            break

        if os.path.isfile(p):
            check_dirs.add(os.path.dirname(p))
        else:
            check_dirs.add(p)

    if 'tools/idf_py_actions/constants.py' in arg.paths or 'tools/ci/check_build_test_rules.py' in arg.paths:
        check_all = True

    if check_all:
        check_dirs = {IDF_PATH}
        _exclude_dirs = [
            os.path.join(IDF_PATH, 'tools', 'unit-test-app'),
            os.path.join(IDF_PATH, 'tools', 'test_build_system', 'build_test_app'),
            os.path.join(IDF_PATH, 'tools', 'templates', 'sample_project'),
        ]
    else:
        _exclude_dirs = [os.path.join(IDF_PATH, 'tools', 'templates', 'sample_project')]

    extra_default_build_targets_list: t.List[str] = []
    bypass_check_test_targets_list: t.List[str] = []
    if arg.config:
        with open(arg.config) as fr:
            configs = yaml.safe_load(fr)

        if configs:
            extra_default_build_targets_list = configs.get('extra_default_build_targets') or []
            bypass_check_test_targets_list = configs.get('bypass_check_test_targets') or []

    if arg.action == 'check-readmes':
        os.environ['INCLUDE_NIGHTLY_RUN'] = '1'
        os.environ['NIGHTLY_RUN'] = '1'
        check_readme(
            list(check_dirs),
            exclude_dirs=_exclude_dirs,
            extra_default_build_targets=extra_default_build_targets_list,
        )
    elif arg.action == 'check-test-scripts':
        os.environ['INCLUDE_NIGHTLY_RUN'] = '1'
        os.environ['NIGHTLY_RUN'] = '1'
        check_test_scripts(
            list(check_dirs),
            exclude_dirs=_exclude_dirs,
            bypass_check_test_targets=bypass_check_test_targets_list,
            extra_default_build_targets=extra_default_build_targets_list,
        )
