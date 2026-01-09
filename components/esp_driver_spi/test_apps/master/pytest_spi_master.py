# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32c5'], reason='c5 eco2 does not support top pd')
@pytest.mark.parametrize(
    'config',
    [
        'release',
        'freertos_flash',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_master_single_dev(case_tester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        if 'test_env' in case.attributes:
            continue  # If `test_env` is defined, should not run on generic runner
        case_tester.run_normal_case(case=case, reset=True)


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'freertos_flash',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c5'], indirect=['target'])
def test_master_single_dev_esp32c5(case_tester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        if 'test_env' in case.attributes:
            continue  # If `test_env` is defined, should not run on generic runner
        case_tester.run_normal_case(case=case, reset=True)


@pytest.mark.generic
@pytest.mark.esp32c5_eco3
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c5'], indirect=['target'])
def test_master_single_dev_esp32c5_eco3(case_tester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        if 'test_env' in case.attributes:
            continue  # If `test_env` is defined, should not run on generic runner
        case_tester.run_normal_case(case=case, reset=True)


# Job for test_env `external_flash` just for esp32 only
@pytest.mark.flash_multi
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_master_esp_flash(case_tester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        # test case `spi_bus_lock_with_flash` use difference test env
        if case.attributes.get('test_env') == 'external_flash':
            case_tester.run_normal_case(case=case, reset=True)


@pytest.mark.generic_multi_device
@pytest.mark.parametrize(
    'count, config',
    [
        (2, 'release'),
        (2, 'freertos_flash'),
        (2, 'iram_safe'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_master_multi_dev(case_tester) -> None:  # type: ignore
    case_tester.run_all_multi_dev_cases(reset=True)
