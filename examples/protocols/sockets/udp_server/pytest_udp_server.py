# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging

import pytest
from common_test_methods import get_env_config_variable, get_my_interface_by_dest_ip
from pytest_embedded import Dut

try:
    from run_udp_client import udp_client
except ImportError:
    import os
    import sys
    sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'scripts')))
    from run_udp_client import udp_client


PORT = 3333
MESSAGE = 'Data to ESP'
MAX_RETRIES = 3


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.esp32c6
@pytest.mark.wifi_router
def test_examples_udp_server_ipv4(dut: Dut) -> None:
    # Parse IP address of STA
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    ipv4 = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
    print(f'Connected with IPv4={ipv4}')

    # test IPv4
    for _ in range(MAX_RETRIES):
        print('Testing UDP on IPv4...')
        received = udp_client(ipv4, PORT, MESSAGE)
        if received == MESSAGE:
            print('OK')
            break
    else:
        raise ValueError('IPv4: Did not receive UDP message after {} retries'.format(MAX_RETRIES))
    dut.expect(MESSAGE)


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.esp32c6
@pytest.mark.wifi_router
def test_examples_udp_server_ipv6(dut: Dut) -> None:
    # Parse IP address of STA
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    ipv4 = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
    # expect all 8 octets from IPv6 (assumes it's printed in the long form)
    ipv6_r = r':'.join((r'[0-9a-fA-F]{4}',) * 8)
    ipv6 = dut.expect(ipv6_r, timeout=30)[0].decode()
    print(f'Connected with IPv4={ipv4} and IPv6={ipv6}')

    interface = get_my_interface_by_dest_ip(ipv4)
    # test IPv6
    for _ in range(MAX_RETRIES):
        print('Testing UDP on IPv6...')
        received = udp_client('{}%{}'.format(ipv6, interface), PORT, MESSAGE)
        if received == MESSAGE:
            print('OK')
            break
    else:
        raise ValueError('IPv6: Did not receive UDP message after {} retries'.format(MAX_RETRIES))
    dut.expect(MESSAGE)
