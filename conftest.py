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

import glob
import json
import logging
import os
import re
import sys
from copy import deepcopy
from typing import Callable, Optional

import pytest
from _pytest.config import Config
from _pytest.fixtures import FixtureRequest
from pytest_embedded.plugin import multi_dut_argument, multi_dut_fixture
from pytest_embedded_idf.dut import IdfDut

try:
    from idf_ci_utils import IDF_PATH
    from idf_pytest.constants import DEFAULT_SDKCONFIG, ENV_MARKERS, SPECIAL_MARKERS, TARGET_MARKERS
    from idf_pytest.plugin import IDF_PYTEST_EMBEDDED_KEY, IdfPytestEmbedded
    from idf_pytest.utils import format_case_id, get_target_marker_from_expr
    from idf_unity_tester import CaseTester
except ImportError:
    sys.path.append(os.path.join(os.path.dirname(__file__), 'tools', 'ci'))
    from idf_ci_utils import IDF_PATH
    from idf_pytest.constants import DEFAULT_SDKCONFIG, ENV_MARKERS, SPECIAL_MARKERS, TARGET_MARKERS
    from idf_pytest.plugin import IDF_PYTEST_EMBEDDED_KEY, IdfPytestEmbedded
    from idf_pytest.utils import format_case_id, get_target_marker_from_expr
    from idf_unity_tester import CaseTester

try:
    import common_test_methods  # noqa: F401
except ImportError:
    sys.path.append(os.path.join(os.path.dirname(__file__), 'tools', 'ci', 'python_packages'))
    import common_test_methods  # noqa: F401


############
# Fixtures #
############
@pytest.fixture(scope='session')
def idf_path() -> str:
    return os.path.dirname(__file__)


@pytest.fixture(scope='session')
def session_root_logdir(idf_path: str) -> str:
    """Session scoped log dir for pytest-embedded"""
    return idf_path


@pytest.fixture
def case_tester(dut: IdfDut, **kwargs):  # type: ignore
    yield CaseTester(dut, **kwargs)


@pytest.fixture
@multi_dut_argument
def config(request: FixtureRequest) -> str:
    return getattr(request, 'param', None) or DEFAULT_SDKCONFIG  # type: ignore


@pytest.fixture
def test_func_name(request: FixtureRequest) -> str:
    return request.node.function.__name__  # type: ignore


@pytest.fixture
def test_case_name(request: FixtureRequest, target: str, config: str) -> str:
    is_qemu = request._pyfuncitem.get_closest_marker('qemu') is not None
    if hasattr(request._pyfuncitem, 'callspec'):
        params = deepcopy(request._pyfuncitem.callspec.params)  # type: ignore
    else:
        params = {}

    filtered_params = {}
    for k, v in params.items():
        if k not in request.session._fixturemanager._arg2fixturedefs:  # type: ignore
            filtered_params[k] = v  # not fixture ones

    return format_case_id(target, config, request.node.originalname, is_qemu=is_qemu, params=filtered_params)  # type: ignore


@pytest.fixture
@multi_dut_fixture
def build_dir(app_path: str, target: Optional[str], config: Optional[str]) -> str:
    """
    Check local build dir with the following priority:

    1. build_<target>_<config>
    2. build_<target>
    3. build_<config>
    4. build

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
            logging.info(f'found valid binary path: {binary_path}')
            return check_dir

        logging.warning('checking binary path: %s... missing... try another place', binary_path)

    raise ValueError(
        f'no build dir valid. Please build the binary via "idf.py -B {check_dirs[0]} build" and run pytest again'
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


@pytest.fixture
def dev_password(request: FixtureRequest) -> str:
    return request.config.getoption('dev_passwd') or ''


@pytest.fixture
def dev_user(request: FixtureRequest) -> str:
    return request.config.getoption('dev_user') or ''


##################
# Hook functions #
##################
def pytest_addoption(parser: pytest.Parser) -> None:
    idf_group = parser.getgroup('idf')
    idf_group.addoption(
        '--sdkconfig',
        help='sdkconfig postfix, like sdkconfig.ci.<config>. (Default: None, which would build all found apps)',
    )
    idf_group.addoption(
        '--dev-user',
        help='user name associated with some specific device/service used during the test execution',
    )
    idf_group.addoption(
        '--dev-passwd',
        help='password associated with some specific device/service used during the test execution',
    )
    idf_group.addoption(
        '--app-info-basedir',
        default=IDF_PATH,
        help='app info base directory. specify this value when you\'re building under a '
             'different IDF_PATH. (Default: $IDF_PATH)',
    )
    idf_group.addoption(
        '--app-info-filepattern',
        help='glob pattern to specify the files that include built app info generated by '
             '`idf-build-apps --collect-app-info ...`. will not raise ValueError when binary '
             'paths not exist in local file system if not listed recorded in the app info.',
    )


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

    apps_list = None
    app_info_basedir = config.getoption('app_info_basedir')
    app_info_filepattern = config.getoption('app_info_filepattern')
    if app_info_filepattern:
        apps_list = []
        for file in glob.glob(os.path.join(IDF_PATH, app_info_filepattern)):
            with open(file) as fr:
                for line in fr.readlines():
                    if not line.strip():
                        continue

                    # each line is a valid json
                    app_info = json.loads(line.strip())
                    if app_info_basedir and app_info['app_dir'].startswith(app_info_basedir):
                        relative_app_dir = os.path.relpath(app_info['app_dir'], app_info_basedir)
                        apps_list.append(os.path.join(IDF_PATH, os.path.join(relative_app_dir, app_info['build_dir'])))
                        print('Detected app: ', apps_list[-1])
                    else:
                        print(
                            f'WARNING: app_info base dir {app_info_basedir} not recognizable in {app_info["app_dir"]}, skipping...'
                        )
                        continue

    config.stash[IDF_PYTEST_EMBEDDED_KEY] = IdfPytestEmbedded(
        target=target,
        sdkconfig=config.getoption('sdkconfig'),
        apps_list=apps_list,
    )
    config.pluginmanager.register(config.stash[IDF_PYTEST_EMBEDDED_KEY])

    for name, description in {**TARGET_MARKERS, **ENV_MARKERS, **SPECIAL_MARKERS}.items():
        config.addinivalue_line('markers', f'{name}: {description}')


def pytest_unconfigure(config: Config) -> None:
    _pytest_embedded = config.stash.get(IDF_PYTEST_EMBEDDED_KEY, None)
    if _pytest_embedded:
        del config.stash[IDF_PYTEST_EMBEDDED_KEY]
        config.pluginmanager.unregister(_pytest_embedded)
