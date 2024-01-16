# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from pathlib import Path

from idf_pytest.script import get_all_apps
from idf_pytest.script import SUPPORTED_TARGETS

from conftest import create_project


def test_get_all_apps_non(tmp_path: Path) -> None:
    create_project('foo', tmp_path)
    create_project('bar', tmp_path)

    test_related_apps, non_test_related_apps = get_all_apps([str(tmp_path)])

    assert test_related_apps == set()
    assert len(non_test_related_apps) == 2 * len(SUPPORTED_TARGETS)


def test_get_all_apps_single_dut_test_script(tmp_path: Path) -> None:
    create_project('foo', tmp_path)
    with open(tmp_path / 'foo' / 'pytest_get_all_apps_single_dut_test_script.py', 'w') as fw:
        fw.write(
            """import pytest

@pytest.mark.esp32
@pytest.mark.esp32s2
def test_foo(dut):
    pass
"""
        )
    create_project('bar', tmp_path)

    test_related_apps, non_test_related_apps = get_all_apps([str(tmp_path)], target='all')

    assert len(test_related_apps) == 2
    assert len(non_test_related_apps) == 2 * len(SUPPORTED_TARGETS) - 2


def test_get_all_apps_multi_dut_with_markers_test_script(tmp_path: Path) -> None:
    create_project('foo', tmp_path)

    (tmp_path / 'foo' / 'pytest_get_all_apps_multi_dut_with_markers_test_script.py').write_text(
        """import pytest

@pytest.mark.esp32
@pytest.mark.parametrize('count', [2, 3], indirect=True)
def test_foo(dut):
    pass
""",
        encoding='utf-8',
    )

    test_related_apps, non_test_related_apps = get_all_apps([str(tmp_path)], target='all')

    assert len(test_related_apps) == 1
    assert len(non_test_related_apps) == len(SUPPORTED_TARGETS) - 1


def test_get_all_apps_multi_dut_test_script(tmp_path: Path) -> None:
    create_project('foo', tmp_path)
    with open(tmp_path / 'foo' / 'pytest_get_all_apps_multi_dut_test_script.py', 'w') as fw:
        fw.write(
            """import pytest

@pytest.mark.parametrize(
    'count, target', [
        (2, 'esp32s2|esp32s3'),
        (3, 'esp32|esp32s3|esp32'),
    ], indirect=True
)
def test_foo(dut):
    pass
"""
        )

    test_related_apps, non_test_related_apps = get_all_apps([str(tmp_path)], target='all')

    assert len(test_related_apps) == 3  # 32, s2, s3
    assert len(non_test_related_apps) == len(SUPPORTED_TARGETS) - 3


def test_get_all_apps_modified_pytest_script(tmp_path: Path) -> None:
    create_project('foo', tmp_path)
    create_project('bar', tmp_path)

    (tmp_path / 'pytest_get_all_apps_modified_pytest_script.py').write_text(
        """import pytest
import os

@pytest.mark.parametrize('count, target', [(2, 'esp32')], indirect=True)
@pytest.mark.parametrize('app_path', [
        '{}|{}'.format(os.path.join(os.path.dirname(__file__), 'foo'), os.path.join(os.path.dirname(__file__), 'bar')),
    ], indirect=True
)
def test_multi_foo_bar(dut):
    pass
""",
        encoding='utf-8',
    )

    test_related_apps, non_test_related_apps = get_all_apps([str(tmp_path)], target='all')
    assert len(test_related_apps) == 2  # foo-esp32, bar-esp32
    assert len(non_test_related_apps) == 2 * len(SUPPORTED_TARGETS) - 2

    test_related_apps, non_test_related_apps = get_all_apps(
        [str(tmp_path)], target='all', modified_files=[], modified_components=[]
    )
    assert len(test_related_apps) == 0
    assert len(non_test_related_apps) == 0

    test_related_apps, non_test_related_apps = get_all_apps(
        [str(tmp_path)],
        target='all',
        modified_files=[str(tmp_path / 'pytest_get_all_apps_modified_pytest_script.py')],
        modified_components=[],
    )
    assert len(test_related_apps) == 2
    assert len(non_test_related_apps) == 0
