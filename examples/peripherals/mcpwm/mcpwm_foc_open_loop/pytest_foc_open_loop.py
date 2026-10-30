# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_mcpwm_foc_open_loop_example(dut: Dut) -> None:
    dut.expect_exact('mcpwm_foc_open_loop: MCPWM motor FOC example started')
    dut.expect_exact('mcpwm_foc_open_loop: Inverter PWM started')
    dut.expect_exact('mcpwm_foc_open_loop: Gate driver enabled')
