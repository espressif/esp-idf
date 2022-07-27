# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s3
@pytest.mark.generic
def test_bldc_hall_control_example(dut: Dut) -> None:
    dut.expect_exact('example: Disable MOSFET gate')
    dut.expect_exact('example: Create MCPWM timer')
    dut.expect_exact('example: Create MCPWM operator')
    dut.expect_exact('example: Connect operators to the same timer')
    dut.expect_exact('example: Create comparators')
    dut.expect_exact('example: Create over current fault detector')
    dut.expect_exact('example: Set brake mode on the fault event')
    dut.expect_exact('example: Create PWM generators')
    dut.expect_exact('example: Set generator actions')
    dut.expect_exact('example: Setup deadtime')
    dut.expect_exact('example: Turn off all the gates')
    dut.expect_exact('example: Create Hall sensor capture channels')
    dut.expect_exact('example: Start a timer to adjust motor speed periodically')
    dut.expect_exact('example: Enable MOSFET gate')
    dut.expect_exact('example: Start the MCPWM timer')
