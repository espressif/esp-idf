# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import time

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32s2', 'esp32s3'], indirect=['target'])
def test_ulp_riscv_interrupts(dut: Dut) -> None:
    dut.expect_exact('Not a ULP RISC-V wakeup, initializing it!')
    dut.expect_exact('Entering in deep sleep')

    # Give the chip time to enter deepsleep and trigger a software interrupt
    time.sleep(5)

    # Verify if SW interrupt got triggered
    dut.expect_exact('ULP RISC-V SW Interrupt triggered 5 times.')

    # Set GPIO#0 using DTR
    dut.serial.proc.setDTR(True)
    time.sleep(1)
    dut.serial.proc.setDTR(False)

    # Verify if GPIO interrupt got triggered
    dut.expect_exact('ULP RISC-V GPIO Interrupt triggered.', timeout=5)
