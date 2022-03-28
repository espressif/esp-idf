# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# pylint: disable=W0621  # redefined-outer-name

# This file is a pytest root configuration file and provide the following functionalities:
# 1. Defines a few fixtures that could be used under the whole project.
# 2. Defines a few hook functions.
#
# IDF is using [pytest](https://github.com/pytest-dev/pytest) and
# [pytest-embedded plugin](https://github.com/espressif/pytest-embedded) as its example test framework.
#
# This is an experimental feature, and if you found any bug or have any question, please report to
# https://github.com/espressif/pytest-embedded/issues

import logging
import os
import sys
import xml.etree.ElementTree as ET
from fnmatch import fnmatch
from typing import Callable, List, Optional, Tuple

import pytest
from _pytest.config import Config
from _pytest.fixtures import FixtureRequest
from _pytest.main import Session
from _pytest.nodes import Item
from _pytest.python import Function
from _pytest.reports import TestReport
from _pytest.runner import CallInfo
from _pytest.terminal import TerminalReporter
from pytest_embedded.plugin import apply_count, parse_configuration
from pytest_embedded.utils import find_by_suffix

SUPPORTED_TARGETS = ['esp32', 'esp32s2', 'esp32c3', 'esp32s3']
PREVIEW_TARGETS = ['linux', 'esp32h2', 'esp32c2']
DEFAULT_SDKCONFIG = 'default'


##################
# Help Functions #
##################
def is_target_marker(marker: str) -> bool:
    if marker.startswith('esp32'):
        return True

    if marker.startswith('esp8'):
        return True

    return False


def format_case_id(target: Optional[str], config: Optional[str], case: str) -> str:
    return f'{target}.{config}.{case}'


def item_marker_names(item: Item) -> List[str]:
    return [marker.name for marker in item.iter_markers()]


############
# Fixtures #
############
@pytest.fixture
@parse_configuration
def config(request: FixtureRequest) -> str:
    return getattr(request, 'param', None) or DEFAULT_SDKCONFIG


@pytest.fixture
def test_func_name(request: FixtureRequest) -> str:
    return request.node.function.__name__  # type: ignore


@pytest.fixture
def test_case_name(request: FixtureRequest, target: str, config: str) -> str:
    return format_case_id(target, config, request.node.originalname)


@pytest.fixture
@apply_count
def build_dir(
    app_path: str, target: Optional[str], config: Optional[str]
) -> str:
    """
    Check local build dir with the following priority:

    1. build_<target>_<config>
    2. build_<target>
    3. build_<config>
    4. build

    Args:
        app_path: app path
        target: target
        config: config

    Returns:
        valid build directory
    """

    check_dirs = []
    if target is not None and config is not None:
        check_dirs.append(f'build_{target}_{config}')
    if target is not None:
        check_dirs.append(f'build_{target}')
    if config is not None:
        check_dirs.append(f'build_{config}')
    check_dirs.append('build')

    for check_dir in check_dirs:
        binary_path = os.path.join(app_path, check_dir)
        if os.path.isdir(binary_path):
            logging.info(f'find valid binary path: {binary_path}')
            return check_dir

        logging.warning(
            'checking binary path: %s... missing... try another place', binary_path
        )

    recommend_place = check_dirs[0]
    logging.error(
        f'no build dir valid. Please build the binary via "idf.py -B {recommend_place} build" and run pytest again'
    )
    sys.exit(1)


@pytest.fixture(autouse=True)
@apply_count
def junit_properties(
    test_case_name: str, record_xml_attribute: Callable[[str, object], None]
) -> None:
    """
    This fixture is autoused and will modify the junit report test case name to <target>.<config>.<case_name>
    """
    record_xml_attribute('name', test_case_name)


##################
# Hook functions #
##################
def pytest_addoption(parser: pytest.Parser) -> None:
    base_group = parser.getgroup('idf')
    base_group.addoption(
        '--sdkconfig',
        help='sdkconfig postfix, like sdkconfig.ci.<config>. (Default: None, which would build all found apps)',
    )
    base_group.addoption(
        '--known-failure-cases-file', help='known failure cases file path'
    )


_idf_pytest_embedded_key = pytest.StashKey['IdfPytestEmbedded']


def pytest_configure(config: Config) -> None:
    config.stash[_idf_pytest_embedded_key] = IdfPytestEmbedded(
        target=config.getoption('target'),
        sdkconfig=config.getoption('sdkconfig'),
        known_failure_cases_file=config.getoption('known_failure_cases_file'),
    )
    config.pluginmanager.register(config.stash[_idf_pytest_embedded_key])


def pytest_unconfigure(config: Config) -> None:
    _pytest_embedded = config.stash.get(_idf_pytest_embedded_key, None)
    if _pytest_embedded:
        del config.stash[_idf_pytest_embedded_key]
        config.pluginmanager.unregister(_pytest_embedded)


class IdfPytestEmbedded:
    def __init__(
        self,
        target: Optional[str] = None,
        sdkconfig: Optional[str] = None,
        known_failure_cases_file: Optional[str] = None,
    ):
        # CLI options to filter the test cases
        self.target = target
        self.sdkconfig = sdkconfig
        self.known_failure_patterns = self._parse_known_failure_cases_file(
            known_failure_cases_file
        )

        self._failed_cases: List[
            Tuple[str, bool]
        ] = []  # (test_case_name, is_known_failure_cases)

    @property
    def failed_cases(self) -> List[str]:
        return [case for case, is_known in self._failed_cases if not is_known]

    @property
    def known_failure_cases(self) -> List[str]:
        return [case for case, is_known in self._failed_cases if is_known]

    @staticmethod
    def _parse_known_failure_cases_file(
        known_failure_cases_file: Optional[str] = None,
    ) -> List[str]:
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

    @pytest.hookimpl(tryfirst=True)
    def pytest_sessionstart(self, session: Session) -> None:
        if self.target:
            self.target = self.target.lower()
            session.config.option.target = self.target

    @pytest.hookimpl(tryfirst=True)
    def pytest_collection_modifyitems(self, items: List[Function]) -> None:
        # sort by file path and callspec.config
        # implement like this since this is a limitation of pytest, couldn't get fixture values while collecting
        # https://github.com/pytest-dev/pytest/discussions/9689
        def _get_param_config(_item: Function) -> str:
            if hasattr(_item, 'callspec'):
                return _item.callspec.params.get('config', DEFAULT_SDKCONFIG)  # type: ignore
            return DEFAULT_SDKCONFIG

        items.sort(key=lambda x: (os.path.dirname(x.path), _get_param_config(x)))

        # add markers for special markers
        for item in items:
            if 'supported_targets' in item_marker_names(item):
                for _target in SUPPORTED_TARGETS:
                    item.add_marker(_target)
            if 'preview_targets' in item_marker_names(item):
                for _target in PREVIEW_TARGETS:
                    item.add_marker(_target)
            if 'all_targets' in item_marker_names(item):
                for _target in [*SUPPORTED_TARGETS, *PREVIEW_TARGETS]:
                    item.add_marker(_target)
            # FIXME: temporarily modify the s3 runner tag "generic" to "s3_generic" due to the deep sleep bug
            if 'generic' in item_marker_names(item) and 'esp32s3' in item_marker_names(item):
                item.add_marker('generic_s3_fixme')

        # filter all the test cases with "--target"
        if self.target:
            items[:] = [
                item for item in items if self.target in item_marker_names(item)
            ]

        # filter all the test cases with cli option "config"
        if self.sdkconfig:
            items[:] = [
                item for item in items if _get_param_config(item) == self.sdkconfig
            ]

    def pytest_runtest_makereport(
        self, item: Function, call: CallInfo[None]
    ) -> Optional[TestReport]:
        if call.when == 'setup':
            return None

        report = TestReport.from_item_and_call(item, call)
        if report.outcome == 'failed':
            test_case_name = item.funcargs.get('test_case_name', '')
            is_known_failure = self._is_known_failure(test_case_name)
            self._failed_cases.append((test_case_name, is_known_failure))

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

        target = item.funcargs['target']
        config = item.funcargs['config']
        for junit in junits:
            xml = ET.parse(junit)
            testcases = xml.findall('.//testcase')
            for case in testcases:
                case.attrib['name'] = format_case_id(
                    target, config, case.attrib['name']
                )
                if 'file' in case.attrib:
                    case.attrib['file'] = case.attrib['file'].replace(
                        '/IDF/', ''
                    )  # our unity test framework
            xml.write(junit)

    def pytest_sessionfinish(self, session: Session, exitstatus: int) -> None:
        if exitstatus != 0 and self.known_failure_cases and not self.failed_cases:
            session.exitstatus = 0

    def pytest_terminal_summary(self, terminalreporter: TerminalReporter) -> None:
        if self.known_failure_cases:
            terminalreporter.section('Known failure cases', bold=True, yellow=True)
            terminalreporter.line('\n'.join(self.known_failure_cases))

        if self.failed_cases:
            terminalreporter.section('Failed cases', bold=True, red=True)
            terminalreporter.line('\n'.join(self.failed_cases))
