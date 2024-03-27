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
def test_servo_mg996r_example(dut: Dut) -> None:
    dut.expect_exact('example: Create timer and operator')
    dut.expect_exact('example: Connect timer and operator')
    dut.expect_exact('example: Create comparator and generator from the operator')
    dut.expect_exact('example: Set generator action on timer and compare event')
    dut.expect_exact('example: Enable and start timer')
    dut.expect_exact('example: Angle of rotation: 0')
