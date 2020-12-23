# This example code is in the Public Domain (or CC0 licensed, at your option.)

# Unless required by applicable law or agreed to in writing, this
# software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, either express or implied.

# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals
import os
import sys
import re
import socket
import ttfw_idf


# -----------  Config  ----------
PORT = 3333
INTERFACE = 'eth0'
# -------------------------------


def tcp_client(address, payload):
    for res in socket.getaddrinfo(address, PORT, socket.AF_UNSPEC,
                                  socket.SOCK_STREAM, 0, socket.AI_PASSIVE):
        family_addr, socktype, proto, canonname, addr = res
    try:
        sock = socket.socket(family_addr, socket.SOCK_STREAM)
        sock.settimeout(60.0)
    except socket.error as msg:
        print('Could not create socket: ' + str(msg[0]) + ': ' + msg[1])
        raise
    try:
        sock.connect(addr)
    except socket.error as msg:
        print('Could not open socket: ', msg)
        sock.close()
        raise
    sock.sendall(payload.encode())
    data = sock.recv(1024)
    if not data:
        return
    print('Reply : ' + data.decode())
    sock.close()
    return data.decode()


@ttfw_idf.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_socket_tcpserver(env, extra_data):
    MESSAGE = "Data to ESP"
    """
    steps:
      1. join AP
      2. have the board connect to the server
      3. send and receive data
    """
    dut1 = env.get_dut("tcp_client", "examples/protocols/sockets/tcp_server", dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "tcp_server.bin")
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance("tcp_server_bin_size", "{}KB".format(bin_size // 1024))
    ttfw_idf.check_performance("tcp_server_bin_size", bin_size // 1024, dut1.TARGET)

    # start test
    dut1.start_app()

    ipv4 = dut1.expect(re.compile(r" IPv4 address: ([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)"), timeout=30)[0]
    ipv6_r = r':'.join((r'[0-9a-fA-F]{4}',) * 8)    # expect all 8 octets from IPv6 (assumes it's printed in the long form)
    ipv6 = dut1.expect(re.compile(r' IPv6 address: ({})'.format(ipv6_r)), timeout=30)[0]
    print("Connected with IPv4={} and IPv6={}".format(ipv4, ipv6))

    # test IPv4
    received = tcp_client(ipv4, MESSAGE)
    if not received == MESSAGE:
        raise
    dut1.expect(MESSAGE)
    # test IPv6
    received = tcp_client("{}%{}".format(ipv6, INTERFACE), MESSAGE)
    if not received == MESSAGE:
        raise
    dut1.expect(MESSAGE)


if __name__ == '__main__':
    if sys.argv[2:]:    # if two arguments provided:
        # Usage: example_test.py <server_address> <message_to_send_to_server>
        tcp_client(sys.argv[1], sys.argv[2])
    else:               # otherwise run standard example test as in the CI
        test_examples_protocol_socket_tcpserver()
