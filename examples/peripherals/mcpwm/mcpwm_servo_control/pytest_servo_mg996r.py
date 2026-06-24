# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_MCPWM_SUPPORTED == 1'),
    indirect=['target'],
)
def test_servo_mg996r_example(dut: Dut) -> None:
    dut.expect_exact('example: Create timer and operator')
    dut.expect_exact('example: Connect timer and operator')
    dut.expect_exact('example: Create comparator and generator from the operator')
    dut.expect_exact('example: Set generator action on timer and compare event')
    dut.expect_exact('example: Enable and start timer')
    dut.expect_exact('example: Angle of rotation: 0')
