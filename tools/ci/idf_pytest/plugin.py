# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import os
import typing as t
from xml.etree import ElementTree as ET

import pytest
from _pytest.config import ExitCode
from _pytest.main import Session
from _pytest.python import Function
from _pytest.runner import CallInfo
from pytest_embedded import Dut
from pytest_embedded.plugin import parse_multi_dut_args
from pytest_embedded.utils import find_by_suffix, to_list
from pytest_ignore_test_results.ignore_results import ChildCase, ChildCasesStashKey

from .constants import DEFAULT_SDKCONFIG, PREVIEW_TARGETS, SUPPORTED_TARGETS, PytestApp, PytestCase
from .utils import format_case_id, merge_junit_files

IDF_PYTEST_EMBEDDED_KEY = pytest.StashKey['IdfPytestEmbedded']()
ITEM_FAILED_CASES_KEY = pytest.StashKey[list]()
ITEM_FAILED_KEY = pytest.StashKey[bool]()


class IdfPytestEmbedded:
    UNITY_RESULT_MAPPINGS = {
        'PASS': 'passed',
        'FAIL': 'failed',
        'IGNORE': 'skipped',
    }

    def __init__(
        self,
        target: str,
        sdkconfig: t.Optional[str] = None,
        apps_list: t.Optional[t.List[str]] = None,
    ):
        # CLI options to filter the test cases
        self.target = target.lower()
        self.sdkconfig = sdkconfig
        self.apps_list = apps_list

        self.cases: t.List[PytestCase] = []

    @staticmethod
    def get_param(item: Function, key: str, default: t.Any = None) -> t.Any:
        # implement like this since this is a limitation of pytest, couldn't get fixture values while collecting
        # https://github.com/pytest-dev/pytest/discussions/9689
        if not hasattr(item, 'callspec'):
            return default

        return item.callspec.params.get(key, default) or default

    def item_to_pytest_case(self, item: Function) -> PytestCase:
        count = 1
        case_path = str(item.path)
        case_name = item.originalname
        target = self.target

        # funcargs is not calculated while collection
        if hasattr(item, 'callspec'):
            count = item.callspec.params.get('count', 1)
            app_paths = to_list(
                parse_multi_dut_args(
                    count,
                    self.get_param(item, 'app_path', os.path.dirname(case_path)),
                )
            )
            configs = to_list(parse_multi_dut_args(count, self.get_param(item, 'config', 'default')))
            targets = to_list(parse_multi_dut_args(count, self.get_param(item, 'target', target)))
        else:
            app_paths = [os.path.dirname(case_path)]
            configs = ['default']
            targets = [target]

        case_apps = set()
        for i in range(count):
            case_apps.add(PytestApp(app_paths[i], targets[i], configs[i]))

        return PytestCase(
            case_path,
            case_name,
            case_apps,
            self.target,
            item,
        )

    @pytest.hookimpl(tryfirst=True)
    def pytest_sessionstart(self, session: Session) -> None:
        # same behavior for vanilla pytest-embedded '--target'
        session.config.option.target = self.target

    @pytest.hookimpl(tryfirst=True)
    def pytest_collection_modifyitems(self, items: t.List[Function]) -> None:
        item_to_case: t.Dict[Function, PytestCase] = {}

        # Add Markers to the test cases
        for item in items:
            # generate PytestCase for each item
            case = self.item_to_pytest_case(item)
            item_to_case[item] = case

            # set default timeout 10 minutes for each case
            if 'timeout' not in item.keywords:
                item.add_marker(pytest.mark.timeout(10 * 60))

            # add markers for special markers
            if 'supported_targets' in item.keywords:
                for _target in SUPPORTED_TARGETS:
                    item.add_marker(_target)
            if 'preview_targets' in item.keywords:
                for _target in PREVIEW_TARGETS:
                    item.add_marker(_target)
            if 'all_targets' in item.keywords:
                for _target in [*SUPPORTED_TARGETS, *PREVIEW_TARGETS]:
                    item.add_marker(_target)

            # add 'xtal_40mhz' tag as a default tag for esp32c2 target
            # only add this marker for esp32c2 cases
            if self.target == 'esp32c2' and 'esp32c2' in case.target_markers and 'xtal_26mhz' not in case.all_markers:
                item.add_marker('xtal_40mhz')

        # Filter the test cases
        filtered_items = []
        for item in items:
            case = item_to_case[item]
            # filter by "nightly_run" marker
            if os.getenv('INCLUDE_NIGHTLY_RUN') == '1':
                # Do not filter nightly_run cases
                pass
            elif os.getenv('NIGHTLY_RUN') == '1':
                if not case.is_nightly_run:
                    logging.debug(
                        'Skipping test case %s because of this test case is not a nightly run test case', item.name
                    )
                    continue
            else:
                if case.is_nightly_run:
                    logging.debug(
                        'Skipping test case %s because of this test case is a nightly run test case', item.name
                    )
                    continue

            # filter by target
            if self.target not in case.target_markers:
                continue

            if self.target in case.skipped_targets:
                continue

            # filter by sdkconfig
            if self.sdkconfig:
                if self.get_param(item, 'config', DEFAULT_SDKCONFIG) != self.sdkconfig:
                    continue

            # filter by apps_list, skip the test case if not listed
            # should only be used in CI
            if self.apps_list is not None:
                bin_not_found = False
                for case_app in case.apps:
                    # in ci, always use build_<target>_<config> as build dir
                    binary_path = os.path.join(case_app.path, f'build_{case_app.target}_{case_app.config}')
                    if binary_path not in self.apps_list:
                        logging.info(
                            'Skipping test case %s because binary path %s is not listed in app info list files',
                            item.name,
                            binary_path,
                        )
                        bin_not_found = True
                        break

                if bin_not_found:
                    continue

            # finally!
            filtered_items.append(item)

        # sort the test cases with (app folder, config)
        items[:] = sorted(
            filtered_items,
            key=lambda x: (os.path.dirname(x.path), self.get_param(x, 'config', DEFAULT_SDKCONFIG))
        )

    def pytest_report_collectionfinish(self, items: t.List[Function]) -> None:
        for item in items:
            self.cases.append(self.item_to_pytest_case(item))

    def pytest_custom_test_case_name(self, item: Function) -> str:
        return item.funcargs.get('test_case_name', item.nodeid)  # type: ignore

    def pytest_runtest_makereport(self, item: Function, call: CallInfo[None]) -> None:
        if call.when == 'call':
            target = item.funcargs['target']
            config = item.funcargs['config']
            is_qemu = item.get_closest_marker('qemu') is not None

            dut: t.Union[Dut, t.Tuple[Dut]] = item.funcargs['dut']  # type: ignore
            if isinstance(dut, (list, tuple)):
                res = []
                for i, _dut in enumerate(dut):
                    res.extend(
                        [
                            ChildCase(
                                format_case_id(target, config, case.name + f' {i}', is_qemu=is_qemu),
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
        tempdir: t.Optional[str] = item.funcargs.get('test_case_tempdir')  # type: ignore
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
        for junit in junits:
            xml = ET.parse(junit)
            testcases = xml.findall('.//testcase')
            for case in testcases:
                # modify the junit files
                new_case_name = format_case_id(target, config, case.attrib['name'], is_qemu=is_qemu)
                case.attrib['name'] = new_case_name
                if 'file' in case.attrib:
                    case.attrib['file'] = case.attrib['file'].replace('/IDF/', '')  # our unity test framework

            xml.write(junit)

    def pytest_sessionfinish(self, session: Session, exitstatus: int) -> None:
        if exitstatus != 0:
            if exitstatus == ExitCode.NO_TESTS_COLLECTED:
                session.exitstatus = 0
