# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
@pytest.mark.temp_skip_ci(targets=['esp32c5'], reason='C5 has not supported deep sleep')  # TODO: [ESP32C5] IDF-8640, IDF-10317
@pytest.mark.generic
def test_app_update(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=90)
