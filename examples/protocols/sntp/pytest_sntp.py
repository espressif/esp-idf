# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import datetime
import logging
from typing import Any, Tuple

import pytest
from common_test_methods import get_env_config_variable
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.wifi_ap
def test_get_time_from_sntp_server(dut: Dut) -> None:
    dut.expect('Time is not set yet. Connecting to WiFi and getting time over NTP.')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_ap'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    dut.expect('IPv4 address:')

    dut.expect('Initializing and starting SNTP')
    dut.expect('Notification of a time synchronization event')

    TIME_FORMAT = '%a %b %d %H:%M:%S %Y'
    TIME_FORMAT_REGEX = r'\w+\s+\w+\s+\d{1,2}\s+\d{2}:\d{2}:\d{2} \d{4}'
    TIME_DIFF = datetime.timedelta(seconds=10 + 2)  # cpu spends 10 seconds in deep sleep
    NY_time = None
    SH_time = None

    def check_time(prev_NY_time: Any, prev_SH_time: Any) -> Tuple[Any, Any]:
        NY_str = dut.expect(r'The current date/time in New York is: ({})'.format(TIME_FORMAT_REGEX))[1].decode()
        SH_str = dut.expect(r'The current date/time in Shanghai is: ({})'.format(TIME_FORMAT_REGEX))[1].decode()
        logging.info('New York: "{}"'.format(NY_str))
        logging.info('Shanghai: "{}"'.format(SH_str))
        dut.expect('Entering deep sleep for 10 seconds')
        logging.info('Sleeping...')
        new_NY_time = datetime.datetime.strptime(NY_str, TIME_FORMAT)
        new_SH_time = datetime.datetime.strptime(SH_str, TIME_FORMAT)

        # The initial time is not checked because datetime has problems with timezones
        assert not prev_NY_time or new_NY_time - prev_NY_time < TIME_DIFF
        assert not prev_SH_time or new_SH_time - prev_SH_time < TIME_DIFF

        return (new_NY_time, new_SH_time)

    NY_time, SH_time = check_time(NY_time, SH_time)
    for i in range(2, 4):
        dut.expect('example: Boot count: {}'.format(i), timeout=30)
        NY_time, SH_time = check_time(NY_time, SH_time)
