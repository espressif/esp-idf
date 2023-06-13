# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import typing as t
from fnmatch import fnmatch
from xml.etree import ElementTree as ET

import pytest
from _pytest.config import ExitCode
from _pytest.main import Session
from _pytest.python import Function
from _pytest.reports import TestReport
from _pytest.runner import CallInfo
from _pytest.terminal import TerminalReporter
from pytest_embedded.plugin import parse_multi_dut_args
from pytest_embedded.utils import find_by_suffix, to_list

from .constants import DEFAULT_SDKCONFIG, PREVIEW_TARGETS, SUPPORTED_TARGETS, PytestApp, PytestCase
from .utils import format_case_id, merge_junit_files

IDF_PYTEST_EMBEDDED_KEY = pytest.StashKey['IdfPytestEmbedded']()
ITEM_FAILED_CASES_KEY = pytest.StashKey[list]()
ITEM_FAILED_KEY = pytest.StashKey[bool]()


class IdfPytestEmbedded:
    def __init__(
        self,
        target: str,
        sdkconfig: t.Optional[str] = None,
        known_failure_cases_file: t.Optional[str] = None,
        apps_list: t.Optional[t.List[str]] = None,
    ):
        # CLI options to filter the test cases
        self.target = target.lower()
        self.sdkconfig = sdkconfig
        self.known_failure_patterns = self._parse_known_failure_cases_file(known_failure_cases_file)
        self.apps_list = apps_list

        self.cases: t.List[PytestCase] = []

        self._failed_cases: t.List[t.Tuple[str, bool, bool]] = []  # (test_case_name, is_known_failure_cases, is_xfail)

    @property
    def failed_cases(self) -> t.List[str]:
        return [case for case, is_known, is_xfail in self._failed_cases if not is_known and not is_xfail]

    @property
    def known_failure_cases(self) -> t.List[str]:
        return [case for case, is_known, _ in self._failed_cases if is_known]

    @property
    def xfail_cases(self) -> t.List[str]:
        return [case for case, _, is_xfail in self._failed_cases if is_xfail]

    @staticmethod
    def _parse_known_failure_cases_file(
        known_failure_cases_file: t.Optional[str] = None,
    ) -> t.List[str]:
        if not known_failure_cases_file or not os.path.isfile(known_failure_cases_file):
            return []

        patterns = []
        with open(known_failure_cases_file) as fr:
            for line in fr.readlines():
                if not line:
                    continue
                if not line.strip():
                    continue
                without_comments = line.split('#')[0].strip()
                if without_comments:
                    patterns.append(without_comments)

        return patterns

    @staticmethod
    def get_param(item: Function, key: str, default: t.Any = None) -> t.Any:
        # implement like this since this is a limitation of pytest, couldn't get fixture values while collecting
        # https://github.com/pytest-dev/pytest/discussions/9689
        if not hasattr(item, 'callspec'):
            raise ValueError(f'Function {item} does not have params')

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

        # filter all the test cases with "nightly_run" marker
        if os.getenv('INCLUDE_NIGHTLY_RUN') == '1':
            # Do not filter nightly_run cases
            pass
        elif os.getenv('NIGHTLY_RUN') == '1':
            items[:] = [item for item in items if item_to_case[item].is_nightly_run]
        else:
            items[:] = [item for item in items if not item_to_case[item].is_nightly_run]

        # filter all the test cases with target and skip_targets
        items[:] = [
            item
            for item in items
            if self.target in item_to_case[item].target_markers
            and self.target not in item_to_case[item].skipped_targets
        ]

        # filter all the test cases with cli option "config"
        if self.sdkconfig:
            items[:] = [item for item in items if self.get_param(item, 'config', DEFAULT_SDKCONFIG) == self.sdkconfig]

    def pytest_report_collectionfinish(self, items: t.List[Function]) -> None:
        for item in items:
            self.cases.append(self.item_to_pytest_case(item))

    def pytest_runtest_makereport(self, item: Function, call: CallInfo[None]) -> t.Optional[TestReport]:
        report = TestReport.from_item_and_call(item, call)
        if item.stash.get(ITEM_FAILED_KEY, None) is None:
            item.stash[ITEM_FAILED_KEY] = False

        if report.outcome == 'failed':
            # Mark the failed test cases
            #
            # This hook function would be called in 3 phases, setup, call, teardown.
            # the report.outcome is the outcome of the single call of current phase, which is independent
            # the call phase outcome is the test result
            item.stash[ITEM_FAILED_KEY] = True

        if call.when == 'teardown':
            item_failed = item.stash[ITEM_FAILED_KEY]
            if item_failed:
                # unity real test cases
                failed_sub_cases = item.stash.get(ITEM_FAILED_CASES_KEY, [])
                if failed_sub_cases:
                    for test_case_name in failed_sub_cases:
                        self._failed_cases.append((test_case_name, self._is_known_failure(test_case_name), False))
                else:  # the case iteself is failing
                    test_case_name = item.funcargs.get('test_case_name', '')
                    if test_case_name:
                        self._failed_cases.append(
                            (
                                test_case_name,
                                self._is_known_failure(test_case_name),
                                report.keywords.get('xfail', False),
                            )
                        )

        return report

    def _is_known_failure(self, case_id: str) -> bool:
        for pattern in self.known_failure_patterns:
            if case_id == pattern:
                return True
            if fnmatch(case_id, pattern):
                return True
        return False

    @pytest.hookimpl(trylast=True)
    def pytest_runtest_teardown(self, item: Function) -> None:
        """
        Format the test case generated junit reports
        """
        tempdir = item.funcargs.get('test_case_tempdir')
        if not tempdir:
            return

        junits = find_by_suffix('.xml', tempdir)
        if not junits:
            return

        if len(junits) > 1:
            merge_junit_files(junits, os.path.join(tempdir, 'dut.xml'))
            junits = [os.path.join(tempdir, 'dut.xml')]

        is_qemu = item.get_closest_marker('qemu') is not None
        failed_sub_cases = []
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

                # collect real failure cases
                if case.find('failure') is not None:
                    failed_sub_cases.append(new_case_name)

            xml.write(junit)

        item.stash[ITEM_FAILED_CASES_KEY] = failed_sub_cases

    def pytest_sessionfinish(self, session: Session, exitstatus: int) -> None:
        if exitstatus != 0:
            if exitstatus == ExitCode.NO_TESTS_COLLECTED:
                session.exitstatus = 0
            elif self.known_failure_cases and not self.failed_cases:
                session.exitstatus = 0

    def pytest_terminal_summary(self, terminalreporter: TerminalReporter) -> None:
        if self.known_failure_cases:
            terminalreporter.section('Known failure cases', bold=True, yellow=True)
            terminalreporter.line('\n'.join(self.known_failure_cases))

        if self.xfail_cases:
            terminalreporter.section('xfail cases', bold=True, yellow=True)
            terminalreporter.line('\n'.join(self.xfail_cases))

        if self.failed_cases:
            terminalreporter.section('Failed cases', bold=True, red=True)
            terminalreporter.line('\n'.join(self.failed_cases))
