# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.unity_tester import CaseTester


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c2
@pytest.mark.esp32c6
@pytest.mark.esp32c61
@pytest.mark.esp32c5
def test_wpa_supplicant_ut(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count',
    [
        2,
    ],
    indirect=True
)
def test_wpa_supplicant_ut_offchan(case_tester: CaseTester) -> None:
    for case in case_tester.test_menu:
        if case.attributes.get('test_env') == 'wifi_two_dut':
            case_tester.run_multi_dev_case(case=case, reset=True)
