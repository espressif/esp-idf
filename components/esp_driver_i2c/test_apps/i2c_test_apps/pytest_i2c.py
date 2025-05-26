# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
        'iram_safe',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_i2c(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic_multi_device
@pytest.mark.parametrize(
    'count, config',
    [
        (
            2,
            'defaults',
        ),
        (
            2,
            'release',
        ),
        (
            2,
            'iram_safe',
        ),
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4', 'esp32s2', 'esp32s3'],
    indirect=['target'],
)
def test_i2c_multi_device(case_tester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        if case.attributes.get('test_env', 'generic_multi_device') == 'generic_multi_device':
            case_tester.run_multi_dev_case(case=case, reset=True)


@pytest.mark.flash_suspend
@pytest.mark.parametrize(
    'config',
    [
        'i2c_isr_flash',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32c3'],
    indirect=['target'],
)
def test_flash_auto_suspend_for_i2c(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=30)
