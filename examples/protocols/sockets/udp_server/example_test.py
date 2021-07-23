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


def udp_client(address, payload):
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
            return
        print('Reply[' + addr[0] + ':' + str(addr[1]) + '] - ' + str(reply))
    except socket.timeout:
        print('Socket operation timeout')
        return str(None)
    except socket.error as msg:
        print('Error while sending or receiving data from the socket')
        print(os.strerror(msg.errno))
        sock.close()
        raise
    return reply.decode()


@ttfw_idf.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_socket_udpserver(env, extra_data):
    MESSAGE = "Data to ESP"
    MAX_RETRIES = 3
    """
    steps:
      1. join AP
      2. have the board connect to the server
      3. send and receive data
    """
    dut1 = env.get_dut("udp_server", "examples/protocols/sockets/udp_server", dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "udp_server.bin")
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance("udp_server_bin_size", "{}KB".format(bin_size // 1024))
    ttfw_idf.check_performance("udp_server_bin_size", bin_size // 1024, dut1.TARGET)

    # start test
    dut1.start_app()

    ipv4 = dut1.expect(re.compile(r" IPv4 address: ([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)"), timeout=30)[0]
    ipv6_r = r':'.join((r'[0-9a-fA-F]{4}',) * 8)    # expect all 8 octets from IPv6 (assumes it's printed in the long form)
    ipv6 = dut1.expect(re.compile(r' IPv6 address: ({})'.format(ipv6_r)), timeout=30)[0]
    print("Connected with IPv4={} and IPv6={}".format(ipv4, ipv6))
    dut1.expect(re.compile(r'Waiting for data'), timeout=10)

    # test IPv4
    for _ in range(MAX_RETRIES):
        print('Testing UDP on IPv4...')
        received = udp_client(ipv4, MESSAGE)
        if received == MESSAGE:
            print('OK')
            break
    else:
        raise ValueError('IPv4: Did not receive UDP message after {} retries'.format(MAX_RETRIES))
    dut1.expect(MESSAGE)

    # test IPv6
    for _ in range(MAX_RETRIES):
        print('Testing UDP on IPv6...')
        received = udp_client('{}%{}'.format(ipv6, INTERFACE), MESSAGE)
        if received == MESSAGE:
            print('OK')
            break
    else:
        raise ValueError('IPv6: Did not receive UDP message after {} retries'.format(MAX_RETRIES))
    dut1.expect(MESSAGE)


if __name__ == '__main__':
    if sys.argv[2:]:    # if two arguments provided:
        # Usage: example_test.py <server_address> <message_to_send_to_server>
        udp_client(sys.argv[1], sys.argv[2])
    else:               # otherwise run standard example test as in the CI
        test_examples_protocol_socket_udpserver()
