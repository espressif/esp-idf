# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.unity_tester import CaseTester
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'target',
    ['esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c2', 'esp32c6', 'esp32c61', 'esp32c5'],
    indirect=['target'],
)
def test_wpa_supplicant_ut(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count',
    [
        2,
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_wpa_supplicant_ut_offchan(case_tester: CaseTester) -> None:
    for case in case_tester.test_menu:
        if case.attributes.get('test_env') == 'wifi_two_dut':
            case_tester.run_multi_dev_case(case=case, reset=True)


# test when external bss segment is enabled
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count, config',
    [
        (
            2,
            'ext_esp32s3',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_wpa_supplicant_ut_psram(case_tester: CaseTester) -> None:
    for case in case_tester.test_menu:
        if case.attributes.get('test_env') == 'wifi_two_dut':
            case_tester.run_multi_dev_case(case=case, reset=True)


@pytest.mark.wifi_two_dut
@pytest.mark.esp32c2eco4
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, config, baud',
    [
        (
            2,
            'esp32c2eco4_xtal26m',
            '74880',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_wpa_supplicant_esp32c2eco4_xtal26mhz(case_tester: CaseTester) -> None:
    for case in case_tester.test_menu:
        if case.attributes.get('test_env') == 'wifi_two_dut':
            case_tester.run_multi_dev_case(case=case, reset=True)


@pytest.mark.wifi_two_dut
@pytest.mark.esp32c3eco7
@pytest.mark.parametrize(
    'count, config',
    [
        (
            2,
            'esp32c3eco7',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_wpa_supplicant_esp32c3eco7(case_tester: CaseTester) -> None:
    for case in case_tester.test_menu:
        if case.attributes.get('test_env') == 'wifi_two_dut':
            case_tester.run_multi_dev_case(case=case, reset=True)
