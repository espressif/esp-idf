#!/usr/bin/env python
#
# Copyright 2021 Espressif Systems (Shanghai) CO LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import division, print_function, unicode_literals

import os
import re
import threading
import time
from types import TracebackType
from typing import Any, Optional

import tiny_test_fw
import ttfw_idf
import websocket
from tiny_test_fw import Utility

OPCODE_TEXT = 0x1
OPCODE_BIN = 0x2
OPCODE_PING = 0x9
OPCODE_PONG = 0xa
CORRECT_ASYNC_DATA = 'Hello client'


class WsClient:
    def __init__(self, ip, port, ca_file):  # type: (str, int, str) -> None
        self.port = port
        self.ip = ip
        sslopt = {'ca_certs':ca_file, 'check_hostname': False}
        self.ws = websocket.WebSocket(sslopt=sslopt)
        # Set timeout to 10 seconds to avoid conection failure at the time of handshake
        self.ws.settimeout(10)

    def __enter__(self):  # type: ignore
        self.ws.connect('wss://{}:{}/ws'.format(self.ip, self.port))
        return self

    def __exit__(self, exc_type, exc_value, traceback):  # type: (type, RuntimeError, TracebackType) -> None
        self.ws.close()

    def read(self):  # type: () -> Any
        return self.ws.recv_data(control_frame=True)

    def write(self, data, opcode=OPCODE_TEXT):  # type: (str, int) -> Any
        if opcode == OPCODE_PING:
            return self.ws.ping(data)
        if opcode == OPCODE_PONG:
            return self.ws.pong(data)
        return self.ws.send(data)


class wss_client_thread(threading.Thread):
    def __init__(self, ip, port, ca_file):  # type: (str, int, str) -> None
        threading.Thread.__init__(self)
        self.ip = ip
        self.port = port
        self.ca_file = ca_file
        self.start_time = time.time()
        self.exc = None
        self.data = 'Espressif'
        self.async_response = False

    def run(self):  # type: () -> None
        with WsClient(self.ip, self.port, self.ca_file) as ws:
            while True:
                try:
                    opcode, data = ws.read()
                    data = data.decode('UTF-8')

                    if opcode == OPCODE_PING:
                        ws.write(data=self.data, opcode=OPCODE_PONG)
                    if opcode == OPCODE_TEXT:
                        if data == CORRECT_ASYNC_DATA:
                            self.async_response = True
                            Utility.console_log('Thread {} obtained correct async message'.format(self.name))
                    # Keep sending pong to update the keepalive in the server
                    if (time.time() - self.start_time) > 20:
                        break
                except Exception as e:
                    Utility.console_log('Failed to connect to the client and read async data')
                    self.exc = e  # type: ignore
        if self.async_response is not True:
            self.exc = RuntimeError('Failed to obtain correct async data')  # type: ignore

    def join(self, timeout=0):  # type:(Optional[float]) -> None
        threading.Thread.join(self)
        if self.exc:
            raise self.exc


def test_multiple_client_keep_alive_and_async_response(ip, port, ca_file):  # type: (str, int, str) -> None
    threads = []
    for _ in range(3):
        try:
            thread = wss_client_thread(ip, port, ca_file)
            thread.start()
            threads.append(thread)
        except OSError:
            Utility.console_log('Error: unable to start thread')
        # keep delay of 5 seconds between two connections to avoid handshake timeout
        time.sleep(5)

    for t in threads:
        t.join()


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_Protocols')
def test_examples_protocol_https_wss_server(env, extra_data):  # type: (tiny_test_fw.Env.Env, None) -> None # pylint: disable=unused-argument

    # Acquire DUT
    dut1 = env.get_dut('https_server', 'examples/protocols/https_server/wss_server', dut_class=ttfw_idf.ESP32DUT)

    # Get binary file
    binary_file = os.path.join(dut1.app.binary_path, 'wss_server.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('https_wss_server_bin_size', '{}KB'.format(bin_size // 1024))

    # Upload binary and start testing
    Utility.console_log('Starting wss_server test app')
    dut1.start_app()

    # Parse IP address of STA
    got_port = dut1.expect(re.compile(r'Server listening on port (\d+)'), timeout=60)[0]
    Utility.console_log('Waiting to connect with AP')
    got_ip = dut1.expect(re.compile(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)'), timeout=60)[0]

    Utility.console_log('Got IP   : ' + got_ip)
    Utility.console_log('Got Port : ' + got_port)

    ca_file = os.path.join(os.path.dirname(__file__), 'main', 'certs', 'cacert.pem')
    # Start ws server test
    with WsClient(got_ip, int(got_port), ca_file) as ws:
        # Check for echo
        DATA = 'Espressif'
        dut1.expect('performing session handshake')
        client_fd = dut1.expect(re.compile(r'New client connected (\d+)'), timeout=20)[0]
        ws.write(data=DATA, opcode=OPCODE_TEXT)
        dut1.expect(re.compile(r'Received packet with message: {}'.format(DATA)))
        opcode, data = ws.read()
        data = data.decode('UTF-8')
        if data != DATA:
            raise RuntimeError('Failed to receive the correct echo response')
        Utility.console_log('Correct echo response obtained from the wss server')

        # Test for keepalive
        Utility.console_log('Testing for keep alive (approx time = 20s)')
        start_time = time.time()
        while True:
            try:
                opcode, data = ws.read()
                if opcode == OPCODE_PING:
                    ws.write(data='Espressif', opcode=OPCODE_PONG)
                    Utility.console_log('Received PING, replying PONG (to update the keepalive)')
                # Keep sending pong to update the keepalive in the server
                if (time.time() - start_time) > 20:
                    break
            except Exception:
                Utility.console_log('Failed the test for keep alive,\nthe client got abruptly disconnected')
                raise

        # keepalive timeout is 10 seconds so do not respond for (10 + 1) senconds
        Utility.console_log('Testing if client is disconnected if it does not respond for 10s i.e. keep_alive timeout (approx time = 11s)')
        try:
            dut1.expect('Client not alive, closing fd {}'.format(client_fd), timeout=20)
            dut1.expect('Client disconnected {}'.format(client_fd))
        except Exception:
            Utility.console_log('ENV_ERROR:Failed the test for keep alive,\nthe connection was not closed after timeout')

        time.sleep(11)
        Utility.console_log('Passed the test for keep alive')

    # Test keep alive and async response for multiple simultaneous client connections
    Utility.console_log('Testing for multiple simultaneous client connections (approx time = 30s)')
    test_multiple_client_keep_alive_and_async_response(got_ip, int(got_port), ca_file)
    Utility.console_log('Passed the test for multiple simultaneous client connections')


if __name__ == '__main__':
    test_examples_protocol_https_wss_server()  # pylint: disable=no-value-for-parameter
