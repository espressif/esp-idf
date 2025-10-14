# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32s3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4'], indirect=['target'])
def test_servo_mg996r_example(dut: Dut) -> None:
    dut.expect_exact('example: Create timer and operator')
    dut.expect_exact('example: Connect timer and operator')
    dut.expect_exact('example: Create comparator and generator from the operator')
    dut.expect_exact('example: Set generator action on timer and compare event')
    dut.expect_exact('example: Enable and start timer')
    dut.expect_exact('example: Angle of rotation: 0')
