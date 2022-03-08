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
from typing import Callable, List, Optional

import pytest
from _pytest.config import Config
from _pytest.fixtures import FixtureRequest
from _pytest.nodes import Item
from _pytest.python import Function
from pytest_embedded.plugin import parse_configuration
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
def config(request: FixtureRequest) -> str:
    return getattr(request, 'param', None) or DEFAULT_SDKCONFIG


@pytest.fixture
def test_func_name(request: FixtureRequest) -> str:
    return request.node.function.__name__  # type: ignore


@pytest.fixture
def test_case_name(request: FixtureRequest, target: str, config: str) -> str:
    return format_case_id(target, config, request.node.originalname)


@pytest.fixture
@parse_configuration
def build_dir(
    request: FixtureRequest, app_path: str, target: Optional[str], config: Optional[str]
) -> str:
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
    param_or_cli: str = getattr(
        request, 'param', None
    ) or request.config.option.__dict__.get('build_dir')
    if param_or_cli is not None:  # respect the param and the cli
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

        logging.warning(
            'checking binary path: %s... missing... try another place', binary_path
        )

    recommend_place = check_dirs[0]
    logging.error(
        f'no build dir valid. Please build the binary via "idf.py -B {recommend_place} build" and run pytest again'
    )
    sys.exit(1)


@pytest.fixture(autouse=True)
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


@pytest.hookimpl(tryfirst=True)
def pytest_collection_modifyitems(config: Config, items: List[Function]) -> None:
    target = config.getoption('target', None)  # use the `build` dir
    if not target:
        return

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

    # filter all the test cases with "--target"
    items[:] = [item for item in items if target in item_marker_names(item)]

    # filter all the test cases with cli option "config"
    if config.getoption('sdkconfig'):
        items[:] = [
            item
            for item in items
            if _get_param_config(item) == config.getoption('sdkconfig')
        ]


@pytest.hookimpl(trylast=True)
def pytest_runtest_teardown(item: Function) -> None:
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
            case.attrib['name'] = format_case_id(target, config, case.attrib['name'])
            if 'file' in case.attrib:
                case.attrib['file'] = case.attrib['file'].replace(
                    '/IDF/', ''
                )  # our unity test framework
        xml.write(junit)
