#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import division, print_function, unicode_literals

import os
import re

import ttfw_idf
from tiny_test_fw import Utility

try:
    import websocket
except ImportError:
    print("Please install 'websocket' by running 'python -m pip install websocket-client'")
    raise

OPCODE_TEXT = 0x1
OPCODE_BIN = 0x2
OPCODE_PING = 0x9
OPCODE_PONG = 0xa


class WsClient:
    def __init__(self, ip, port):
        self.port = port
        self.ip = ip
        self.ws = websocket.WebSocket()

    def __enter__(self):
        self.ws.connect('ws://{}:{}/ws'.format(self.ip, self.port))
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.ws.close()

    def read(self):
        return self.ws.recv_data(control_frame=True)

    def write(self, data='', opcode=OPCODE_TEXT):
        if opcode == OPCODE_BIN:
            return self.ws.send_binary(data.encode())
        if opcode == OPCODE_PING:
            return self.ws.ping(data)
        return self.ws.send(data)


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_Protocols')
def test_examples_protocol_http_ws_echo_server(env, extra_data):
    # Acquire DUT
    dut1 = env.get_dut('http_server', 'examples/protocols/http_server/ws_echo_server', dut_class=ttfw_idf.ESP32DUT)

    # Get binary file
    binary_file = os.path.join(dut1.app.binary_path, 'ws_echo_server.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('http_ws_server_bin_size', '{}KB'.format(bin_size // 1024))

    # Upload binary and start testing
    Utility.console_log('Starting ws-echo-server test app based on http_server')
    dut1.start_app()

    # Parse IP address of STA
    Utility.console_log('Waiting to connect with AP')
    got_ip = dut1.expect(re.compile(r'IPv4 address: (\d+.\d+.\d+.\d+)'), timeout=60)[0]
    got_port = dut1.expect(re.compile(r"Starting server on port: '(\d+)'"), timeout=60)[0]

    Utility.console_log('Got IP   : ' + got_ip)
    Utility.console_log('Got Port : ' + got_port)

    # Start ws server test
    with WsClient(got_ip, int(got_port)) as ws:
        DATA = 'Espressif'
        for expected_opcode in [OPCODE_TEXT, OPCODE_BIN, OPCODE_PING]:
            ws.write(data=DATA, opcode=expected_opcode)
            opcode, data = ws.read()
            Utility.console_log('Testing opcode {}: Received opcode:{}, data:{}'.format(expected_opcode, opcode, data))
            data = data.decode()
            if expected_opcode == OPCODE_PING:
                dut1.expect('Got a WS PING frame, Replying PONG')
                if opcode != OPCODE_PONG or data != DATA:
                    raise RuntimeError('Failed to receive correct opcode:{} or data:{}'.format(opcode, data))
                continue
            dut_data = dut1.expect(re.compile(r'Got packet with message: ([A-Za-z0-9_]*)'))[0]
            dut_opcode = int(dut1.expect(re.compile(r'Packet type: ([0-9]*)'))[0])
            if opcode != expected_opcode or data != DATA or opcode != dut_opcode or data != dut_data:
                raise RuntimeError('Failed to receive correct opcode:{} or data:{}'.format(opcode, data))
        ws.write(data='Trigger async', opcode=OPCODE_TEXT)
        opcode, data = ws.read()
        Utility.console_log('Testing async send: Received opcode:{}, data:{}'.format(opcode, data))
        data = data.decode()
        if opcode != OPCODE_TEXT or data != 'Async data':
            raise RuntimeError('Failed to receive correct opcode:{} or data:{}'.format(opcode, data))


if __name__ == '__main__':
    test_examples_protocol_http_ws_echo_server()
