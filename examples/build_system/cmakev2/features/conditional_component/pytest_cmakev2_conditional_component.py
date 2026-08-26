# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_cmakev2_conditional_component(dut: IdfDut) -> None:
    dut.expect_exact('Conditional Component Example')

    # With default config, both logging and math components are enabled
    dut.expect_exact('Logging component is ENABLED')
    dut.expect_exact('This is an info message from logging_util')
    dut.expect_exact('This is a warning message from logging_util')

    dut.expect_exact('Math component is ENABLED')
    dut.expect_exact('add(10, 5) = 15')
    dut.expect_exact('subtract(10, 5) = 5')
    dut.expect_exact('multiply(10, 5) = 50')
