#!/usr/bin/env python
# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import inspect
import os
import re
import sys
from pathlib import Path
from typing import Dict
from typing import List
from typing import Optional
from typing import Tuple

import yaml
from idf_ci_utils import get_all_manifest_files
from idf_ci_utils import IDF_PATH

YES = u'\u2713'
NO = u'\u2717'

# | Supported Target | ... |
# | ---------------- | --- |
SUPPORTED_TARGETS_TABLE_REGEX = re.compile(
    r'^\|\s*Supported Targets.+$\n^\|(?:\s*|-).+$\n?', re.MULTILINE
)

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
    'linux': 'Linux',
}

FORMAL_TO_USUAL = {
    'ESP32': 'esp32',
    'ESP32-S2': 'esp32s2',
    'ESP32-S3': 'esp32s3',
    'ESP32-C3': 'esp32c3',
    'ESP32-C2': 'esp32c2',
    'ESP32-C6': 'esp32c6',
    'ESP32-C5': 'esp32c5',
    'ESP32-H2': 'esp32h2',
    'ESP32-P4': 'esp32p4',
    'ESP32-C61': 'esp32c61',
    'Linux': 'linux',
}


def doublequote(s: str) -> str:
    if s.startswith('"') and s.endswith('"'):
        return s

    return f'"{s}"'


def check_readme(
    paths: List[str],
    exclude_dirs: Optional[List[str]] = None,
    extra_default_build_targets: Optional[List[str]] = None,
) -> None:
    from idf_build_apps import App, find_apps
    from idf_build_apps.constants import SUPPORTED_TARGETS

    def get_readme_path(_app: App) -> Optional[str]:
        _readme_path = os.path.join(_app.app_dir, 'README.md')

        if not os.path.isfile(_readme_path):
            _readme_path = os.path.join(_app.app_dir, '..', 'README.md')

            if not os.path.isfile(_readme_path):
                _readme_path = None  # type: ignore

        return _readme_path

    def _generate_new_support_table_str(_app: App) -> str:
        # extra space here
        table_headers = [
            f'{USUAL_TO_FORMAL[target]}' for target in _app.supported_targets
        ]
        table_headers = ['Supported Targets'] + table_headers

        res = '| ' + ' | '.join(table_headers) + ' |\n'
        res += '| ' + ' | '.join(['-' * len(item) for item in table_headers]) + ' |'

        return res

    def _parse_existing_support_table_str(_app: App) -> Tuple[Optional[str], List[str]]:
        _readme_path = get_readme_path(_app)
        if not _readme_path:
            return None, SUPPORTED_TARGETS

        with open(_readme_path, encoding='utf8') as _fr:
            _readme_str = _fr.read()

        support_string = SUPPORTED_TARGETS_TABLE_REGEX.findall(_readme_str)
        if not support_string:
            return None, SUPPORTED_TARGETS

        # old style
        parts = [
            part.strip()
            for part in support_string[0].split('\n', 1)[0].split('|')
            if part.strip()
        ]
        return support_string[0].strip(), [FORMAL_TO_USUAL[part] for part in parts[1:] if part in FORMAL_TO_USUAL]

    def check_enable_build(_app: App, _old_supported_targets: List[str]) -> bool:
        if _app.supported_targets == sorted(_old_supported_targets):
            return True

        _readme_path = get_readme_path(_app)
        if_clause = f'IDF_TARGET in [{", ".join([doublequote(target) for target in sorted(_old_supported_targets)])}]'

        print(
            inspect.cleandoc(
                f'''
            {_app.app_dir}:
            - enable build targets according to the manifest file: {_app.supported_targets}
            - enable build targets according to the old Supported Targets table under readme "{_readme_path}": {_old_supported_targets}

            If you want to disable some targets, please use the following snippet:

            # Please combine this with the original one
            #
            # Notes:
            # - please keep in mind to avoid duplicated folders as yaml keys
            # - please use parentheses to group conditions, the "or" and "and" operators could only accept two operands
            {_app.app_dir}:
              enable:
                - if: {if_clause}
                  temporary: true
                  reason: <why only enable build jobs for these targets>

            '''
            )
        )

        return False

    apps = sorted(
        find_apps(
            paths,
            'all',
            recursive=True,
            exclude_list=exclude_dirs or [],
            manifest_files=get_all_manifest_files(),
            default_build_targets=SUPPORTED_TARGETS + extra_default_build_targets,
        )
    )
    exit_code = 0

    checked_app_dirs = set()
    for app in apps:
        if app.app_dir not in checked_app_dirs:
            checked_app_dirs.add(app.app_dir)
        else:
            continue

        replace_str, old_supported_targets = _parse_existing_support_table_str(app)
        success = check_enable_build(app, old_supported_targets)
        if not success:
            print(f'check_enable_build failed for app: {app}')
            print('-' * 80)
            exit_code = 1

        readme_path = get_readme_path(app)
        # no readme, create a new file
        if not readme_path:
            with open(os.path.join(app.app_dir, 'README.md'), 'w') as fw:
                fw.write(_generate_new_support_table_str(app) + '\n')
            print(f'Added new README file: {os.path.join(app.app_dir, "README.md")}')
            print('-' * 80)
            exit_code = 1
        # has old table, but different string
        elif replace_str and replace_str != _generate_new_support_table_str(app):
            with open(readme_path) as fr:
                readme_str = fr.read()

            with open(readme_path, 'w') as fw:
                fw.write(
                    readme_str.replace(
                        replace_str, _generate_new_support_table_str(app)
                    )
                )
                print(f'Modified README file: {readme_path}')
            print('-' * 80)
            exit_code = 1
        # does not have old table
        elif not replace_str:
            with open(readme_path) as fr:
                readme_str = fr.read()

            with open(readme_path, 'w') as fw:
                fw.write(
                    _generate_new_support_table_str(app) + '\n\n' + readme_str
                )  # extra new line

            print(f'Modified README file: {readme_path}')
            print('-' * 80)
            exit_code = 1

    sys.exit(exit_code)


def check_test_scripts(
    paths: List[str],
    exclude_dirs: Optional[List[str]] = None,
    bypass_check_test_targets: Optional[List[str]] = None,
    extra_default_build_targets: Optional[List[str]] = None,
) -> None:
    from idf_build_apps import App, find_apps
    from idf_build_apps.constants import SUPPORTED_TARGETS
    from idf_pytest.script import get_pytest_cases

    # takes long time, run only in CI
    # dict:
    # {
    #      app_dir: {
    #          'script_path': 'path/to/script',
    #          'targets': ['esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c2', 'linux'],
    #      }
    # }
    def check_enable_test(
        _app: App,
        _pytest_app_dir_targets_dict: Dict[str, Dict[str, str]],
    ) -> bool:
        if _app.app_dir in _pytest_app_dir_targets_dict:
            test_script_path = _pytest_app_dir_targets_dict[_app.app_dir]['script_path']
            actual_verified_targets = sorted(
                set(_pytest_app_dir_targets_dict[_app.app_dir]['targets'])
            )
        else:
            return True  # no test case

        actual_extra_tested_targets = set(actual_verified_targets) - set(
            _app.verified_targets
        )
        if actual_extra_tested_targets - set(bypass_check_test_targets or []):
            print(
                inspect.cleandoc(
                    f'''
                {_app.app_dir}:
                - enable test targets according to the manifest file: {_app.verified_targets}
                - enable test targets according to the test scripts: {actual_verified_targets}

                test scripts enabled targets should be a subset of the manifest file declared ones.
                Please check the test script: {test_script_path}.

                '''
                )
            )
            return False

        if _app.verified_targets == actual_verified_targets:
            return True
        elif not (set(_app.verified_targets) - set(actual_verified_targets + (bypass_check_test_targets or []))):
            print(f'WARNING: bypass test script check on {_app.app_dir} for targets {bypass_check_test_targets} ')
            return True

        if_clause = f'IDF_TARGET in [{", ".join([doublequote(target) for target in sorted(set(_app.verified_targets) - set(actual_verified_targets))])}]'

        print(
            inspect.cleandoc(
                f'''
            {_app.app_dir}:
            - enable test targets according to the manifest file: {_app.verified_targets}
            - enable test targets according to the test scripts: {actual_verified_targets}

            the test scripts enabled test targets should be the same with the manifest file enabled ones. Please check
            the test script manually: {test_script_path}.

            If you want to enable test targets in the pytest test scripts, please add `@pytest.mark.MISSING_TARGET`
            marker above the test case function.

            If you want to disable the test targets in the manifest file, please modify your manifest file with
            the following code snippet:

            # Please combine this with the original one
            #
            # Notes:
            # - please keep in mind to avoid duplicated folders as yaml keys
            # - please use parentheses to group conditions, the "or" and "and" operators could only accept two operands
            {_app.app_dir}:
              disable_test:
                - if: {if_clause}
                  temporary: true
                  reason: <why you disable this test>

            '''
            )
        )
        return False

    apps = sorted(
        find_apps(
            paths,
            'all',
            recursive=True,
            exclude_list=exclude_dirs or [],
            manifest_files=get_all_manifest_files(),
            default_build_targets=SUPPORTED_TARGETS + extra_default_build_targets,
        )
    )
    exit_code = 0

    pytest_cases = get_pytest_cases(paths)

    pytest_app_dir_targets_dict = {}
    for case in pytest_cases:
        for pytest_app in case.apps:
            app_dir = os.path.relpath(pytest_app.path, IDF_PATH)
            if app_dir not in pytest_app_dir_targets_dict:
                pytest_app_dir_targets_dict[app_dir] = {
                    'script_path': case.path,
                    'targets': [pytest_app.target],
                }
            else:
                pytest_app_dir_targets_dict[app_dir]['targets'].append(
                    pytest_app.target
                )

    checked_app_dirs = set()
    for app in apps:
        if app.app_dir not in checked_app_dirs:
            checked_app_dirs.add(app.app_dir)
        else:
            continue

        success = check_enable_test(
            app, pytest_app_dir_targets_dict
        )
        if not success:
            print(f'check_enable_test failed for app: {app}')
            print('-' * 80)
            exit_code = 1
            continue

    sys.exit(exit_code)


def check_exist() -> None:
    exit_code = 0

    config_files = get_all_manifest_files()
    for file in config_files:
        if 'managed_components' in Path(file).parts:
            continue

        with open(file) as fr:
            configs = yaml.safe_load(fr)
            for path in configs.keys():
                if path.startswith('.'):
                    continue
                if not os.path.isdir(path):
                    print(f'Path \'{path}\' referred in \'{file}\' does not exist!')
                    exit_code = 1

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
    os.environ['IDF_PATH'] = os.path.realpath(
        os.path.join(os.path.dirname(__file__), '..', '..')
    )

    if arg.action == 'check-exist':
        check_exist()
    else:
        check_dirs = set()

        # check if *_caps.h files changed
        check_all = False
        soc_caps_header_files = list(
            (Path(IDF_PATH) / 'components' / 'soc').glob('**/*_caps.h')
        )
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
            _exclude_dirs = [os.path.join(IDF_PATH, 'tools', 'unit-test-app'),
                             os.path.join(IDF_PATH, 'tools', 'test_build_system', 'build_test_app')]
        else:
            _exclude_dirs = []

        extra_default_build_targets_list: List[str] = []
        bypass_check_test_targets_list: List[str] = []
        if arg.config:
            with open(arg.config) as fr:
                configs = yaml.safe_load(fr)

            if configs:
                extra_default_build_targets_list = (
                    configs.get('extra_default_build_targets') or []
                )
                bypass_check_test_targets_list = (
                    configs.get('bypass_check_test_targets') or []
                )

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
