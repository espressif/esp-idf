#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import time

import pytest
from common_test_methods import get_env_config_variable
from pytest_embedded import Dut

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
    def __init__(self, ip: str, port: int) -> None:
        self.port = port
        self.ip = ip
        self.ws = websocket.WebSocket()
        # Set timeout to 10 seconds to avoid indefinite blocking
        self.ws.settimeout(10)

    def __enter__(self):  # type: ignore
        url = f'ws://{self.ip}:{self.port}/ws'
        for attempt in range(3):
            try:
                self.ws.connect(url)
                return self
            except (websocket.WebSocketBadStatusException, ConnectionRefusedError, TimeoutError, OSError) as e:
                logging.warning('WS connect attempt %d/3 to %s failed: %s', attempt + 1, url, e)
                if attempt == 2:
                    raise
                time.sleep(2)
        return self

    def __exit__(self, exc_type, exc_value, traceback):     # type: ignore
        self.ws.close()

    def read(self):     # type: ignore
        return self.ws.recv_data(control_frame=True)

    def write(self, data='', opcode=OPCODE_TEXT):   # type: ignore
        if opcode == OPCODE_BIN:
            return self.ws.send_binary(data.encode())
        if opcode == OPCODE_PING:
            return self.ws.ping(data)
        return self.ws.send(data)


@pytest.mark.esp32
@pytest.mark.wifi_router
def test_examples_protocol_http_ws_echo_server(dut: Dut) -> None:
    # Get binary file
    binary_file = os.path.join(dut.app.binary_path, 'ws_echo_server.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info(f'http_ws_server_bin_size : {bin_size // 1024}KB')

    logging.info('Starting ws-echo-server test app based on http_server')

    # Parse IP address of STA
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=60)[1].decode()
    got_port = dut.expect(r"Starting server on port: '(\d+)'", timeout=30)[1].decode()

    # Wait for all URI handlers to be registered before connecting.
    # The /ws URI handler is registered asynchronously after the server
    # starts, taking 40-660ms depending on config and CI load.
    # Connecting before registration completes causes 404 Not Found errors.
    dut.expect('Returned from app_main()', timeout=30)
    time.sleep(1)

    logging.info(f'Got IP   : {got_ip}')
    logging.info(f'Got Port : {got_port}')

    # Start ws server test
    with WsClient(got_ip, int(got_port)) as ws:
        DATA = 'Espressif'
        for expected_opcode in [OPCODE_TEXT, OPCODE_BIN, OPCODE_PING]:
            ws.write(data=DATA, opcode=expected_opcode)
            opcode, data = ws.read()
            logging.info(f'Testing opcode {expected_opcode}: Received opcode:{opcode}, data:{data}')
            data = data.decode()
            if expected_opcode == OPCODE_PING:
                dut.expect('Got a WS PING frame, Replying PONG')
                if opcode != OPCODE_PONG or data != DATA:
                    raise RuntimeError(f'Failed to receive correct opcode:{opcode} or data:{data}')
                continue
            dut_data = dut.expect(r'Got packet with message: ([A-Za-z0-9_]*)')[1]
            dut_opcode = dut.expect(r'Packet type: ([0-9]*)')[1].decode()

            if opcode != expected_opcode or data != DATA or opcode != int(dut_opcode) or (data not in str(dut_data)):
                raise RuntimeError(f'Failed to receive correct opcode:{opcode} or data:{data}')
        ws.write(data='Trigger async', opcode=OPCODE_TEXT)
        opcode, data = ws.read()
        logging.info(f'Testing async send: Received opcode:{opcode}, data:{data}')
        data = data.decode()
        if opcode != OPCODE_TEXT or data != 'Async data':
            raise RuntimeError('Failed to receive correct opcode:{} or data:{}'.format(opcode, data))
