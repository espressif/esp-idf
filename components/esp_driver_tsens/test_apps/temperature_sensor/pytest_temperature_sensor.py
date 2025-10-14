# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.unity_tester import CaseTester
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32s2', 'esp32c3', 'esp32s3', 'esp32c2', 'esp32c6', 'esp32h2', 'esp32p4', 'esp32c5', 'esp32c61'],
    indirect=['target'],
)
def test_temperature_sensor_driver(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c6', 'esp32h2', 'esp32p4', 'esp32c5', 'esp32c61'], indirect=['target'])
def test_temperature_sensor_cbs(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.wifi_two_dut
@pytest.mark.parametrize('count', [2], indirect=True)
@idf_parametrize('target', ['esp32s2', 'esp32c3', 'esp32s3', 'esp32c2', 'esp32c6', 'esp32c61'], indirect=['target'])
def test_temperature_phy_cases(case_tester: CaseTester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        if case.attributes.get('test_env', 'wifi_two_dut') == 'wifi_two_dut':
            case_tester.run_all_multi_dev_cases(case=case, reset=True)
