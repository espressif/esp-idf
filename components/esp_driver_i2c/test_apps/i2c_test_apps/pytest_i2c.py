# SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='esp32p4 support TBD')  # TODO: IDF-8960
@pytest.mark.parametrize(
    'config',
    [
        'release',
        'iram_safe',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_i2c(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic_multi_device
@pytest.mark.parametrize(
    'count, config',
    [
        (
            2,
            'defaults',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32c3', 'esp32c6', 'esp32h2', 'esp32s2', 'esp32s3'], indirect=['target'])
def test_i2c_multi_device(case_tester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        if case.attributes.get('test_env', 'generic_multi_device') == 'generic_multi_device':
            case_tester.run_multi_dev_case(case=case, reset=True)


@pytest.mark.generic_multi_device
@pytest.mark.parametrize(
    'count, config',
    [
        (
            2,
            'sleep_retention',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c6', 'esp32h2'], indirect=['target'])
def test_i2c_sleep_retention(case_tester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        if case.attributes.get('test_env', 'generic_multi_device') == 'generic_multi_device':
            case_tester.run_multi_dev_case(case=case, reset=True, timeout=250)
