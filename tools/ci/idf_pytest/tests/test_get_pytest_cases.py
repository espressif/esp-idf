# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from pathlib import Path

from idf_pytest.constants import CollectMode
from idf_pytest.script import get_pytest_cases

TEMPLATE_SCRIPT = '''
import pytest

@pytest.mark.esp32
@pytest.mark.esp32s2
def test_foo_single(dut):
    pass

@pytest.mark.parametrize(
    'count, target', [
        (2, 'esp32|esp32s2'),
        (3, 'esp32s2|esp32s2|esp32s3'),
    ], indirect=True
)
def test_foo_multi(dut):
    pass

@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.parametrize(
    'count', [2], indirect=True
)
def test_foo_multi_with_marker(dut):
    pass
'''


def test_get_pytest_cases_single_specific(work_dirpath: Path) -> None:
    script = work_dirpath / 'pytest_get_pytest_cases_single_specific.py'
    script.write_text(TEMPLATE_SCRIPT)
    cases = get_pytest_cases([str(work_dirpath)], 'esp32')

    assert len(cases) == 1
    assert cases[0].targets == ['esp32']


def test_get_pytest_cases_multi_specific(work_dirpath: Path) -> None:
    script = work_dirpath / 'pytest_get_pytest_cases_multi_specific.py'
    script.write_text(TEMPLATE_SCRIPT)
    cases = get_pytest_cases([str(work_dirpath)], 'esp32s2,esp32s2, esp32s3')

    assert len(cases) == 1
    assert cases[0].targets == ['esp32s2', 'esp32s2', 'esp32s3']

    cases = get_pytest_cases([str(work_dirpath)], 'esp32s3,esp32s2,esp32s2')  # order matters
    assert len(cases) == 0


def test_get_pytest_cases_multi_all(work_dirpath: Path) -> None:
    script = work_dirpath / 'pytest_get_pytest_cases_multi_all.py'
    script.write_text(TEMPLATE_SCRIPT)
    cases = get_pytest_cases([str(work_dirpath)], CollectMode.MULTI_ALL_WITH_PARAM)

    assert len(cases) == 2
    assert cases[0].targets == ['esp32', 'esp32s2']
    assert cases[1].targets == ['esp32s2', 'esp32s2', 'esp32s3']


def test_get_pytest_cases_all(work_dirpath: Path) -> None:
    script = work_dirpath / 'pytest_get_pytest_cases_all.py'
    script.write_text(TEMPLATE_SCRIPT)
    cases = get_pytest_cases([str(work_dirpath)], CollectMode.ALL)

    assert len(cases) == 6
    assert cases[0].targets == ['esp32', 'esp32s2']
    assert cases[0].name == 'test_foo_multi'

    assert cases[1].targets == ['esp32s2', 'esp32s2', 'esp32s3']
    assert cases[1].name == 'test_foo_multi'

    assert cases[2].targets == ['esp32', 'esp32']
    assert cases[2].name == 'test_foo_multi_with_marker'

    assert cases[3].targets == ['esp32s2', 'esp32s2']
    assert cases[3].name == 'test_foo_multi_with_marker'

    assert cases[4].targets == ['esp32']
    assert cases[4].name == 'test_foo_single'

    assert cases[5].targets == ['esp32s2']
    assert cases[5].name == 'test_foo_single'
