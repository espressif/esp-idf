# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import os
import socket
import time

import pytest
from common_test_methods import get_env_config_variable, get_my_interface_by_dest_ip
from pytest_embedded import Dut

PORT = 3333
MESSAGE = 'Data to ESP'


def tcp_client(address: str, payload: str) -> str:
    for res in socket.getaddrinfo(address, PORT, socket.AF_UNSPEC,
                                  socket.SOCK_STREAM, 0, socket.AI_PASSIVE):
        family_addr, socktype, proto, canonname, addr = res
    try:
        sock = socket.socket(family_addr, socket.SOCK_STREAM)
        sock.settimeout(60.0)
    except socket.error as msg:
        print('Could not create socket')
        print(os.strerror(msg.errno))
        raise
    try:
        sock.connect(addr)
    except socket.error as e:
        print('Could not open socket: ' + str(e))
        sock.close()
        raise
    sock.sendall(payload.encode())
    data = sock.recv(1024)
    if not data:
        return ''
    print('Reply : ' + data.decode())
    sock.close()
    return data.decode()


@pytest.mark.esp32
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
    print(f'Connected with IPv4={ipv4}')
    time.sleep(1)

    # test IPv4
    received = tcp_client(ipv4, MESSAGE)
    if not received == MESSAGE:
        raise
    dut.expect(MESSAGE)


@pytest.mark.esp32
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
    print(f'Connected with IPv4={ipv4} and IPv6={ipv6}')
    time.sleep(1)

    interface = get_my_interface_by_dest_ip(ipv4)
    # test IPv6
    received = tcp_client('{}%{}'.format(ipv6, interface), MESSAGE)
    if not received == MESSAGE:
        raise
    dut.expect(MESSAGE)
