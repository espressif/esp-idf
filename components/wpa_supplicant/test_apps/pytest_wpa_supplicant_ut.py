# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
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
def test_wpa_supplicant_ut(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count',
    [
        2,
    ],
    indirect=True
)
def test_wpa_supplicant_ut_offchan(case_tester: CaseTester) -> None:
    for case in case_tester.test_menu:
        if case.attributes.get('test_env') == 'two_duts':
            case_tester.run_multi_dev_case(case=case, reset=True)


@pytest.mark.two_duts
@pytest.mark.esp32c2eco4
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, config, target, baud',
    [
        (
            2,
            'esp32c2eco4_xtal26m',
            'esp32c2',
            '74880',
        ),
    ],
    indirect=True,
)
def test_wpa_supplicant_esp32c2eco4_xtal26mhz(case_tester: CaseTester) -> None:
    for case in case_tester.test_menu:
        if case.attributes.get('test_env') == 'two_duts':
            case_tester.run_multi_dev_case(case=case, reset=True)


@pytest.mark.two_duts
@pytest.mark.esp32c3eco7
@pytest.mark.parametrize(
    'count, config, target',
    [
        (
            2,
            'esp32c3eco7',
            'esp32c3',
        ),
    ],
    indirect=True,
)
def test_wpa_supplicant_esp32c3eco7(case_tester: CaseTester) -> None:
    for case in case_tester.test_menu:
        if case.attributes.get('test_env') == 'two_duts':
            case_tester.run_multi_dev_case(case=case, reset=True)
