# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded_idf.unity_tester import CaseTester
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.wifi_two_dut
@pytest.mark.parametrize('count', [2], indirect=True)
@idf_parametrize(
    'target',
    ['esp32', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32s2', 'esp32s3'],
    indirect=['target'],
)
def test_wifi_nvs_connect_cases(case_tester: CaseTester) -> None:  # type: ignore
    case_tester.run_all_cases()


@pytest.mark.wifi_two_dut
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, config, baud',
    [
        (2, 'esp32c2_xtal26m', '74880'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_wifi_nvs_connect_cases_esp32c2_xtal26m(case_tester: CaseTester) -> None:
    case_tester.run_all_cases()
