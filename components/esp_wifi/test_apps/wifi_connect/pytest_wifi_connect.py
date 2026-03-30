# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded_idf.unity_tester import CaseTester
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.two_duts
@pytest.mark.parametrize('count', [2], indirect=True)
@idf_parametrize(
    'target', ['esp32', 'esp32c3', 'esp32s2', 'esp32s3', 'esp32c5', 'esp32c6', 'esp32c61'], indirect=['target']
)
def test_wifi_connect_cases(case_tester: CaseTester) -> None:  # type: ignore
    case_tester.run_all_cases()


@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, config, baud',
    [
        (2, 'esp32c2_xtal26m', '74880'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_wifi_connect_cases_esp32c2_xtal26m(case_tester: CaseTester) -> None:
    case_tester.run_all_cases()


@pytest.mark.esp32c2_rev2
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, config, baud',
    [
        (2, 'esp32c2_rev2_xtal26m', '74880'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_wifi_connect_cases_esp32c2_rev2_xtal26m(case_tester: CaseTester) -> None:
    case_tester.run_all_cases()


@pytest.mark.two_duts
@pytest.mark.esp32c3_rev1
@pytest.mark.parametrize(
    'count, config',
    [
        (2, 'esp32c3_rev1'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_wifi_connect_cases_esp32c3_rev1(case_tester: CaseTester) -> None:
    case_tester.run_all_cases()


# Must match TEST_CASE_MULTIPLE_DEVICES name string in test_connect_passive_channel_hidden_ap.c (menu line quoted text).
_PASSIVE_HIDDEN_AP_CASE = 'connect passive channel hidden softap'


@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, config',
    [
        (2, 'passive_hidden_ap'),
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32', 'esp32c3', 'esp32s2', 'esp32s3', 'esp32c5', 'esp32c6', 'esp32c61'],
    indirect=['target'],
)
def test_wifi_connect_passive_hidden_ap(case_tester: CaseTester) -> None:
    """Firmware uses sdkconfig.ci.passive_hidden_ap; runs only the passive hidden AP multi-DUT case."""
    for case in case_tester.test_menu:
        if case.name == _PASSIVE_HIDDEN_AP_CASE:
            case_tester.run_multi_dev_case(case=case, reset=True)
            return
    pytest.fail(
        f'Case {_PASSIVE_HIDDEN_AP_CASE!r} not in menu; build must merge sdkconfig.ci.passive_hidden_ap '
        '(CONFIG_ESP_WIFI_PASSIVE_HIDDEN_AP_SUPPORT).'
    )
