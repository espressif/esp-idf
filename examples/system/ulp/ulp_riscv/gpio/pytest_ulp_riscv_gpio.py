# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import time

import pexpect
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_ulp_riscv_gpio(dut: Dut) -> None:

    dut.expect_exact('Not a ULP-RISC-V wakeup, initializing it!')
    dut.expect_exact('Entering in deep sleep')

    # Give the chip time to enter deepsleep
    time.sleep(1)

    # Run two times to make sure device sleep
    # and wake up properly
    for i in range(0, 2):
        # Set GPIO0 using DTR
        dut.serial.proc.setDTR(i % 2 == 0)

        dut.expect_exact('ULP-RISC-V woke up the main CPU!', timeout=5)

        # Check GPIO state
        state = 'Low' if i % 2 == 0 else 'High'
        dut.expect(r'ULP-RISC-V read changes in GPIO_0 current is: %s' % state, timeout=5)

        # Go back to sleep
        dut.expect_exact('Entering in deep sleep', timeout=5)

        try:
            # We expect a timeout here, otherwise it means that
            # the main CPU woke up unexpectedly!
            dut.expect('ULP-RISC-V woke up the main CPU!', timeout=20)
            raise Exception('Main CPU woke up unexpectedly!')
        except pexpect.exceptions.TIMEOUT:
            pass
