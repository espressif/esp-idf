# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s3
@pytest.mark.generic
def test_open_foc(dut: Dut) -> None:
    dut.expect_exact('example_foc: Hello FOC')
    dut.expect_exact('example_foc: Inverter init OK')
    dut.expect_exact('example_foc: Inverter start OK')
    dut.expect_exact('example_foc: Enable MOSFET gate')
    dut.expect_exact('example_foc: Start FOC')
