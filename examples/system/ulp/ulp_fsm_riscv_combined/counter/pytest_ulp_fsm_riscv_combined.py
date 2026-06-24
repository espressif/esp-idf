#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32s2', 'esp32s3'], indirect=['target'])
def test_ulp_fsm_riscv_combined(dut: Dut) -> None:
    dut.expect('ULP FSM and RISC-V Combined Example', timeout=10)

    # Check Step 1: FSM counting
    dut.expect('Step 1: Starting ULP FSM to count from 0 to 100', timeout=5)
    dut.expect('HP core going to sleep, will be woken by ULP FSM when counting completes', timeout=5)
    dut.expect('HP core woken up by: ULP', timeout=5)
    dut.expect('ULP FSM completed. Counter value: 100', timeout=5)

    # Check Step 2: RISC-V counting
    dut.expect('Step 2: Starting ULP RISC-V to count from 100 to 500', timeout=5)
    dut.expect('HP core going to sleep, will be woken by ULP RISC-V when counting completes', timeout=5)
    dut.expect('HP core woken up by: ULP', timeout=5)
    dut.expect('ULP RISC-V completed. Counter value: 500', timeout=5)

    # Check Step 3: Main CPU counting
    dut.expect('Step 3: Main CPU counting from 500 to 1500', timeout=5)
    dut.expect('Main CPU completed. Final counter value: 1500', timeout=5)
    # Verify success message
    dut.expect('All stages completed successfully!', timeout=5)
    dut.expect('FSM: 0 -> 100', timeout=5)
    dut.expect('RISC-V: 100 -> 500', timeout=5)
    dut.expect('Main CPU: 500 -> 1500', timeout=5)
