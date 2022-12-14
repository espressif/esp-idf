# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import socket
from threading import Event, Thread

import pytest
from common_test_methods import (get_env_config_variable, get_host_ip4_by_dest_ip, get_host_ip6_by_dest_ip,
                                 get_my_interface_by_dest_ip)
from pytest_embedded import Dut

PORT = 3333


class TcpServer(object):
    def __init__(self, port, family_addr, persist=False):  # type: ignore
        self.port = port
        self.socket = socket.socket(family_addr, socket.SOCK_STREAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.socket.settimeout(60.0)
        self.shutdown = Event()
        self.persist = persist
        self.family_addr = family_addr

    def __enter__(self):  # type: ignore
        try:
            self.socket.bind(('', self.port))
        except socket.error as e:
            print('Bind failed:{}'.format(e))
            raise
        self.socket.listen(1)

        print('Starting server on port={} family_addr={}'.format(self.port, self.family_addr))
        self.server_thread = Thread(target=self.run_server)
        self.server_thread.start()
        return self

    def __exit__(self, exc_type, exc_value, traceback) -> None:  # type: ignore
        if self.persist:
            sock = socket.socket(self.family_addr, socket.SOCK_STREAM)
            sock.connect(('localhost', self.port))
            sock.sendall(b'Stop', )
            sock.close()
            self.shutdown.set()
        self.shutdown.set()
        self.server_thread.join()
        self.socket.close()

    def run_server(self) -> None:
        while not self.shutdown.is_set():
            try:
                conn, address = self.socket.accept()  # accept new connection
                print('Connection from: {}'.format(address))
                conn.setblocking(1)
                data = conn.recv(1024)
                if not data:
                    return
                data = data.decode()
                print('Received data: ' + data)
                reply = 'OK: ' + data
                conn.send(reply.encode())
                conn.close()
            except socket.error as e:
                print('Running server failed:{}'.format(e))
                raise
            if not self.persist:
                break


@pytest.mark.esp32
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
