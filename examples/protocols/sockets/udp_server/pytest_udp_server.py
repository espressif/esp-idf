# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import os
import socket

import pytest
from common_test_methods import get_env_config_variable, get_my_interface_by_dest_ip
from pytest_embedded import Dut

PORT = 3333
MESSAGE = 'Data to ESP'
MAX_RETRIES = 3


def udp_client(address: str, payload: str) -> str:
    for res in socket.getaddrinfo(address, PORT, socket.AF_UNSPEC,
                                  socket.SOCK_DGRAM, 0, socket.AI_PASSIVE):
        family_addr, socktype, proto, canonname, addr = res
    try:
        sock = socket.socket(family_addr, socket.SOCK_DGRAM)
        sock.settimeout(20.0)
    except socket.error as msg:
        print('Could not create socket')
        print(os.strerror(msg.errno))
        raise
    try:
        sock.sendto(payload.encode(), addr)
        reply, addr = sock.recvfrom(128)
        if not reply:
            return ''
        print('Reply[' + addr[0] + ':' + str(addr[1]) + '] - ' + str(reply))
    except socket.timeout:
        print('Socket operation timeout')
        return ''
    except socket.error as msg:
        print('Error while sending or receiving data from the socket')
        print(os.strerror(msg.errno))
        sock.close()
        raise
    return reply.decode()


@pytest.mark.esp32
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
        received = udp_client(ipv4, MESSAGE)
        if received == MESSAGE:
            print('OK')
            break
    else:
        raise ValueError('IPv4: Did not receive UDP message after {} retries'.format(MAX_RETRIES))
    dut.expect(MESSAGE)


@pytest.mark.esp32
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
        received = udp_client('{}%{}'.format(ipv6, interface), MESSAGE)
        if received == MESSAGE:
            print('OK')
            break
    else:
        raise ValueError('IPv6: Did not receive UDP message after {} retries'.format(MAX_RETRIES))
    dut.expect(MESSAGE)
