# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import typing as t
from pathlib import Path
from xml.etree import ElementTree as ET

import pytest
import yaml
from _pytest.config import Config
from _pytest.python import Function
from _pytest.runner import CallInfo
from dynamic_pipelines.constants import KNOWN_GENERATE_TEST_CHILD_PIPELINE_WARNINGS_FILEPATH
from idf_ci import IdfPytestPlugin
from idf_ci import PytestCase
from idf_ci.idf_pytest.plugin import IDF_CI_PYTEST_DEBUG_INFO_KEY
from idf_ci_utils import to_list
from pytest_embedded import Dut
from pytest_embedded.utils import find_by_suffix
from pytest_ignore_test_results.ignore_results import ChildCase
from pytest_ignore_test_results.ignore_results import ChildCasesStashKey

from .constants import ECO_MARKERS
from .utils import format_case_id
from .utils import merge_junit_files
from .utils import normalize_testcase_file_path

IDF_LOCAL_PLUGIN_KEY = pytest.StashKey['IdfLocalPlugin']()


def requires_elf_or_map(case: PytestCase) -> bool:
    """
    Determines whether the test case requires elf or map file. By default, one app in the test case
    only requires .bin files.

    :return: True if the test case requires elf or map file, False otherwise
    """
    if 'jtag' in case.env_markers or 'usb_serial_jtag' in case.env_markers:
        return True

    folders_need_elf = ['panic', 'gdbstub_runtime']
    if 'require_elf' in case.all_markers:
        return True

    for folder in folders_need_elf:
        if any(folder in Path(app.path).parts for app in case.apps):
            return True

    return False


def skipped_targets(item: Function) -> set[str]:
    def _get_temp_markers_disabled_targets(marker_name: str) -> set[str]:
        targets = []
        for _m in item.own_markers:
            if _m.name == marker_name:
                if not _m.kwargs.get('targets') or not _m.kwargs.get('reason'):
                    raise ValueError(
                        f'`{marker_name}` should always use keyword arguments `targets` and `reason`. '  # noqa: W604
                        f'For example: '
                        f'`@pytest.mark.{marker_name}(targets=["esp32"], reason="IDF-xxxx, will fix it ASAP")`'
                    )
                targets.extend(to_list(_m.kwargs['targets']))

        if targets:
            return set(targets)
        return set()

    _count = IdfLocalPlugin.get_param(item, 'count', 1)

    def normalize_targets(target: str) -> str:
        targets = target.split(',')
        if len(targets) == 1:
            return ','.join(targets * _count)
        if len(targets) != _count:
            raise ValueError(
                f"Invalid target format: '{target}'. "
                f'Expected a single target or exactly {_count} values separated by commas.'
                f'len({targets}) != {_count}'
            )
        return target

    temp_skip_ci_targets = set(normalize_targets(_t) for _t in _get_temp_markers_disabled_targets('temp_skip_ci'))
    temp_skip_targets = set(normalize_targets(_t) for _t in _get_temp_markers_disabled_targets('temp_skip'))

    # in CI we skip the union of `temp_skip` and `temp_skip_ci`
    if os.getenv('CI_JOB_ID'):
        _skip_targets = temp_skip_ci_targets.union(temp_skip_targets)
    else:  # we use `temp_skip` locally
        _skip_targets = temp_skip_targets

    return _skip_targets


class IdfLocalPlugin:
    UNITY_RESULT_MAPPINGS = {
        'PASS': 'passed',
        'FAIL': 'failed',
        'IGNORE': 'skipped',
    }

    def __init__(self) -> None:
        with open(KNOWN_GENERATE_TEST_CHILD_PIPELINE_WARNINGS_FILEPATH) as fr:
            known_warnings_dict = yaml.safe_load(fr) or dict()

        self.exclude_no_env_markers_test_cases: set[str] = set(known_warnings_dict['no_env_marker_test_cases'])

    @staticmethod
    def get_param(item: Function, key: str, default: t.Any = None) -> t.Any:
        # funcargs is not calculated while collection
        # callspec is something defined in parametrize
        if not hasattr(item, 'callspec'):
            return default

        return item.callspec.params.get(key, default) or default

    @pytest.hookimpl(wrapper=True)
    def pytest_collection_modifyitems(self, config: Config, items: list[Function]) -> t.Generator[None, None, None]:
        yield  # throw it back to idf-ci

        deselected_items = []

        # Filter
        filtered_items = []
        for item in items:
            case = IdfPytestPlugin.get_case_by_item(item)
            if not case:
                deselected_items.append(item)
                continue

            skipped = False
            current_targets = case.target_selector.split(',')
            for st in skipped_targets(item):
                # Handle wildcard patterns like "esp32p4,*" or "*,esp32p4"
                if '*' in st:
                    skip_pattern = st.split(',')
                    if len(skip_pattern) != len(current_targets):
                        continue
                    if all(_p == '*' or _p == _t for _p, _t in zip(skip_pattern, current_targets)):
                        skipped = True
                        break
                # Exact match (no wildcard)
                elif case.target_selector == st:
                    skipped = True
                    break

            if skipped:
                deselected_items.append(item)
                item.stash[IDF_CI_PYTEST_DEBUG_INFO_KEY] = 'skipped by temp_skip markers'
                continue

            if not case.env_markers and 'host_test' not in case.all_markers:
                if case.name in self.exclude_no_env_markers_test_cases:
                    deselected_items.append(item)
                    continue

                raise ValueError(
                    f'Test case {case.name} does not have any env markers. '
                    f'Please add env markers to the test case or add it to the '
                    f'`no_env_markers_test_cases` list in {KNOWN_GENERATE_TEST_CHILD_PIPELINE_WARNINGS_FILEPATH}'
                )

            filtered_items.append(item)

        items[:] = filtered_items

        # Deselect
        config.hook.pytest_deselected(items=deselected_items)

        # OKAY!!! All left ones will be executed, sort it and add more markers
        items[:] = sorted(items, key=lambda x: (os.path.dirname(x.path), self.get_param(x, 'config', 'default')))

        for item in items:
            case = IdfPytestPlugin.get_case_by_item(item)

            # set default timeout 10 minutes for each case
            if 'timeout' not in item.keywords:
                item.add_marker(pytest.mark.timeout(10 * 60))

            # add 'xtal_40mhz' tag as a default tag for esp32c2 target
            if 'esp32c2' in case.targets and 'xtal_26mhz' not in case.all_markers:
                item.add_marker('xtal_40mhz')

            for eco_marker in ECO_MARKERS:
                if eco_marker in case.all_markers:
                    break
            else:
                item.add_marker('rev_default')

            if 'host_test' in case.all_markers:
                item.add_marker('skip_app_downloader')  # host_test jobs will build the apps itself

    def pytest_custom_test_case_name(self, item: Function) -> str:
        return item.funcargs.get('test_case_name', item.nodeid)  # type: ignore

    def pytest_runtest_makereport(self, item: Function, call: CallInfo[None]) -> None:
        if call.when == 'call':
            target = item.funcargs['target']
            config = item.funcargs['config']
            is_qemu = item.get_closest_marker('qemu') is not None

            dut: Dut | tuple[Dut] = item.funcargs['dut']  # type: ignore
            if isinstance(dut, (list | tuple)):
                res = []
                for i, _dut in enumerate(dut):
                    res.extend(
                        [
                            ChildCase(
                                format_case_id(target, config, case.name, is_qemu=is_qemu),
                                self.UNITY_RESULT_MAPPINGS[case.result],
                            )
                            for case in _dut.testsuite.testcases
                        ]
                    )
                item.config.stash[ChildCasesStashKey] = {item.nodeid: res}
            else:
                item.config.stash[ChildCasesStashKey] = {
                    item.nodeid: [
                        ChildCase(
                            format_case_id(target, config, case.name, is_qemu=is_qemu),
                            self.UNITY_RESULT_MAPPINGS[case.result],
                        )
                        for case in dut.testsuite.testcases
                    ]
                }

    @pytest.hookimpl(trylast=True)
    def pytest_runtest_teardown(self, item: Function) -> None:
        """
        Modify the junit reports. Format the unity c test case names.
        """
        tempdir: str | None = item.funcargs.get('test_case_tempdir')  # type: ignore
        if not tempdir:
            return

        junits = find_by_suffix('.xml', tempdir)
        if not junits:
            return

        if len(junits) > 1:
            merge_junit_files(junits, os.path.join(tempdir, 'dut.xml'))
            junits = [os.path.join(tempdir, 'dut.xml')]

        # unity cases
        is_qemu = item.get_closest_marker('qemu') is not None
        target = item.funcargs['target']
        config = item.funcargs['config']
        app_path = item.funcargs.get('app_path')
        for junit in junits:
            xml = ET.parse(junit)
            testcases = xml.findall('.//testcase')
            for case in testcases:
                # modify the junit files
                # Use from case attrib if available, otherwise fallback to the previously defined
                app_path = case.attrib.get('app_path') or app_path
                new_case_name = format_case_id(target, config, case.attrib['name'], is_qemu=is_qemu)
                case.attrib['name'] = new_case_name
                if 'file' in case.attrib:
                    # our unity test framework
                    case.attrib['file'] = normalize_testcase_file_path(case.attrib['file'], app_path)
                if ci_job_url := os.getenv('CI_JOB_URL'):
                    case.attrib['ci_job_url'] = ci_job_url

            xml.write(junit)
