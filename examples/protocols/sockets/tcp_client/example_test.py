# This example code is in the Public Domain (or CC0 licensed, at your option.)

# Unless required by applicable law or agreed to in writing, this
# software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, either express or implied.

# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals
from builtins import input
import os
import re
import sys
import netifaces
import socket
from threading import Thread, Event
import ttfw_idf


# -----------  Config  ----------
PORT = 3333
INTERFACE = 'eth0'
# -------------------------------


def get_my_ip(type):
    for i in netifaces.ifaddresses(INTERFACE)[type]:
        return i['addr'].replace("%{}".format(INTERFACE), "")


class TcpServer:

    def __init__(self, port, family_addr, persist=False):
        self.port = port
        self.socket = socket.socket(family_addr, socket.SOCK_STREAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.socket.settimeout(10.0)
        self.shutdown = Event()
        self.persist = persist
        self.family_addr = family_addr

    def __enter__(self):
        try:
            self.socket.bind(('', self.port))
        except socket.error as e:
            print("Bind failed:{}".format(e))
            raise
        self.socket.listen(1)

        self.server_thread = Thread(target=self.run_server)
        self.server_thread.start()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        if self.persist:
            sock = socket.socket(self.family_addr, socket.SOCK_STREAM)
            sock.connect(('localhost', self.port))
            sock.sendall(b'Stop', )
            sock.close()
            self.shutdown.set()
        self.shutdown.set()
        self.server_thread.join()
        self.socket.close()

    def run_server(self):
        while not self.shutdown.is_set():
            try:
                conn, address = self.socket.accept()  # accept new connection
                print("Connection from: {}".format(address))
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
                print("Running server failed:{}".format(e))
                raise
            if not self.persist:
                break


@ttfw_idf.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_socket(env, extra_data):
    """
    steps:
      1. join AP
      2. have the board connect to the server
      3. send and receive data
    """
    dut1 = env.get_dut("tcp_client", "examples/protocols/sockets/tcp_client", dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "tcp_client.bin")
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance("tcp_client_bin_size", "{}KB".format(bin_size // 1024))
    ttfw_idf.check_performance("tcp_client_bin_size", bin_size // 1024, dut1.TARGET)

    # start test
    dut1.start_app()

    data = dut1.expect(re.compile(r" IPv4 address: ([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)"), timeout=30)
    print("Connected with IPv4: {}".format(data[0]))

    # test IPv4
    with TcpServer(PORT, socket.AF_INET):
        dut1.write(get_my_ip(netifaces.AF_INET))
        dut1.expect(re.compile(r"OK: Message from ESP32"))
    # test IPv6
    with TcpServer(PORT, socket.AF_INET6):
        dut1.write(get_my_ip(netifaces.AF_INET6))
        dut1.expect(re.compile(r"OK: Message from ESP32"))


if __name__ == '__main__':
    if sys.argv[1:] and sys.argv[1].startswith("IPv"):     # if additional arguments provided:
        # Usage: example_test.py <IPv4|IPv6>
        family_addr = socket.AF_INET6 if sys.argv[1] == "IPv6" else socket.AF_INET
        with TcpServer(PORT, family_addr, persist=True) as s:
            print(input("Press Enter stop the server..."))
    else:
        test_examples_protocol_socket()
