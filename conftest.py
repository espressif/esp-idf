# SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
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
import re
import sys
import xml.etree.ElementTree as ET
from datetime import datetime
from fnmatch import fnmatch
from typing import Callable, List, Optional, Tuple

import pytest
from _pytest.config import Config, ExitCode
from _pytest.fixtures import FixtureRequest
from _pytest.main import Session
from _pytest.nodes import Item
from _pytest.python import Function
from _pytest.reports import TestReport
from _pytest.runner import CallInfo
from _pytest.terminal import TerminalReporter
from pytest_embedded.plugin import multi_dut_argument, multi_dut_fixture
from pytest_embedded.utils import find_by_suffix
from pytest_embedded_idf.dut import IdfDut

try:
    from idf_ci_utils import to_list
    from idf_unity_tester import CaseTester
except ImportError:
    sys.path.append(os.path.join(os.path.dirname(__file__), 'tools', 'ci'))
    from idf_ci_utils import to_list
    from idf_unity_tester import CaseTester

try:
    import common_test_methods  # noqa: F401
except ImportError:
    sys.path.append(os.path.join(os.path.dirname(__file__), 'tools', 'ci', 'python_packages'))
    import common_test_methods  # noqa: F401

SUPPORTED_TARGETS = ['esp32', 'esp32s2', 'esp32c3', 'esp32s3', 'esp32c2', 'esp32c6', 'esp32h2']
PREVIEW_TARGETS: List[str] = []  # this PREVIEW_TARGETS excludes 'linux' target
DEFAULT_SDKCONFIG = 'default'

TARGET_MARKERS = {
    'esp32': 'support esp32 target',
    'esp32s2': 'support esp32s2 target',
    'esp32s3': 'support esp32s3 target',
    'esp32c3': 'support esp32c3 target',
    'esp32c2': 'support esp32c2 target',
    'esp32c6': 'support esp32c6 target',
    'esp32h2': 'support esp32h2 target',
    'linux': 'support linux target',
}

SPECIAL_MARKERS = {
    'supported_targets': "support all officially announced supported targets ('esp32', 'esp32s2', 'esp32c3', 'esp32s3', 'esp32c2', 'esp32c6')",
    'preview_targets': "support all preview targets ('none')",
    'all_targets': 'support all targets, including supported ones and preview ones',
    'temp_skip_ci': 'temp skip tests for specified targets only in ci',
    'temp_skip': 'temp skip tests for specified targets both in ci and locally',
    'nightly_run': 'tests should be executed as part of the nightly trigger pipeline',
    'host_test': 'tests which should not be built at the build stage, and instead built in host_test stage',
    'qemu': 'build and test using qemu-system-xtensa, not real target',
}

ENV_MARKERS = {
    # single-dut markers
    'generic': 'tests should be run on generic runners',
    'flash_suspend': 'support flash suspend feature',
    'eth_ip101': 'connected via wired 10/100M ethernet',
    'eth_lan8720': 'connected via LAN8720 ethernet transceiver',
    'eth_rtl8201': 'connected via RTL8201 ethernet transceiver',
    'eth_ksz8041': 'connected via KSZ8041 ethernet transceiver',
    'eth_dp83848': 'connected via DP83848 ethernet transceiver',
    'eth_w5500': 'SPI Ethernet module with two W5500',
    'eth_ksz8851snl': 'SPI Ethernet module with two KSZ8851SNL',
    'eth_dm9051': 'SPI Ethernet module with two DM9051',
    'quad_psram': 'runners with quad psram',
    'octal_psram': 'runners with octal psram',
    'usb_host': 'usb host runners',
    'usb_host_flash_disk': 'usb host runners with USB flash disk attached',
    'usb_device': 'usb device runners',
    'ethernet_ota': 'ethernet OTA runners',
    'flash_encryption': 'Flash Encryption runners',
    'flash_encryption_f4r8': 'Flash Encryption runners with 4-line flash and 8-line psram',
    'flash_encryption_f8r8': 'Flash Encryption runners with 8-line flash and 8-line psram',
    'flash_multi': 'Multiple flash chips tests',
    'psram': 'Chip has 4-line psram',
    'ir_transceiver': 'runners with a pair of IR transmitter and receiver',
    'twai_transceiver': 'runners with a TWAI PHY transceiver',
    'flash_encryption_wifi_high_traffic': 'Flash Encryption runners with wifi high traffic support',
    'ethernet': 'ethernet runner',
    'ethernet_flash_8m': 'ethernet runner with 8mb flash',
    'ethernet_router': 'both the runner and dut connect to the same router through ethernet NIC',
    'wifi_ap': 'a wifi AP in the environment',
    'wifi_router': 'both the runner and dut connect to the same wifi router',
    'wifi_high_traffic': 'wifi high traffic runners',
    'wifi_wlan': 'wifi runner with a wireless NIC',
    'Example_ShieldBox_Basic': 'basic configuration of the AP and ESP DUT placed in shielded box',
    'Example_ShieldBox': 'multiple shielded APs connected to shielded ESP DUT via RF cable with programmable attenuator',
    'xtal_26mhz': 'runner with 26MHz xtal on board',
    'xtal_40mhz': 'runner with 40MHz xtal on board',
    'external_flash': 'external flash memory connected via VSPI (FSPI)',
    'sdcard_sdmode': 'sdcard running in SD mode',
    'sdcard_spimode': 'sdcard running in SPI mode',
    'emmc': 'eMMC card',
    'MSPI_F8R8': 'runner with Octal Flash and Octal PSRAM',
    'MSPI_F4R8': 'runner with Quad Flash and Octal PSRAM',
    'MSPI_F4R4': 'runner with Quad Flash and Quad PSRAM',
    'jtag': 'runner where the chip is accessible through JTAG as well',
    'usb_serial_jtag': 'runner where the chip is accessible through builtin JTAG as well',
    'adc': 'ADC related tests should run on adc runners',
    'xtal32k': 'Runner with external 32k crystal connected',
    'no32kXtal': 'Runner with no external 32k crystal connected',
    'multi_dut_modbus_rs485': 'a pair of runners connected by RS485 bus',
    'psramv0': 'Runner with PSRAM version 0',
    'esp32eco3': 'Runner with esp32 eco3 connected',
    'ecdsa_efuse': 'Runner with test ECDSA private keys programmed in efuse',
    'ccs811': 'Runner with CCS811 connected',
    'httpbin': 'runner for tests that need to access the httpbin service',
    # multi-dut markers
    'ieee802154': 'ieee802154 related tests should run on ieee802154 runners.',
    'openthread_br': 'tests should be used for openthread border router.',
    'openthread_sleep': 'tests should be used for openthread sleepy device.',
    'wifi_two_dut': 'tests should be run on runners which has two wifi duts connected.',
    'generic_multi_device': 'generic multiple devices whose corresponding gpio pins are connected to each other.',
    'twai_network': 'multiple runners form a TWAI network.',
    'sdio_master_slave': 'Test sdio multi board.',
}


##################
# Help Functions #
##################
def format_case_id(target: Optional[str], config: Optional[str], case: str) -> str:
    return f'{target}.{config}.{case}'


def item_marker_names(item: Item) -> List[str]:
    return [marker.name for marker in item.iter_markers()]


def item_target_marker_names(item: Item) -> List[str]:
    res = set()
    for marker in item.iter_markers():
        if marker.name in TARGET_MARKERS:
            res.add(marker.name)

    return sorted(res)


def item_env_marker_names(item: Item) -> List[str]:
    res = set()
    for marker in item.iter_markers():
        if marker.name in ENV_MARKERS:
            res.add(marker.name)

    return sorted(res)


def item_skip_targets(item: Item) -> List[str]:
    def _get_temp_markers_disabled_targets(marker_name: str) -> List[str]:
        temp_marker = item.get_closest_marker(marker_name)

        if not temp_marker:
            return []

        # temp markers should always use keyword arguments `targets` and `reason`
        if not temp_marker.kwargs.get('targets') or not temp_marker.kwargs.get('reason'):
            raise ValueError(
                f'`{marker_name}` should always use keyword arguments `targets` and `reason`. '
                f'For example: '
                f'`@pytest.mark.{marker_name}(targets=["esp32"], reason="IDF-xxxx, will fix it ASAP")`'
            )

        return to_list(temp_marker.kwargs['targets'])  # type: ignore

    temp_skip_ci_targets = _get_temp_markers_disabled_targets('temp_skip_ci')
    temp_skip_targets = _get_temp_markers_disabled_targets('temp_skip')

    # in CI we skip the union of `temp_skip` and `temp_skip_ci`
    if os.getenv('CI_JOB_ID'):
        skip_targets = list(set(temp_skip_ci_targets).union(set(temp_skip_targets)))
    else:  # we use `temp_skip` locally
        skip_targets = temp_skip_targets

    return skip_targets


def get_target_marker_from_expr(markexpr: str) -> str:
    candidates = set()
    # we use `-m "esp32 and generic"` in our CI to filter the test cases
    # this doesn't cover all use cases, but fit what we do in CI.
    for marker in markexpr.split('and'):
        marker = marker.strip()
        if marker in TARGET_MARKERS:
            candidates.add(marker)

    if len(candidates) > 1:
        raise ValueError(f'Specified more than one target markers: {candidates}. Please specify no more than one.')
    elif len(candidates) == 1:
        return candidates.pop()
    else:
        raise ValueError('Please specify one target marker via "--target [TARGET]" or via "-m [TARGET]"')


############
# Fixtures #
############
@pytest.fixture(scope='session')
def idf_path() -> str:
    return os.path.dirname(__file__)


@pytest.fixture(scope='session', autouse=True)
def session_tempdir() -> str:
    _tmpdir = os.path.join(
        os.path.dirname(__file__),
        'pytest_embedded_log',
        datetime.now().strftime('%Y-%m-%d_%H-%M-%S'),
    )
    os.makedirs(_tmpdir, exist_ok=True)
    return _tmpdir


@pytest.fixture
def case_tester(dut: IdfDut, **kwargs):  # type: ignore
    yield CaseTester(dut, **kwargs)


@pytest.fixture
@multi_dut_argument
def config(request: FixtureRequest) -> str:
    return getattr(request, 'param', None) or DEFAULT_SDKCONFIG


@pytest.fixture
def test_func_name(request: FixtureRequest) -> str:
    return request.node.function.__name__  # type: ignore


@pytest.fixture
def test_case_name(request: FixtureRequest, target: str, config: str) -> str:
    return format_case_id(target, config, request.node.originalname)


@pytest.fixture
@multi_dut_fixture
def build_dir(app_path: str, target: Optional[str], config: Optional[str]) -> str:
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

        logging.warning('checking binary path: %s... missing... try another place', binary_path)

    recommend_place = check_dirs[0]
    raise ValueError(
        f'no build dir valid. Please build the binary via "idf.py -B {recommend_place} build" and run pytest again'
    )


@pytest.fixture(autouse=True)
@multi_dut_fixture
def junit_properties(test_case_name: str, record_xml_attribute: Callable[[str, object], None]) -> None:
    """
    This fixture is autoused and will modify the junit report test case name to <target>.<config>.<case_name>
    """
    record_xml_attribute('name', test_case_name)


@pytest.fixture(autouse=True)
def set_test_case_name(request: FixtureRequest, test_case_name: str) -> None:
    request.node.funcargs['test_case_name'] = test_case_name


######################
# Log Util Functions #
######################
@pytest.fixture
def log_performance(record_property: Callable[[str, object], None]) -> Callable[[str, str], None]:
    """
    log performance item with pre-defined format to the console
    and record it under the ``properties`` tag in the junit report if available.
    """

    def real_func(item: str, value: str) -> None:
        """
        :param item: performance item name
        :param value: performance value
        """
        logging.info('[Performance][%s]: %s', item, value)
        record_property(item, value)

    return real_func


@pytest.fixture
def check_performance(idf_path: str) -> Callable[[str, float, str], None]:
    """
    check if the given performance item meets the passing standard or not
    """

    def real_func(item: str, value: float, target: str) -> None:
        """
        :param item: performance item name
        :param value: performance item value
        :param target: target chip
        :raise: AssertionError: if check fails
        """

        def _find_perf_item(operator: str, path: str) -> float:
            with open(path, 'r') as f:
                data = f.read()
            match = re.search(r'#define\s+IDF_PERFORMANCE_{}_{}\s+([\d.]+)'.format(operator, item.upper()), data)
            return float(match.group(1))  # type: ignore

        def _check_perf(operator: str, standard_value: float) -> None:
            if operator == 'MAX':
                ret = value <= standard_value
            else:
                ret = value >= standard_value
            if not ret:
                raise AssertionError(
                    "[Performance] {} value is {}, doesn't meet pass standard {}".format(item, value, standard_value)
                )

        path_prefix = os.path.join(idf_path, 'components', 'idf_test', 'include')
        performance_files = (
            os.path.join(path_prefix, target, 'idf_performance_target.h'),
            os.path.join(path_prefix, 'idf_performance.h'),
        )

        found_item = False
        for op in ['MIN', 'MAX']:
            for performance_file in performance_files:
                try:
                    standard = _find_perf_item(op, performance_file)
                except (IOError, AttributeError):
                    # performance file doesn't exist or match is not found in it
                    continue

                _check_perf(op, standard)
                found_item = True
                break

        if not found_item:
            raise AssertionError('Failed to get performance standard for {}'.format(item))

    return real_func


@pytest.fixture
def log_minimum_free_heap_size(dut: IdfDut, config: str) -> Callable[..., None]:
    def real_func() -> None:
        res = dut.expect(r'Minimum free heap size: (\d+) bytes')
        logging.info(
            '\n------ heap size info ------\n'
            '[app_name] {}\n'
            '[config_name] {}\n'
            '[target] {}\n'
            '[minimum_free_heap_size] {} Bytes\n'
            '------ heap size end ------'.format(
                os.path.basename(dut.app.app_path),
                config,
                dut.target,
                res.group(1).decode('utf8'),
            )
        )

    return real_func


##################
# Hook functions #
##################
def pytest_addoption(parser: pytest.Parser) -> None:
    base_group = parser.getgroup('idf')
    base_group.addoption(
        '--sdkconfig',
        help='sdkconfig postfix, like sdkconfig.ci.<config>. (Default: None, which would build all found apps)',
    )
    base_group.addoption('--known-failure-cases-file', help='known failure cases file path')


_idf_pytest_embedded_key = pytest.StashKey['IdfPytestEmbedded']()
_item_failed_cases_key = pytest.StashKey[list]()
_item_failed_key = pytest.StashKey[bool]()


def pytest_configure(config: Config) -> None:
    # cli option "--target"
    target = config.getoption('target') or ''

    help_commands = ['--help', '--fixtures', '--markers', '--version']
    for cmd in help_commands:
        if cmd in config.invocation_params.args:
            target = 'unneeded'
            break

    if not target:  # also could specify through markexpr via "-m"
        target = get_target_marker_from_expr(config.getoption('markexpr') or '')

    config.stash[_idf_pytest_embedded_key] = IdfPytestEmbedded(
        target=target,
        sdkconfig=config.getoption('sdkconfig'),
        known_failure_cases_file=config.getoption('known_failure_cases_file'),
    )
    config.pluginmanager.register(config.stash[_idf_pytest_embedded_key])

    for name, description in {**TARGET_MARKERS, **ENV_MARKERS, **SPECIAL_MARKERS}.items():
        config.addinivalue_line('markers', f'{name}: {description}')


def pytest_unconfigure(config: Config) -> None:
    _pytest_embedded = config.stash.get(_idf_pytest_embedded_key, None)
    if _pytest_embedded:
        del config.stash[_idf_pytest_embedded_key]
        config.pluginmanager.unregister(_pytest_embedded)


class IdfPytestEmbedded:
    def __init__(
        self,
        target: str,
        sdkconfig: Optional[str] = None,
        known_failure_cases_file: Optional[str] = None,
    ):
        # CLI options to filter the test cases
        self.target = target.lower()
        self.sdkconfig = sdkconfig
        self.known_failure_patterns = self._parse_known_failure_cases_file(known_failure_cases_file)

        self._failed_cases: List[Tuple[str, bool, bool]] = []  # (test_case_name, is_known_failure_cases, is_xfail)

    @property
    def failed_cases(self) -> List[str]:
        return [case for case, is_known, is_xfail in self._failed_cases if not is_known and not is_xfail]

    @property
    def known_failure_cases(self) -> List[str]:
        return [case for case, is_known, _ in self._failed_cases if is_known]

    @property
    def xfail_cases(self) -> List[str]:
        return [case for case, _, is_xfail in self._failed_cases if is_xfail]

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
        # same behavior for vanilla pytest-embedded '--target'
        session.config.option.target = self.target

    @pytest.hookimpl(tryfirst=True)
    def pytest_collection_modifyitems(self, items: List[Function]) -> None:
        # sort by file path and callspec.config
        # implement like this since this is a limitation of pytest, couldn't get fixture values while collecting
        # https://github.com/pytest-dev/pytest/discussions/9689
        # after sort the test apps, the test may use the app cache to reduce the flash times.
        def _get_param_config(_item: Function) -> str:
            if hasattr(_item, 'callspec'):
                return _item.callspec.params.get('config', DEFAULT_SDKCONFIG)  # type: ignore
            return DEFAULT_SDKCONFIG

        items.sort(key=lambda x: (os.path.dirname(x.path), _get_param_config(x)))

        # set default timeout 10 minutes for each case
        for item in items:
            if 'timeout' not in item.keywords:
                item.add_marker(pytest.mark.timeout(10 * 60))

        # add markers for special markers
        for item in items:
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
            if (
                self.target == 'esp32c2'
                and 'esp32c2' in item_marker_names(item)
                and 'xtal_26mhz' not in item_marker_names(item)
            ):
                item.add_marker('xtal_40mhz')

        # filter all the test cases with "nightly_run" marker
        if os.getenv('INCLUDE_NIGHTLY_RUN') == '1':
            # Do not filter nightly_run cases
            pass
        elif os.getenv('NIGHTLY_RUN') == '1':
            items[:] = [item for item in items if 'nightly_run' in item_marker_names(item)]
        else:
            items[:] = [item for item in items if 'nightly_run' not in item_marker_names(item)]

        # filter all the test cases with target and skip_targets
        items[:] = [
            item
            for item in items
            if self.target in item_marker_names(item) and self.target not in item_skip_targets(item)
        ]

        # filter all the test cases with cli option "config"
        if self.sdkconfig:
            items[:] = [item for item in items if _get_param_config(item) == self.sdkconfig]

    def pytest_runtest_makereport(self, item: Function, call: CallInfo[None]) -> Optional[TestReport]:
        report = TestReport.from_item_and_call(item, call)
        if item.stash.get(_item_failed_key, None) is None:
            item.stash[_item_failed_key] = False

        if report.outcome == 'failed':
            # Mark the failed test cases
            #
            # This hook function would be called in 3 phases, setup, call, teardown.
            # the report.outcome is the outcome of the single call of current phase, which is independent
            # the call phase outcome is the test result
            item.stash[_item_failed_key] = True

        if call.when == 'teardown':
            item_failed = item.stash[_item_failed_key]
            if item_failed:
                # unity real test cases
                failed_sub_cases = item.stash.get(_item_failed_cases_key, [])
                if failed_sub_cases:
                    for test_case_name in failed_sub_cases:
                        self._failed_cases.append((test_case_name, self._is_known_failure(test_case_name), False))
                else:  # the case iteself is failing
                    test_case_name = item.funcargs.get('test_case_name', '')
                    if test_case_name:
                        self._failed_cases.append(
                            (test_case_name, self._is_known_failure(test_case_name), report.keywords.get('xfail', False))
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

        failed_sub_cases = []
        target = item.funcargs['target']
        config = item.funcargs['config']
        for junit in junits:
            xml = ET.parse(junit)
            testcases = xml.findall('.//testcase')
            for case in testcases:
                # modify the junit files
                new_case_name = format_case_id(target, config, case.attrib['name'])
                case.attrib['name'] = new_case_name
                if 'file' in case.attrib:
                    case.attrib['file'] = case.attrib['file'].replace('/IDF/', '')  # our unity test framework

                # collect real failure cases
                if case.find('failure') is not None:
                    failed_sub_cases.append(new_case_name)

            xml.write(junit)

        item.stash[_item_failed_cases_key] = failed_sub_cases

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
