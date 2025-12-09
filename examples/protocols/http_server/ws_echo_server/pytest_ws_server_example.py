#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os

import pytest
from common_test_methods import get_env_config_variable
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

try:
    import websocket
except ImportError:
    print("Please install 'websocket' by running 'python -m pip install websocket-client'")
    raise

OPCODE_TEXT = 0x1
OPCODE_BIN = 0x2
OPCODE_PING = 0x9
OPCODE_PONG = 0xA


class WsClient:
    def __init__(self, ip: str, port: int, uri: str = '') -> None:
        self.port = port
        self.ip = ip
        self.ws = websocket.WebSocket()
        self.uri = uri

    def __enter__(self):  # type: ignore
        self.ws.connect(f'ws://{self.ip}:{self.port}/{self.uri}')
        return self

    def __exit__(self, exc_type, exc_value, traceback):  # type: ignore
        self.ws.close()

    def read(self):  # type: ignore
        return self.ws.recv_data(control_frame=True)

    def write(self, data='', opcode=OPCODE_TEXT):  # type: ignore
        if opcode == OPCODE_BIN:
            return self.ws.send_binary(data.encode())
        if opcode == OPCODE_PING:
            return self.ws.ping(data)
        return self.ws.send(data)


@pytest.mark.wifi_router
@pytest.mark.parametrize(
    'config',
    [
        'default',  # mbedTLS crypto backend
        'psa',  # PSA crypto backend (tests SHA-1 for WebSocket handshake)
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_examples_protocol_http_ws_echo_server(dut: Dut) -> None:
    """
    Test WebSocket echo server with both mbedTLS and PSA crypto backends.
    This specifically tests the SHA-1 computation used in WebSocket handshake (RFC 6455).
    """
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

    logging.info(f'Got IP   : {got_ip}')
    logging.info(f'Got Port : {got_port}')

    # Start ws server test
    with WsClient(got_ip, int(got_port), uri='ws') as ws:
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
            raise RuntimeError(f'Failed to receive correct opcode:{opcode} or data:{data}')


@pytest.mark.wifi_router
@pytest.mark.parametrize(
    'config',
    [
        'default',  # mbedTLS crypto backend
        'psa',  # PSA crypto backend (tests SHA-1 for WebSocket handshake)
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_ws_auth_handshake(dut: Dut) -> None:
    """
    Test that connecting to /ws does NOT print the handshake success log.
    This is used to verify ws_pre_handshake_cb can reject the handshake.
    Tests both mbedTLS and PSA crypto backends.
    """
    # Wait for device to connect and start server
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=60)[1].decode()
    got_port = dut.expect(r"Starting server on port: '(\d+)'", timeout=30)[1].decode()
    # Prepare a minimal WebSocket handshake request
    # Use WSClient to attempt the handshake, expecting it to fail (handshake rejected)

    handshake_success = False
    try:
        # Attempt to use WSClient, expecting it to fail handshake
        with WsClient(got_ip, int(got_port), uri='auth?token=valid') as ws:  # type: ignore  # noqa: F841
            handshake_success = True
    except Exception as e:
        logging.info(f'WebSocket handshake failed: {e}')
        handshake_success = False

    if handshake_success is False:
        raise RuntimeError('WebSocket handshake succeeded, but it should have been rejected by ws_pre_handshake_cb')
