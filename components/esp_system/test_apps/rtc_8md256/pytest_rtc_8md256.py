# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


def deepsleep_test(dut: Dut, case_name: str) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write(case_name)
    reset_reason = 'DEEPSLEEP_RESET' if dut.target == 'esp32' else 'DSLEEP'
    if dut.target == 'esp32c3':
        # Known issue: IDF-5003
        dut.expect(r'rst:.*\(%s\)' % reset_reason, timeout=40)
    elif dut.target == 'esp32c2':
        # Known issue: IDF-5003
        dut.expect(r'rst:.*\(%s\)' % reset_reason, timeout=60)
    else:
        dut.expect(r'rst:.*\(%s\)' % reset_reason, timeout=10)


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c2
@pytest.mark.generic
def test_rtc_8md256_deepsleep(dut: Dut) -> None:
    deepsleep_test(dut, '"Can use 8MD256 as RTC clock source in deepsleep"')


# Only targets with SOC_PM_SUPPORT_RTC_PERIPH_PD defined
@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_rtc_8md256_deepsleep_force_rtcperiph(dut: Dut) -> None:
    deepsleep_test(dut, '"Can use 8MD256 as RTC clock source in deepsleep (force rtc_periph)"')


def lightsleep_test(dut: Dut, case_name: str) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write(case_name)
    if dut.target == 'esp32c3':
        # Known issue: IDF-5003
        dut.expect(r'Returned from light sleep, reason: timer', timeout=40)
    else:
        dut.expect(r'Returned from light sleep, reason: timer', timeout=10)


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.generic
def test_rtc_8md256_lightsleep(dut: Dut) -> None:
    lightsleep_test(dut, '"Can use 8MD256 as RTC clock source in lightsleep"')


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_rtc_8md256_lightsleep_force_rtcperiph(dut: Dut) -> None:
    lightsleep_test(dut, '"Can use 8MD256 as RTC clock source in lightsleep (force rtc_periph)"')
