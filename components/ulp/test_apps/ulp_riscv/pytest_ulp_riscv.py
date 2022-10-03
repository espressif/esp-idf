# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_ulp_riscv(dut: Dut) -> None:
    dut.expect('Press ENTER to see the list of tests')
    dut.write('![ulp_deep_sleep_wakeup]')
    dut.expect_unity_test_output()


# Run all deepsleep wakeup tests one after the other instead of running them all with the `ulp_deep_sleep_wakeup` tag.
# This makes sure that all tests are run even after one test causes a system reset.
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_ulp_deep_sleep_wakeup(dut: Dut) -> None:
    dut.expect('Press ENTER to see the list of tests')
    dut.write('"ULP-RISC-V is able to wakeup main CPU from deep sleep"')
    dut.expect('rst:0x5')
