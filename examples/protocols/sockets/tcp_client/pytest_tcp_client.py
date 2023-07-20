# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import socket

import pytest
from common_test_methods import (get_env_config_variable, get_host_ip4_by_dest_ip, get_host_ip6_by_dest_ip,
                                 get_my_interface_by_dest_ip)
from pytest_embedded import Dut

try:
    from run_tcp_server import TcpServer
except ImportError:
    import os
    import sys
    sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'scripts')))
    from run_tcp_server import TcpServer


PORT = 3333


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.esp32c6
@pytest.mark.wifi_router
def test_examples_tcp_client_ipv4(dut: Dut) -> None:
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
    with TcpServer(PORT, socket.AF_INET):
        server_ip = get_host_ip4_by_dest_ip(ipv4)
        print('Connect tcp client to server IP={}'.format(server_ip))
        dut.write(server_ip)
        dut.expect('OK: Message from ESP32')


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.esp32c6
@pytest.mark.wifi_router
def test_examples_tcp_client_ipv6(dut: Dut) -> None:
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
    print('Connected with IPv4={} and IPv6={}'.format(ipv4, ipv6))

    # test IPv6
    my_interface = get_my_interface_by_dest_ip(ipv4)
    with TcpServer(PORT, socket.AF_INET6):
        server_ip = get_host_ip6_by_dest_ip(ipv6, my_interface)
        print('Connect tcp client to server IP={}'.format(server_ip))
        dut.write(server_ip)
        dut.expect('OK: Message from ESP32')
