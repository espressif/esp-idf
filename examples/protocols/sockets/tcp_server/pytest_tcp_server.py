# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import time

import netifaces
import pytest
from common_test_methods import get_env_config_variable
from common_test_methods import get_my_interface_by_dest_ip
from pytest_embedded import Dut

try:
    from run_tcp_client import tcp_client
except ImportError:
    import os
    import sys
    sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'scripts')))
    from run_tcp_client import tcp_client


PORT = 3333
MESSAGE = 'Data to ESP'


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.esp32c5
@pytest.mark.esp32c6
@pytest.mark.wifi_router
def test_examples_tcp_server_ipv4(dut: Dut) -> None:
    # Parse IP address of STA
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    ipv4 = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
    logging.info(f'Connected with IPv4={ipv4}')
    time.sleep(1)

    # test IPv4
    received = tcp_client(ipv4, PORT, MESSAGE)
    if not received == MESSAGE:
        raise
    dut.expect(MESSAGE)


@pytest.mark.esp32c2
@pytest.mark.wifi_router
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'config, baud', [
        ('c2_xtal26m', '74880'),
    ], indirect=True
)
def test_examples_tcp_server_ipv4_esp32c2_26mhz(dut: Dut) -> None:
    # Parse IP address of STA
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    ipv4 = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
    logging.info(f'Connected with IPv4={ipv4}')
    time.sleep(1)

    # test IPv4
    received = tcp_client(ipv4, PORT, MESSAGE)
    if not received == MESSAGE:
        raise
    dut.expect(MESSAGE)


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.esp32c6
@pytest.mark.wifi_router
def test_examples_tcp_server_ipv6(dut: Dut) -> None:
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
    logging.info(f'Connected with IPv4={ipv4} and IPv6={ipv6}')
    time.sleep(1)

    interface = get_my_interface_by_dest_ip(ipv4)
    # test IPv6
    received = tcp_client('{}%{}'.format(ipv6, interface), PORT, MESSAGE)
    if not received == MESSAGE:
        raise
    dut.expect(MESSAGE)


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.esp32c6
@pytest.mark.wifi_router
def test_examples_tcp_server_ipv6_only(dut: Dut) -> None:
    # Parse IP address of STA
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    # expect all 8 octets from IPv6 (assumes it's printed in the long form)
    ipv6_r = r':'.join((r'[0-9a-fA-F]{4}',) * 8)
    ipv6 = dut.expect(ipv6_r, timeout=30)[0].decode()
    logging.info(f'Connected AP with IPv6={ipv6}')
    time.sleep(5)

    # test IPv6, try all interfaces
    for interface in netifaces.interfaces():
        try:
            logging.info(f'[{interface}] Connect to tcp_server {ipv6}%{interface}')
            received = tcp_client(f'{ipv6}%{interface}', PORT, MESSAGE)
        except Exception:
            # wrong interface
            continue
        if not received == MESSAGE:
            raise AssertionError(f'Wrong data received ({received}/{MESSAGE})')
        dut.expect(MESSAGE)
        break
    else:
        raise AssertionError(f'Can not connect to tcp_server: {ipv6}')
