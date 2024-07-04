# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
# TODO: [ESP32P4] IDF-8960, [ESP32C5] IDF-10332
@pytest.mark.temp_skip_ci(targets=['esp32p4', 'esp32c5'], reason='esp32p4 support TBD, c5 test failed')
@pytest.mark.parametrize(
    'config',
    [
        'release',
        'iram_safe',
    ],
    indirect=True,
)
def test_i2c(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic_multi_device
@pytest.mark.parametrize(
    'count, config',
    [
        (2, 'defaults',),
    ],
    indirect=True
)
def test_i2c_multi_device(case_tester) -> None:        # type: ignore
    for case in case_tester.test_menu:
        if case.attributes.get('test_env', 'generic_multi_device') == 'generic_multi_device':
            case_tester.run_multi_dev_case(case=case, reset=True)


@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.generic_multi_device
@pytest.mark.parametrize(
    'count, config',
    [
        (2, 'sleep_retention',),
    ],
    indirect=True
)
def test_i2c_sleep_retention(case_tester) -> None:        # type: ignore
    for case in case_tester.test_menu:
        if case.attributes.get('test_env', 'generic_multi_device') == 'generic_multi_device':
            case_tester.run_multi_dev_case(case=case, reset=True, timeout=250)