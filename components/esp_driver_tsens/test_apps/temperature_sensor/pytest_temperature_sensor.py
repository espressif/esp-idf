# SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from idf_unity_tester import CaseTester
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.esp32c2
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.generic
@pytest.mark.parametrize('config', [
    'release',
], indirect=True)
def test_temperature_sensor_driver(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.generic
@pytest.mark.parametrize('config', [
    'iram_safe',
], indirect=True)
def test_temperature_sensor_cbs(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32s2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.esp32c2
@pytest.mark.esp32c6
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize('count', [2], indirect=True)
def test_temperature_phy_cases(case_tester: CaseTester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        if case.attributes.get('test_env', 'wifi_two_dut') == 'wifi_two_dut':
            case_tester.run_all_multi_dev_cases(case=case, reset=True)
