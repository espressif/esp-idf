# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_RMT_SUPPORT_TX_LOOP_AUTO_STOP == 1'), indirect=['target'])
def test_stepper_motor_example(dut: Dut) -> None:
    dut.expect_exact('example: Initialize EN + DIR GPIO')
    dut.expect_exact('example: Create RMT TX channel')
    dut.expect_exact('example: Set spin direction')
    dut.expect_exact('example: Enable step motor')
    dut.expect_exact('example: Create motor encoders')
    dut.expect_exact('example: Enable RMT channel')
    dut.expect_exact('example: Spin motor for 6000 steps: 500 accel + 5000 uniform + 500 decel')
