# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded_idf.unity_tester import CaseTester


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c5
@pytest.mark.esp32c6
@pytest.mark.esp32c61
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize('count', [2], indirect=True)
def test_wifi_connect_cases(case_tester: CaseTester) -> None:  # type: ignore
    case_tester.run_all_cases()


@pytest.mark.esp32c2
@pytest.mark.wifi_two_dut
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, config, baud',
    [
        (2, 'esp32c2_xtal26m', '74880'),
    ],
    indirect=True,
)
def test_wifi_connect_cases_esp32c2_xtal26m(case_tester: CaseTester) -> None:
    case_tester.run_all_cases()
