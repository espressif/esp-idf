# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s3
@pytest.mark.generic
def test_bdc_speed_control_example(dut: Dut) -> None:
    dut.expect_exact('example: Create DC motor')
    dut.expect_exact('example: Init pcnt driver to decode rotary signal')
    dut.expect_exact('example: Create PID control block')
    dut.expect_exact('example: Create a timer to do PID calculation periodically')
    dut.expect_exact('example: Enable motor')
    dut.expect_exact('example: Forward motor')
    dut.expect_exact('example: Start motor speed loop')
