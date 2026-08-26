# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32s2', 'esp32s3'], indirect=['target'])
def test_ulp_combined(dut: Dut) -> None:
    dut.expect_exact('ULP FSM and RISC-V full subproject example')
    dut.expect_exact('Starting ULP FSM counter')
    dut.expect_exact('ULP FSM counter: 5', timeout=5)
    dut.expect_exact('Starting ULP RISC-V counter')
    dut.expect_exact('ULP RISC-V counter: 10', timeout=5)
    dut.expect_exact('Example finished')
