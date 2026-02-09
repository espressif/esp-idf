#!/usr/bin/env python
# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os
import re
import sys
from collections import defaultdict
from pathlib import Path

import yaml
from idf_build_apps import App
from idf_build_apps import find_apps
from idf_build_apps.constants import SUPPORTED_TARGETS
from idf_ci_utils import IDF_PATH
from idf_ci_utils import idf_relpath
from rich.console import Console
from rich.table import Table

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
    'esp32s31': 'ESP32-S31',
    'linux': 'Linux',
}

FORMAL_TO_USUAL = {v: k for k, v in USUAL_TO_FORMAL.items()}


def print_diff_table(
    list1: list[str], list2: list[str], title1: str, title2: str, excluded: list[str] | None = None
) -> None:
    left_set = set(list1)
    right_set = set(list2)

    if left_set == right_set:
        return

    all_items = sorted(left_set.union(right_set))
    left_column = []
    right_column = []

    for item in all_items:
        if item in left_set and item not in right_set:
            if excluded and item in excluded:
                left_column.append(f'[red]{item}[/red] (excluded)')
            else:
                left_column.append(f'[red]{item}[/red]')
            right_column.append('')
        elif item in right_set and item not in left_set:
            if excluded and item in excluded:
                right_column.append(f'[green]{item}[/green] (excluded)')
            else:
                right_column.append(f'[green]{item}[/green]')
            left_column.append('')
        else:
            left_column.append(item)
            right_column.append(item)

    table = Table(show_header=True, header_style='bold magenta')
    table.add_column(title1.strip(), justify='left')
    table.add_column(title2.strip(), justify='left')

    for l_col, r_col in zip(left_column, right_column):
        table.add_row(l_col, r_col)

    console = Console()
    console.print(table)


def get_readme_path(app_dir: str) -> str | None:
    readme_path = os.path.join(app_dir, 'README.md')

    if not os.path.isfile(readme_path):
        readme_path = os.path.join(app_dir, '..', 'README.md')

        if not os.path.isfile(readme_path):
            readme_path = None  # type: ignore

    return readme_path


def generate_new_support_table(targets: list[str]) -> str:
    # extra space here
    table_headers = [f'{USUAL_TO_FORMAL[target]}' for target in targets]
    table_headers = ['Supported Targets'] + table_headers

    res = '| ' + ' | '.join(table_headers) + ' |\n'
    res += '| ' + ' | '.join(['-' * len(item) for item in table_headers]) + ' |'

    return res


def parse_existing_table(app_dir: str) -> tuple[str | None, list[str]]:
    readme_path = get_readme_path(app_dir)
    if not readme_path:
        return None, SUPPORTED_TARGETS

    with open(readme_path, encoding='utf8') as _fr:
        readme_str = _fr.read()

    support_string = SUPPORTED_TARGETS_TABLE_REGEX.findall(readme_str)
    if not support_string:
        return None, SUPPORTED_TARGETS

    # old style
    parts = [part.strip() for part in support_string[0].split('\n', 1)[0].split('|') if part.strip()]
    return support_string[0].strip(), [FORMAL_TO_USUAL[part] for part in parts[1:] if part in FORMAL_TO_USUAL]


def get_grouped_apps(
    paths: list[str],
    exclude_dirs: list[str] | None = None,
) -> dict[str, list[App]]:
    apps = sorted(
        find_apps(
            paths,
            'all',
            exclude_list=exclude_dirs or [],
        )
    )

    grouped_apps: dict[str, list[App]] = defaultdict(list)
    for app in apps:
        grouped_apps[app.app_dir].append(app)

    return grouped_apps


def check_readme(
    paths: list[str],
    exclude_dirs: list[str] | None = None,
) -> None:
    grouped_apps = get_grouped_apps(paths, exclude_dirs)
    exit_code = 0

    for app_dir, apps in grouped_apps.items():
        old_table, old_targets = parse_existing_table(app_dir)
        manifest_targets = sorted(
            {
                target
                for app in apps
                for target in (
                    App.MANIFEST.enable_build_targets(app_dir)
                    + App.MANIFEST.enable_build_targets(app_dir, config_name=app.config_name)
                )
            }
        )

        if sorted(manifest_targets) != sorted(old_targets):
            print(f'Build target MISMATCH!!!: {app_dir}')
            print_diff_table(manifest_targets, old_targets, 'Manifest', 'README')
            print(f'Corresponding manifest file: {App.MANIFEST.most_suitable_rule(app_dir).by_manifest_file}')
            exit_code = 1

        readme_path = get_readme_path(app_dir)
        new_table = generate_new_support_table(manifest_targets)
        # no readme, create a new file
        if not readme_path:
            with open(os.path.join(app_dir, 'README.md'), 'w') as fw:
                fw.write(new_table + '\n')
            print(f'Auto-fixing: Added new README: {os.path.join(app_dir, "README.md")}')
            exit_code = 1
        # has old table, but different string
        elif old_table and old_table != new_table:
            with open(readme_path) as fr:
                readme_str = fr.read()

            with open(readme_path, 'w') as fw:
                fw.write(readme_str.replace(old_table, new_table))
            print(f'Auto-fixing: Updated README: {readme_path}')
            exit_code = 1
        # does not have old table
        elif not old_table:
            with open(readme_path) as fr:
                readme_str = fr.read()

            with open(readme_path, 'w') as fw:
                fw.write(new_table + '\n\n' + readme_str)  # extra new line
            print(f'Auto-fixing: Modified README: {readme_path}')
            exit_code = 1

    if exit_code != 0:
        print('')  # extra new line for readability
        print('Related documentation:')
        print(
            '\t- https://docs.espressif.com/projects/idf-build-apps/en/latest/references/manifest.html#enable-disable-rules'
        )
    sys.exit(exit_code)


def get_grouped_cases(paths: list[str]) -> dict[str, dict[str, set[str]]]:
    """
    return something like this:
    {
         app_dir: {
             'script_paths': {'path/to/script1', 'path/to/script2'},
             'targets': {'esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c2', 'linux'},
         }
    }
    """
    from idf_ci import get_pytest_cases

    pytest_cases = get_pytest_cases(
        paths=paths,
        marker_expr=None,  # don't filter host_test
    )

    grouped_cases = {}
    for case in pytest_cases:
        for pytest_app in case.apps:
            app_dir = idf_relpath(pytest_app.path)
            if app_dir not in grouped_cases:
                grouped_cases[app_dir] = {
                    'script_paths': {case.path},
                    'targets': {pytest_app.target},
                }
            else:
                grouped_cases[app_dir]['script_paths'].add(case.path)
                grouped_cases[app_dir]['targets'].add(pytest_app.target)

    return grouped_cases


def check_test_scripts(
    paths: list[str],
    exclude_dirs: list[str] | None = None,
    *,
    bypass_targets: list[str] | None = None,
) -> None:
    # takes long time, run only in CI
    grouped_apps = get_grouped_apps(paths, exclude_dirs)
    grouped_cases = get_grouped_cases(paths)
    exit_code = 0

    for app_dir, apps in grouped_apps.items():
        if app_dir not in grouped_cases:
            continue

        # manifest defined ones
        manifest_targets = sorted(
            {
                target
                for app in apps
                for target in (
                    App.MANIFEST.enable_test_targets(app_dir)
                    + App.MANIFEST.enable_test_targets(app_dir, config_name=app.config_name)
                )
            }
        )
        actual_targets = sorted(grouped_cases[app_dir]['targets'])

        if manifest_targets == actual_targets:
            continue
        if not ((set(manifest_targets) ^ set(actual_targets)) - set(bypass_targets or [])):
            continue

        print(f'Test target MISMATCH!!!: {app_dir}')
        print_diff_table(manifest_targets, actual_targets, 'Manifest', 'Test Scripts', bypass_targets)
        print(f'Corresponding manifest file: {App.MANIFEST.most_suitable_rule(app_dir).by_manifest_file}')
        print('Corresponding test scripts:')
        for script_path in grouped_cases[app_dir]['script_paths']:
            print('  - ' + script_path)
        exit_code = 1

    if exit_code != 0:
        print('')  # extra new line for readability
        print('Related documentation:')
        print(
            '\t- https://docs.espressif.com/projects/idf-build-apps/en/latest/references/manifest.html#enable-disable-rules'
        )
        print(
            '\t- https://docs.espressif.com/projects/esp-idf/en/latest/esp32/contribute/esp-idf-tests-with-pytest.html'
        )

    sys.exit(exit_code)


if __name__ == '__main__':
    if 'CI_JOB_ID' not in os.environ:
        os.environ['CI_JOB_ID'] = 'fake'  # this is a CI script

    parser = argparse.ArgumentParser(description='ESP-IDF apps build/test checker')
    action = parser.add_subparsers(dest='action')

    readme_parser = action.add_parser('check-readmes')
    readme_parser.add_argument('paths', nargs='+', help='check under paths')
    readme_parser.add_argument(
        '-c',
        '--config',
        default=os.path.join(IDF_PATH, '.gitlab', 'ci', 'default-build-test-rules.yml'),
        help='config file',
    )

    test_parser = action.add_parser('check-test-scripts')
    test_parser.add_argument('paths', nargs='+', help='check under paths')
    test_parser.add_argument(
        '-c',
        '--config',
        default=os.path.join(IDF_PATH, '.gitlab', 'ci', 'default-build-test-rules.yml'),
        help='config file',
    )
    arg = parser.parse_args()

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
            os.path.join(IDF_PATH, 'tools', 'test_build_system', 'build_test_app'),
            os.path.join(IDF_PATH, 'tools', 'test_build_system', 'buildv2_test_app'),
            os.path.join(IDF_PATH, 'tools', 'templates', 'sample_project'),
            os.path.join(IDF_PATH, 'tools', 'cmakev2', 'test'),
        ]
    else:
        _exclude_dirs = [os.path.join(IDF_PATH, 'tools', 'templates', 'sample_project')]

    _bypass_targets: list[str] = []
    if arg.config:
        with open(arg.config) as fr:
            configs = yaml.safe_load(fr)

        if configs:
            _bypass_targets = configs.get('bypass_check_test_targets') or []

    os.environ.update(
        {
            'IDF_PATH': IDF_PATH,
            'INCLUDE_NIGHTLY_RUN': '1',
            'NIGHTLY_RUN': '1',
        }
    )

    if arg.action == 'check-readmes':
        check_readme(
            list(check_dirs),
            _exclude_dirs,
        )
    elif arg.action == 'check-test-scripts':
        check_test_scripts(
            list(check_dirs),
            _exclude_dirs,
            bypass_targets=_bypass_targets,
        )
