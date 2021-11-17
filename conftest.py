# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# pylint: disable=W0621

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
from typing import List, Optional

import pytest
from _pytest.config import Config
from _pytest.fixtures import FixtureRequest
from pytest_embedded.plugin import parse_configuration


def _is_target_marker(marker: str) -> bool:
    if marker.startswith('esp32'):
        return True

    if marker.startswith('esp8'):
        return True

    return False


@pytest.fixture(scope='session')
def target_markers(pytestconfig: Config) -> List[str]:
    res = []
    for item in pytestconfig.getini('markers'):
        marker = item.split(':')[0]
        if _is_target_marker(marker):
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


@pytest.fixture(scope='session')
def param_markers(pytestconfig: Config) -> List[str]:
    res: List[str] = []
    offset = -1
    while True:
        try:
            offset = pytestconfig.invocation_params.args.index('-m', offset + 1)
        except ValueError:
            return res
        res.append(pytestconfig.invocation_params.args[offset + 1])  # we want the marker after '-m'


@pytest.fixture
def target(request: FixtureRequest, target_markers: List[str], param_markers: List[str]) -> Optional[str]:
    param_target_markers = [marker for marker in param_markers if marker in target_markers]
    if len(param_target_markers) > 1:
        raise ValueError('Please only specify one target marker at the same time')
    elif len(param_target_markers) == 0:
        target = None
    else:
        target = param_target_markers[0]

    return getattr(request, 'param', None) or target


@pytest.fixture
def config(request: FixtureRequest) -> Optional[str]:
    return getattr(request, 'param', None) or request.config.option.__dict__.get('config') or None


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
