# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import fnmatch
import io
import logging
import os.path
import typing as t
from contextlib import redirect_stdout
from pathlib import Path

import pytest
from _pytest.config import ExitCode
from idf_build_apps import App
from idf_build_apps import find_apps
from idf_build_apps.constants import BuildStatus
from idf_build_apps.constants import SUPPORTED_TARGETS
from idf_ci.app import IdfCMakeApp
from idf_ci_utils import get_all_manifest_files
from idf_ci_utils import IDF_PATH
from idf_ci_utils import idf_relpath
from idf_ci_utils import to_list
from idf_py_actions.constants import PREVIEW_TARGETS as TOOLS_PREVIEW_TARGETS
from idf_py_actions.constants import SUPPORTED_TARGETS as TOOLS_SUPPORTED_TARGETS

from .constants import CollectMode
from .constants import DEFAULT_BUILD_LOG_FILENAME
from .constants import DEFAULT_CONFIG_RULES_STR
from .constants import PytestCase
from .plugin import IdfPytestEmbedded


def get_pytest_files(paths: t.List[str]) -> t.List[str]:
    # this is a workaround to solve pytest collector super slow issue
    # benchmark with
    # - time pytest -m esp32 --collect-only
    #   user=15.57s system=1.35s cpu=95% total=17.741
    # - time { find -name 'pytest_*.py'; } | xargs pytest -m esp32 --collect-only
    #   user=0.11s system=0.63s cpu=36% total=2.044
    #   user=1.76s system=0.22s cpu=43% total=4.539
    # use glob.glob would also save a bunch of time
    pytest_scripts: t.Set[str] = set()
    for p in paths:
        path = Path(p)
        pytest_scripts.update(str(_p) for _p in path.glob('**/pytest_*.py') if 'managed_components' not in _p.parts)

    return list(pytest_scripts)


def get_pytest_cases(
    paths: t.Union[str, t.List[str]],
    target: str = CollectMode.ALL,
    *,
    config_name: t.Optional[str] = None,
    marker_expr: t.Optional[str] = None,
    filter_expr: t.Optional[str] = None,
    apps: t.Optional[t.List[App]] = None,
) -> t.List[PytestCase]:
    """
    Return the list of test cases

    For single-dut test cases, `target` could be
    - [TARGET], e.g. `esp32`, to get the test cases for the given target
    - or `single_all`, to get all single-dut test cases

    For multi-dut test cases, `target` could be
    - [TARGET,[TARGET...]], e.g. `esp32,esp32s2`, to get the test cases for the given targets
    - or `multi_all`, to get all multi-dut test cases

    :param paths: paths to search for pytest scripts
    :param target: target or keywords to get test cases for, detailed above
    :param config_name: sdkconfig name
    :param marker_expr: pytest marker expression, `-m`
    :param filter_expr: pytest filter expression, `-k`
    :param apps: built app list, skip the tests required by apps not in the list
    :return: list of test cases
    """
    paths = to_list(paths)

    cases: t.List[PytestCase] = []
    pytest_scripts = get_pytest_files(paths)  # type: ignore
    if not pytest_scripts:
        print(f'WARNING: no pytest scripts found for target {target} under paths {", ".join(paths)}')
        return cases

    def _get_pytest_cases(_target: str, _single_target_duplicate_mode: bool = False) -> t.List[PytestCase]:
        collector = IdfPytestEmbedded(_target, config_name=config_name, single_target_duplicate_mode=_single_target_duplicate_mode, apps=apps)

        with io.StringIO() as buf:
            with redirect_stdout(buf):
                cmd = ['--collect-only', *pytest_scripts, '--target', _target, '-q']
                if marker_expr:
                    cmd.extend(['-m', marker_expr])
                if filter_expr:
                    cmd.extend(['-k', filter_expr])
                res = pytest.main(cmd, plugins=[collector])

            if res.value != ExitCode.OK:
                if res.value == ExitCode.NO_TESTS_COLLECTED:
                    print(f'WARNING: no pytest app found for target {_target} under paths {", ".join(paths)}')
                else:
                    print(buf.getvalue())
                    raise RuntimeError(
                        f'pytest collection failed at {", ".join(paths)} with command \"{" ".join(cmd)}\"'
                    )

        return collector.cases  # type: ignore

    if target == CollectMode.ALL:
        targets = TOOLS_SUPPORTED_TARGETS + TOOLS_PREVIEW_TARGETS + [CollectMode.MULTI_ALL_WITH_PARAM]
    else:
        targets = [target]

    for _target in targets:
        if target == CollectMode.ALL:
            cases.extend(_get_pytest_cases(_target, _single_target_duplicate_mode=True))
        else:
            cases.extend(_get_pytest_cases(_target))

    return sorted(cases, key=lambda x: (x.path, x.name, str(x.targets)))


def get_all_apps(
    paths: t.List[str],
    target: str = CollectMode.ALL,
    *,
    marker_expr: t.Optional[str] = None,
    filter_expr: t.Optional[str] = None,
    config_rules_str: t.Optional[t.List[str]] = None,
    preserve_all: bool = False,
    extra_default_build_targets: t.Optional[t.List[str]] = None,
    modified_components: t.Optional[t.List[str]] = None,
    modified_files: t.Optional[t.List[str]] = None,
    ignore_app_dependencies_filepatterns: t.Optional[t.List[str]] = None,
) -> t.Tuple[t.Set[App], t.Set[App]]:
    """
    Return the tuple of test-required apps and non-test-related apps

    :param paths: paths to search for pytest scripts
    :param target: target or keywords to get test cases for, explained in `get_pytest_cases`
    :param marker_expr: pytest marker expression, `-m`
    :param filter_expr: pytest filter expression, `-k`
    :param config_rules_str: config rules string
    :param preserve_all: preserve all apps
    :param extra_default_build_targets: extra default build targets
    :param modified_components: modified components
    :param modified_files: modified files
    :param ignore_app_dependencies_filepatterns: ignore app dependencies filepatterns
    :return: tuple of test-required apps and non-test-related apps
    """
    # target could be comma separated list
    all_apps: t.List[App] = []
    for _t in set(target.split(',')):
        all_apps.extend(find_apps(
            paths,
            _t,
            build_system=IdfCMakeApp,
            recursive=True,
            build_dir='build_@t_@w',
            config_rules_str=config_rules_str or DEFAULT_CONFIG_RULES_STR,
            build_log_filename=DEFAULT_BUILD_LOG_FILENAME,
            size_json_filename='size.json',
            check_warnings=True,
            manifest_rootpath=IDF_PATH,
            manifest_files=get_all_manifest_files(),
            default_build_targets=SUPPORTED_TARGETS + (extra_default_build_targets or []),
            modified_components=modified_components,
            modified_files=modified_files,
            ignore_app_dependencies_filepatterns=ignore_app_dependencies_filepatterns,
            include_skipped_apps=True,
        ))

    pytest_cases = get_pytest_cases(
        paths,
        target,
        marker_expr=marker_expr,
        filter_expr=filter_expr,
    )

    modified_pytest_cases = []
    if modified_files:
        modified_pytest_scripts = [
            os.path.dirname(f) for f in modified_files if fnmatch.fnmatch(os.path.basename(f), 'pytest_*.py')
        ]
        if modified_pytest_scripts:
            modified_pytest_cases = get_pytest_cases(
                modified_pytest_scripts,
                target,
                marker_expr=marker_expr,
                filter_expr=filter_expr,
            )

    # app_path, target, config
    pytest_app_path_tuple_dict: t.Dict[t.Tuple[str, str, str], PytestCase] = {}
    for case in pytest_cases:
        for app in case.apps:
            pytest_app_path_tuple_dict[(app.path, app.target, app.config)] = case

    modified_pytest_app_path_tuple_dict: t.Dict[t.Tuple[str, str, str], PytestCase] = {}
    for case in modified_pytest_cases:
        for app in case.apps:
            modified_pytest_app_path_tuple_dict[(app.path, app.target, app.config)] = case

    test_related_apps: t.Set[App] = set()
    non_test_related_apps: t.Set[App] = set()
    for app in all_apps:
        # PytestCase.app.path is idf_relpath
        app_path = idf_relpath(app.app_dir)

        # override build_status if test script got modified
        if case := modified_pytest_app_path_tuple_dict.get((app_path, app.target, app.config_name)):
            test_related_apps.add(app)
            app.build_status = BuildStatus.SHOULD_BE_BUILT
            app.preserve = True
            logging.debug('Found app: %s - required by modified test case %s', app, case.path)
        elif app.build_status != BuildStatus.SKIPPED:
            if case := pytest_app_path_tuple_dict.get((app_path, app.target, app.config_name)):
                test_related_apps.add(app)
                # build or not should be decided by the build stage
                app.preserve = True
                logging.debug('Found test-related app: %s - required by %s', app, case.path)
            else:
                non_test_related_apps.add(app)
                app.preserve = preserve_all
                logging.debug('Found non-test-related app: %s', app)

    print(f'Found {len(test_related_apps)} test-related apps')
    print(f'Found {len(non_test_related_apps)} non-test-related apps')

    return test_related_apps, non_test_related_apps
