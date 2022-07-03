# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s3
@pytest.mark.generic
def test_ir_nec_example(dut: Dut) -> None:
    dut.expect_exact('example: Initialize EN + DIR GPIO')
    dut.expect_exact('example: Create RMT TX channel')
    dut.expect_exact('example: Set spin direction')
    dut.expect_exact('example: Enable step motor')
    dut.expect_exact('example: Create motor encoders')
    dut.expect_exact('example: Enable RMT channel')
    dut.expect_exact('example: Spin motor for 6000 steps: 500 accel + 5000 uniform + 500 decel')
