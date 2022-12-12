# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_ulp_fsm(dut: Dut) -> None:
    dut.expect('Press ENTER to see the list of tests')
    dut.write('![ulp_deep_sleep_wakeup]')
    dut.expect_unity_test_output()


# Run all deepsleep wakeup tests one after the other instead of running them all with the `ulp_deep_sleep_wakeup` tag.
# This makes sure that all tests are run even after one test causes a system reset.
@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_ulp_fsm_deep_sleep_wakeup(dut: Dut) -> None:
    dut.expect('Press ENTER to see the list of tests')
    dut.write('"ULP FSM deep-sleep wakeup test"')
    dut.expect('rst:0x5')


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_ulp_fsm_rtc_io(dut: Dut) -> None:
    dut.expect('Press ENTER to see the list of tests')
    dut.write('"ULP FSM controls RTC_IO"')
    dut.expect('rst:0x5')


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_ulp_fsm_deep_sleep_power_consumption(dut: Dut) -> None:
    dut.expect('Press ENTER to see the list of tests')
    dut.write('"ULP FSM power consumption in deep sleep"')
    dut.expect('rst:0x5')


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_ulp_fsm_tsens(dut: Dut) -> None:
    dut.expect('Press ENTER to see the list of tests')
    dut.write('"ULP FSM can use temperature sensor (TSENS) in deep sleep"')
    dut.expect('rst:0x5')


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_ulp_fsm_adc(dut: Dut) -> None:
    dut.expect('Press ENTER to see the list of tests')
    dut.write('"ULP FSM can use ADC in deep sleep"')
    dut.expect('rst:0x5')
