# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_open_foc(dut: Dut) -> None:
    dut.expect_exact('example_foc: Hello FOC')
    dut.expect_exact('example_foc: Inverter init OK')
    dut.expect_exact('example_foc: Inverter start OK')
    dut.expect_exact('example_foc: Enable MOSFET gate')
    dut.expect_exact('example_foc: Start FOC')
