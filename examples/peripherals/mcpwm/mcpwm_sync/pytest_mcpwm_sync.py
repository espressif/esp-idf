# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.generic
@pytest.mark.parametrize('config', [
    pytest.param('gpio', marks=[pytest.mark.esp32, pytest.mark.esp32s3]),
    pytest.param('tez', marks=[pytest.mark.esp32, pytest.mark.esp32s3]),
    pytest.param('soft', marks=[pytest.mark.esp32s3]),
], indirect=True)
def test_mcpwm_sync_example(dut: Dut) -> None:
    dut.expect_exact('example: Create timers')
    dut.expect_exact('example: Create operators')
    dut.expect_exact('example: Create comparators')
    dut.expect_exact('example: Create generators')
    dut.expect_exact('example: Set generator actions on timer and compare event')
    dut.expect_exact('example: Start timers one by one, so they are not synced')
    dut.expect_exact('example: Setup sync strategy')
    dut.expect_exact('example: Now the output PWMs should in sync')
