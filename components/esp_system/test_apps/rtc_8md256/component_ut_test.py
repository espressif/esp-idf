# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import re
import typing

import tiny_test_fw
import ttfw_idf
from ttfw_idf import IDFDUT


def deepsleep_test(dut: IDFDUT, case_name: str) -> None:
    dut.expect('Press ENTER to see the list of tests')
    dut.write(case_name)
    reset_reason = 'DEEPSLEEP_RESET' if dut.TARGET == 'esp32' else 'DSLEEP'
    if dut.TARGET == 'esp32c3':
        # Known issue: IDF-5003
        dut.expect(re.compile(r'rst:.*\(%s\)' % reset_reason), timeout=40)
    else:
        dut.expect(re.compile(r'rst:.*\(%s\)' % reset_reason), timeout=10)


@ttfw_idf.idf_component_unit_test(env_tag='COMPONENT_UT_GENERIC', target=['esp32', 'esp32s2', 'esp32s3'])
def test_component_ut_rtc_8md256_deepsleep_force_rtcperiph(env, _):  # type: (tiny_test_fw.Env, typing.Any) -> None
    dut = env.get_dut('rtc_8md256', 'components/esp_system/test_apps/rtc_8md256')
    dut.start_app()

    deepsleep_test(dut, '"Can use 8MD256 as RTC clock source in deepsleep (force rtc_periph)"')


@ttfw_idf.idf_component_unit_test(env_tag='COMPONENT_UT_GENERIC', target=['esp32', 'esp32s2', 'esp32s3', 'esp32c3'])
def test_component_ut_rtc_8md256_deepsleep(env, _):  # type: (tiny_test_fw.Env, typing.Any) -> None
    dut = env.get_dut('rtc_8md256', 'components/esp_system/test_apps/rtc_8md256')
    dut.start_app()

    deepsleep_test(dut, '"Can use 8MD256 as RTC clock source in deepsleep"')


def lightsleep_test(dut: IDFDUT, case_name: str) -> None:
    dut.expect('Press ENTER to see the list of tests')
    dut.write(case_name)
    if dut.TARGET == 'esp32c3':
        # Known issue: IDF-5003
        dut.expect(r'Returned from light sleep, reason: timer', timeout=40)
    else:
        dut.expect(r'Returned from light sleep, reason: timer', timeout=10)


@ttfw_idf.idf_component_unit_test(env_tag='COMPONENT_UT_GENERIC', target=['esp32', 'esp32s2', 'esp32s3', 'esp32c3'])
def test_component_ut_rtc_8md256_lightsleep(env, _):  # type: (tiny_test_fw.Env, typing.Any) -> None
    dut = env.get_dut('rtc_8md256', 'components/esp_system/test_apps/rtc_8md256')
    dut.start_app()

    lightsleep_test(dut, '"Can use 8MD256 as RTC clock source in lightsleep"')


@ttfw_idf.idf_component_unit_test(env_tag='COMPONENT_UT_GENERIC', target=['esp32', 'esp32s2', 'esp32s3'])
def test_component_ut_rtc_8md256_lightsleep_force_rtcperiph(env, _):  # type: (tiny_test_fw.Env, typing.Any) -> None
    dut = env.get_dut('rtc_8md256', 'components/esp_system/test_apps/rtc_8md256')
    dut.start_app()

    lightsleep_test(dut, '"Can use 8MD256 as RTC clock source in lightsleep (force rtc_periph)"')


if __name__ == '__main__':
    test_component_ut_rtc_8md256_deepsleep()
    test_component_ut_rtc_8md256_deepsleep_force_rtcperiph()
    test_component_ut_rtc_8md256_lightsleep()
    test_component_ut_rtc_8md256_lightsleep_force_rtcperiph()
