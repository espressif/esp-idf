# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import importlib
import logging
import os
import sys
import typing as t
from collections import defaultdict
from functools import cached_property
from unittest.mock import MagicMock
from xml.etree import ElementTree as ET

import pytest
from _pytest.config import ExitCode
from _pytest.main import Session
from _pytest.python import Function
from _pytest.runner import CallInfo
from idf_build_apps import App
from idf_build_apps.constants import BuildStatus
from idf_ci_utils import idf_relpath
from pytest_embedded import Dut
from pytest_embedded.plugin import parse_multi_dut_args
from pytest_embedded.utils import find_by_suffix
from pytest_embedded.utils import to_list
from pytest_ignore_test_results.ignore_results import ChildCase
from pytest_ignore_test_results.ignore_results import ChildCasesStashKey

from .constants import CollectMode
from .constants import DEFAULT_SDKCONFIG
from .constants import PREVIEW_TARGETS
from .constants import PytestApp
from .constants import PytestCase
from .constants import SUPPORTED_TARGETS
from .utils import comma_sep_str_to_list
from .utils import format_case_id
from .utils import merge_junit_files

IDF_PYTEST_EMBEDDED_KEY = pytest.StashKey['IdfPytestEmbedded']()
ITEM_FAILED_CASES_KEY = pytest.StashKey[list]()
ITEM_FAILED_KEY = pytest.StashKey[bool]()
ITEM_PYTEST_CASE_KEY = pytest.StashKey[PytestCase]()


class IdfPytestEmbedded:
    UNITY_RESULT_MAPPINGS = {
        'PASS': 'passed',
        'FAIL': 'failed',
        'IGNORE': 'skipped',
    }

    def __init__(
        self,
        target: t.Union[t.List[str], str],
        *,
        config_name: t.Optional[str] = None,
        single_target_duplicate_mode: bool = False,
        apps: t.Optional[t.List[App]] = None,
    ):
        if isinstance(target, str):
            # sequence also matters
            self.target = comma_sep_str_to_list(target)
        else:
            self.target = target

        if not self.target:
            raise ValueError('`target` should not be empty')

        self.config_name = config_name

        # these are useful while gathering all the multi-dut test cases
        # when this mode is activated,
        #
        # pytest.mark.esp32
        # pytest.mark.parametrize('count', [2], indirect=True)
        # def test_foo(dut):
        #     pass
        #
        # should be collected when running `pytest --target esp32`
        #
        # otherwise, it should be collected when running `pytest --target esp32,esp32`
        self._single_target_duplicate_mode = single_target_duplicate_mode

        self.apps_list = (
            [os.path.join(idf_relpath(app.app_dir), app.build_dir) for app in apps if app.build_status == BuildStatus.SUCCESS]
            if apps is not None
            else None
        )

        self.cases: t.List[PytestCase] = []

        # record the additional info
        # test case id: {key: value}
        self.additional_info: t.Dict[str, t.Dict[str, t.Any]] = defaultdict(dict)

    @cached_property
    def collect_mode(self) -> CollectMode:
        if len(self.target) == 1:
            if self.target[0] == CollectMode.MULTI_ALL_WITH_PARAM:
                return CollectMode.MULTI_ALL_WITH_PARAM
            else:
                return CollectMode.SINGLE_SPECIFIC
        else:
            return CollectMode.MULTI_SPECIFIC

    @staticmethod
    def get_param(item: Function, key: str, default: t.Any = None) -> t.Any:
        # funcargs is not calculated while collection
        # callspec is something defined in parametrize
        if not hasattr(item, 'callspec'):
            return default

        return item.callspec.params.get(key, default) or default

    def item_to_pytest_case(self, item: Function) -> t.Optional[PytestCase]:
        """
        Turn pytest item to PytestCase
        """
        count = self.get_param(item, 'count', 1)

        # default app_path is where the test script locates
        app_paths = to_list(parse_multi_dut_args(count, self.get_param(item, 'app_path', os.path.dirname(item.path))))
        configs = to_list(parse_multi_dut_args(count, self.get_param(item, 'config', DEFAULT_SDKCONFIG)))
        targets = to_list(parse_multi_dut_args(count, self.get_param(item, 'target')))

        multi_dut_without_param = False
        if count > 1 and targets == [None] * count:
            multi_dut_without_param = True
            try:
                targets = to_list(parse_multi_dut_args(count, '|'.join(self.target)))  # check later while collecting
            except ValueError:  # count doesn't match
                return None

        elif targets is None:
            targets = self.target

        return PytestCase(
            apps=[PytestApp(app_paths[i], targets[i], configs[i]) for i in range(count)],
            item=item,
            multi_dut_without_param=multi_dut_without_param
        )

    def pytest_collectstart(self) -> None:
        # mock the optional packages while collecting locally
        if not os.getenv('CI_JOB_ID') or os.getenv('PYTEST_IGNORE_COLLECT_IMPORT_ERROR') == '1':
            # optional packages required by test scripts
            for p in [
                'scapy',
                'scapy.all',
                'websocket',  # websocket-client
                'netifaces',
                'RangeHTTPServer',  # rangehttpserver
                'dbus',  # dbus-python
                'dbus.mainloop',
                'dbus.mainloop.glib',
                'google.protobuf',  # protobuf
                'google.protobuf.internal',
                'bleak',
                'paho',  # paho-mqtt
                'paho.mqtt',
                'paho.mqtt.client',
                'paramiko',
                'netmiko',
                'pyecharts',
                'pyecharts.options',
                'pyecharts.charts',
                'can',  # python-can
            ]:
                try:
                    importlib.import_module(p)
                except ImportError:
                    logging.warning(f'Optional package {p} is not installed, mocking it while collecting...')
                    sys.modules[p] = MagicMock()

    @pytest.hookimpl(tryfirst=True)
    def pytest_collection_modifyitems(self, items: t.List[Function]) -> None:
        """
        Background info:

        We're using `pytest.mark.[TARGET]` as a syntactic sugar to indicate that they are actually supported by all
        the listed targets. For example,

        >>> @pytest.mark.esp32
        >>> @pytest.mark.esp32s2

        should be treated as

        >>> @pytest.mark.parametrize('target', [
        >>>     'esp32',
        >>>     'esp32s2',
        >>> ], indirect=True)

        All single-dut test cases, and some of the multi-dut test cases with the same targets, are using this
        way to indicate the supported targets.

        To avoid ambiguity,

        - when we're collecting single-dut test cases with esp32, we call

            `pytest --collect-only --target esp32`

        - when we're collecting multi-dut test cases, we list all the targets, even when they're the same

            `pytest --collect-only --target esp32,esp32` for two esp32 connected
            `pytest --collect-only --target esp32,esp32s2` for esp32 and esp32s2 connected

        therefore, we have two different logic for searching test cases, explained in 2.1 and 2.2
        """
        # 1. Filter according to nighty_run related markers
        if os.getenv('INCLUDE_NIGHTLY_RUN') == '1':
            # nightly_run and non-nightly_run cases are both included
            pass
        elif os.getenv('NIGHTLY_RUN') == '1':
            # only nightly_run cases are included
            items[:] = [_item for _item in items if _item.get_closest_marker('nightly_run') is not None]
        else:
            # only non-nightly_run cases are included
            items[:] = [_item for _item in items if _item.get_closest_marker('nightly_run') is None]

        # 2. Add markers according to special markers
        item_to_case_dict: t.Dict[Function, PytestCase] = {}
        for item in items:
            case = self.item_to_pytest_case(item)
            if case is None:
                continue

            item.stash[ITEM_PYTEST_CASE_KEY] = item_to_case_dict[item] = case
            if 'supported_targets' in item.keywords:
                for _target in SUPPORTED_TARGETS:
                    item.add_marker(_target)
            if 'preview_targets' in item.keywords:
                for _target in PREVIEW_TARGETS:
                    item.add_marker(_target)
            if 'all_targets' in item.keywords:
                for _target in [*SUPPORTED_TARGETS, *PREVIEW_TARGETS]:
                    item.add_marker(_target)

            # add single-dut "target" as param
            _item_target_param = self.get_param(item, 'target', None)
            if case.is_single_dut_test_case and _item_target_param and _item_target_param not in case.all_markers:
                item.add_marker(_item_target_param)

        items[:] = [_item for _item in items if _item in item_to_case_dict]

        # 3.1. CollectMode.SINGLE_SPECIFIC, like `pytest --target esp32`
        if self.collect_mode == CollectMode.SINGLE_SPECIFIC:
            filtered_items = []
            for item in items:
                case = item_to_case_dict[item]

                # single-dut one
                if case.is_single_dut_test_case and self.target[0] in case.target_markers:
                    filtered_items.append(item)

                # multi-dut ones and in single_target_duplicate_mode
                elif self._single_target_duplicate_mode and not case.is_single_dut_test_case:
                    # ignore those test cases with `target` defined in parametrize, since these will be covered in 3.3
                    if self.get_param(item, 'target', None) is None and self.target[0] in case.target_markers:
                        filtered_items.append(item)

            items[:] = filtered_items
        # 3.2. CollectMode.MULTI_SPECIFIC, like `pytest --target esp32,esp32`
        elif self.collect_mode == CollectMode.MULTI_SPECIFIC:
            items[:] = [_item for _item in items if item_to_case_dict[_item].targets == self.target]

        # 3.3. CollectMode.MULTI_ALL_WITH_PARAM, intended to be used by `get_pytest_cases`
        else:
            items[:] = [
                _item
                for _item in items
                if not item_to_case_dict[_item].is_single_dut_test_case
                and self.get_param(_item, 'target', None) is not None
            ]

        # 4. filter according to the sdkconfig, if there's param 'config' defined
        if self.config_name:
            _items = []
            for item in items:
                case = item_to_case_dict[item]
                if self.config_name not in set(app.config or DEFAULT_SDKCONFIG for app in case.apps):
                    self.additional_info[case.name]['skip_reason'] = f'Only run with sdkconfig {self.config_name}'
                else:
                    _items.append(item)
            items[:] = _items

        # 5. filter by `self.apps_list`, skip the test case if not listed
        #   should only be used in CI
        _items = []
        for item in items:
            case = item_to_case_dict[item]
            if msg := case.all_built_in_app_lists(self.apps_list):
                self.additional_info[case.name]['skip_reason'] = msg
            else:
                _items.append(item)

        # OKAY!!! All left ones will be executed, sort it and add more markers
        items[:] = sorted(
            _items, key=lambda x: (os.path.dirname(x.path), self.get_param(x, 'config', DEFAULT_SDKCONFIG))
        )
        for item in items:
            case = item_to_case_dict[item]
            # set default timeout 10 minutes for each case
            if 'timeout' not in item.keywords:
                item.add_marker(pytest.mark.timeout(10 * 60))

            # add 'xtal_40mhz' tag as a default tag for esp32c2 target
            # only add this marker for esp32c2 cases
            if 'esp32c2' in self.target and 'esp32c2' in case.targets and 'xtal_26mhz' not in case.all_markers:
                item.add_marker('xtal_40mhz')

    def pytest_report_collectionfinish(self, items: t.List[Function]) -> None:
        self.cases = [item.stash[ITEM_PYTEST_CASE_KEY] for item in items]

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

                if ci_job_url := os.getenv('CI_JOB_URL'):
                    case.attrib['ci_job_url'] = ci_job_url

            xml.write(junit)

    def pytest_sessionfinish(self, session: Session, exitstatus: int) -> None:
        if exitstatus != 0:
            if exitstatus == ExitCode.NO_TESTS_COLLECTED:
                session.exitstatus = 0
