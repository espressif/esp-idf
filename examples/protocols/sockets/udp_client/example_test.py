# This example code is in the Public Domain (or CC0 licensed, at your option.)

# Unless required by applicable law or agreed to in writing, this
# software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, either express or implied.

# -*- coding: utf-8 -*-
import os
import re
import socket
import sys
from threading import Event, Thread

import netifaces
import ttfw_idf
from common_test_methods import get_env_config_variable, get_host_ip_by_interface, get_my_interface_by_dest_ip
from tiny_test_fw.DUT import ExpectTimeout

# -----------  Config  ----------
PORT = 3333
# -------------------------------


class UdpServer:

    def __init__(self, port, family_addr, persist=False):
        self.port = port
        self.family_addr = family_addr
        self.socket = socket.socket(family_addr, socket.SOCK_DGRAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.socket.settimeout(60.0)
        self.shutdown = Event()
        self.persist = persist

    def __enter__(self):
        try:
            self.socket.bind(('', self.port))
        except socket.error as e:
            print('Bind failed:{}'.format(e))
            raise

        print('Starting server on port={} family_addr={}'.format(self.port, self.family_addr))
        self.server_thread = Thread(target=self.run_server)
        self.server_thread.start()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        if self.persist:
            sock = socket.socket(self.family_addr, socket.SOCK_DGRAM)
            sock.sendto(b'Stop', ('localhost', self.port))
            sock.close()
            self.shutdown.set()
        self.server_thread.join()
        self.socket.close()

    def run_server(self):
        while not self.shutdown.is_set():
            try:
                data, addr = self.socket.recvfrom(1024)
                print(addr)
                if not data:
                    return
                data = data.decode()
                print('Reply[' + addr[0] + ':' + str(addr[1]) + '] - ' + data)
                reply = 'OK: ' + data
                self.socket.sendto(reply.encode(), addr)
            except socket.error as e:
                print('Running server failed:{}'.format(e))
                raise
            if not self.persist:
                break


@ttfw_idf.idf_example_test(env_tag='wifi_router')
def test_examples_protocol_socket_udpclient(env, extra_data):
    """
    steps:
      1. join AP
      2. have the board connect to the server
      3. send and receive data
    """
    dut1 = env.get_dut('udp_client', 'examples/protocols/sockets/udp_client', dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'udp_client.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('udp_client_bin_size', '{}KB'.format(bin_size // 1024))

    # start test
    dut1.start_app()
    if dut1.app.get_sdkconfig_config_value('CONFIG_EXAMPLE_WIFI_SSID_PWD_FROM_STDIN'):
        dut1.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut1.write(f'{ap_ssid} {ap_password}')

    ipv4 = dut1.expect(re.compile(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]'), timeout=30)[0]
    ipv6_r = r':'.join((r'[0-9a-fA-F]{4}',) * 8)    # expect all 8 octets from IPv6 (assumes it's printed in the long form)
    ipv6 = dut1.expect(re.compile(r' IPv6 address: ({})'.format(ipv6_r)), timeout=30)[0]
    print('Connected with IPv4={} and IPv6={}'.format(ipv4, ipv6))

    my_interface = get_my_interface_by_dest_ip(ipv4)
    # test IPv4
    with UdpServer(PORT, socket.AF_INET):
        server_ip = get_host_ip_by_interface(my_interface, netifaces.AF_INET)
        print('Connect udp client to server IP={}'.format(server_ip))
        for _ in range(3):
            try:
                dut1.write(server_ip)
                dut1.expect(re.compile(r'OK: Message from ESP32'))
                break
            except ExpectTimeout:
                pass
        else:
            raise ValueError('Failed to send/recv udp packets.')
    # test IPv6
    with UdpServer(PORT, socket.AF_INET6):
        server_ip = get_host_ip_by_interface(my_interface, netifaces.AF_INET6)
        print('Connect udp client to server IP={}'.format(server_ip))
        for _ in range(3):
            try:
                dut1.write(server_ip)
                dut1.expect(re.compile(r'OK: Message from ESP32'))
                break
            except ExpectTimeout:
                pass
        else:
            raise ValueError('Failed to send/recv udp packets.')


if __name__ == '__main__':
    if sys.argv[1:] and sys.argv[1].startswith('IPv'):     # if additional arguments provided:
        # Usage: example_test.py <IPv4|IPv6>
        family_addr = socket.AF_INET6 if sys.argv[1] == 'IPv6' else socket.AF_INET
        with UdpServer(PORT, family_addr, persist=True) as s:
            print(input('Press Enter stop the server...'))
    else:
        test_examples_protocol_socket_udpclient()
