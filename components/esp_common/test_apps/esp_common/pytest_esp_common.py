# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import re

import pytest
from pytest_embedded import Dut

DEFAULT_TIMEOUT = 20
TEST_SUBMENU_PATTERN_PYTEST = re.compile(rb'\s+\((\d+)\)\s+"([^"]+)"\r?\n')


@pytest.mark.generic
@pytest.mark.supported_targets
@pytest.mark.parametrize(
    'config',
    [
        'default'
    ]
)
def test_esp_common(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('*')
    dut.expect_unity_test_output(timeout=300)


def run_multiple_stages(dut: Dut, test_case_num: int, stages: int) -> None:
    for stage in range(1, stages + 1):
        dut.write(str(test_case_num))
        dut.expect(TEST_SUBMENU_PATTERN_PYTEST, timeout=DEFAULT_TIMEOUT)
        dut.write(str(stage))
        if stage != stages:
            dut.expect_exact('Press ENTER to see the list of tests.')


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.parametrize(
    'config',
    [
        'esp32_psram'
    ]
)
def test_esp_common_psram_esp32(dut: Dut) -> None:
    extra_data = dut.parse_test_menu()
    for test_case in extra_data:
        if test_case.type != 'multi_stage':
            dut.write(str(test_case.index))
        else:
            run_multiple_stages(dut, test_case.index, len(test_case.subcases))
        dut.expect_unity_test_output(timeout=90)
        dut.expect_exact("Enter next test, or 'enter' to see menu")


@pytest.mark.generic
@pytest.mark.esp32s2
@pytest.mark.parametrize(
    'config',
    [
        'esp32s2_psram'
    ]
)
def test_esp_common_psram_esp32s2(dut: Dut) -> None:
    extra_data = dut.parse_test_menu()
    for test_case in extra_data:
        if test_case.type != 'multi_stage':
            dut.write(str(test_case.index))
        else:
            run_multiple_stages(dut, test_case.index, len(test_case.subcases))
        dut.expect_unity_test_output(timeout=90)
        dut.expect_exact("Enter next test, or 'enter' to see menu")
