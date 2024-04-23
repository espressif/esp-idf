# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import re

import pytest
from pytest_embedded import Dut

DEFAULT_TIMEOUT = 20
TEST_SUBMENU_PATTERN_PYTEST = re.compile(rb'\s+\((\d+)\)\s+"([^"]+)"\r?\n')


def run_multiple_stages(dut: Dut, test_case_num: int, stages: int) -> None:
    for stage in range(1, stages + 1):
        dut.write(str(test_case_num))
        dut.expect(TEST_SUBMENU_PATTERN_PYTEST, timeout=DEFAULT_TIMEOUT)
        dut.write(str(stage))
        if stage != stages:
            dut.expect_exact('Press ENTER to see the list of tests.')


@pytest.mark.supported_targets
@pytest.mark.temp_skip_ci(targets=['esp32c6', 'esp32h2'], reason='c6/h2 support TBD')
@pytest.mark.generic
def test_app_update(dut: Dut) -> None:
    extra_data = dut.parse_test_menu()
    for test_case in extra_data:
        if test_case.type != 'multi_stage':
            dut.write(str(test_case.index))
        else:
            run_multiple_stages(dut, test_case.index, len(test_case.subcases))
        dut.expect_unity_test_output(timeout=90)
        dut.expect_exact("Enter next test, or 'enter' to see menu")
