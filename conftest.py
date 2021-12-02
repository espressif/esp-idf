# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
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
from typing import Callable, List, Optional

import pytest
from _pytest.config import Config
from _pytest.fixtures import FixtureRequest
from _pytest.nodes import Item
from pytest_embedded.plugin import parse_configuration
from pytest_embedded_idf.app import IdfApp


##################
# Help Functions #
##################
def is_target_marker(marker: str) -> bool:
    if marker.startswith('esp32'):
        return True

    if marker.startswith('esp8'):
        return True

    return False


def format_case_id(target: str, config: str, case: str) -> str:
    return f'{target}.{config}.{case}'


############
# Fixtures #
############
@pytest.fixture(scope='session')
def target_markers(pytestconfig: Config) -> List[str]:
    res = []
    for item in pytestconfig.getini('markers'):
        marker = item.split(':')[0]
        if is_target_marker(marker):
            res.append(marker)
    return res


@pytest.fixture(scope='session')
def env_markers(pytestconfig: Config) -> List[str]:
    res = []
    for item in pytestconfig.getini('markers'):
        marker = item.split(':')[0]
        if not marker.startswith('esp32'):
            res.append(marker)
    return res


@pytest.fixture
def config(request: FixtureRequest) -> str:
    return getattr(request, 'param', None) or request.config.getoption('config', 'default')  # type: ignore


@pytest.fixture
@parse_configuration
def build_dir(request: FixtureRequest, app_path: str, target: Optional[str], config: Optional[str]) -> str:
    """
    Check local build dir with the following priority:

    1. build_<target>_<config>
    2. build_<target>
    3. build_<config>
    4. build

    Args:
        request: pytest fixture
        app_path: app path
        target: target
        config: config

    Returns:
        valid build directory
    """
    param_or_cli: str = getattr(request, 'param', None) or request.config.option.__dict__.get('build_dir')
    if param_or_cli is not None:  # respect the parametrize and the cli
        return param_or_cli

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

        logging.warning(f'checking binary path: {binary_path}... missing... try another place')

    recommend_place = check_dirs[0]
    logging.error(
        f'no build dir valid. Please build the binary via "idf.py -B {recommend_place} build" and run pytest again')
    sys.exit(1)


@pytest.fixture(autouse=True)
def junit_properties(app: IdfApp, config: str, test_case_name: str,
                     record_xml_attribute: Callable[[str, object], None]) -> None:
    """
    This fixture is autoused and will modify the junit report test case name to <target>.<config>.<case_name>
    """
    record_xml_attribute('name', format_case_id(app.target, config, test_case_name))


##################
# Hook functions #
##################
@pytest.hookimpl(trylast=True)
def pytest_collection_modifyitems(config: Config, items: List[Item]) -> None:
    target = config.getoption('target', None)
    if not target:
        return

    # filter all the test cases with "--target"
    items[:] = [item for item in items if target in [marker.name for marker in item.iter_markers()]]
