#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
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
        # Set timeout to 10 seconds to avoid indefinite blocking
        self.ws.settimeout(10)
        self.uri = uri

    def __enter__(self):  # type: ignore
        self.ws.connect(f'ws://{self.ip}:{self.port}/{self.uri}')
        return self

    def __exit__(self, exc_type, exc_value, traceback):  # type: ignore
        self.ws.close()

    def read(self):  # type: ignore
        result = self.ws.recv_data(control_frame=True)
        if result is None:
            raise RuntimeError('WebSocket recv_data returned None (timeout or connection closed)')
        if not isinstance(result, tuple) or len(result) != 2:
            raise RuntimeError(f'WebSocket recv_data returned unexpected format: {result}')
        return result

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
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_examples_protocol_http_ws_echo_server(dut: Dut) -> None:
    """
    Test WebSocket echo server with mbedTLS crypto backends.
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
                if opcode != OPCODE_PONG or data != DATA:
                    raise RuntimeError(f'Failed to receive correct opcode:{opcode} or data:{data}')
                continue
            dut_data = dut.expect(r'Got packet with message: ([A-Za-z0-9_]*)')[1]
            dut_opcode = dut.expect(r'Packet type: ([0-9]*)')[1].decode()

            if opcode != expected_opcode or data != DATA or opcode != int(dut_opcode) or (data not in str(dut_data)):
                raise RuntimeError(f'Failed to receive correct opcode:{opcode} or data:{data}')
        # Test async send - server queues the work, so we need to wait for it to be processed
        logging.info('Testing async send')
        ws.write(data='Trigger async', opcode=OPCODE_TEXT)
        # Wait for server to receive and queue the async send
        dut.expect(r'Got packet with message: Trigger async', timeout=10)
        # Now read the async response (server processes work queue asynchronously)
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
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_examples_protocol_http_ws_echo_server_partial(dut: Dut) -> None:
    # Get binary file
    binary_file = os.path.join(dut.app.binary_path, 'ws_echo_server.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info(f'http_ws_server_bin_size : {bin_size // 1024}KB')

    logging.info('Starting ws-echo-server partial read test')

    # Parse IP address of STA
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
    got_port = dut.expect(r"Starting server on port: '(\d+)'", timeout=30)[1].decode()

    logging.info(f'Got IP   : {got_ip}')
    logging.info(f'Got Port : {got_port}')

    # Start ws server test with partial read endpoint
    # Create a new websocket connection to the partial endpoint
    ws_partial = websocket.WebSocket()
    ws_partial.settimeout(10)
    try:
        ws_partial.connect(f'ws://{got_ip}:{int(got_port)}/ws_partial')

        # Create a large message (200 bytes) to force multiple partial reads
        # The server uses 64-byte chunks, so this will require at least 4 reads
        large_data = 'A' * 200
        logging.info(f'Sending large message of {len(large_data)} bytes to test partial reads')
        ws_partial.send(large_data)

        # Verify partial reads happened in server logs
        # Add timeouts to prevent indefinite waiting in CI environments
        dut.expect('Frame len is 200', timeout=10)
        dut.expect('Reading chunk: left_len=200', timeout=10)
        dut.expect('Sent fragmented chunk: len=64, final=0', timeout=10)  # First chunk (not final)
        dut.expect('Reading chunk: left_len=136', timeout=10)  # After first 64-byte read
        dut.expect('Sent fragmented chunk: len=64, final=0', timeout=10)  # Second chunk (not final)
        dut.expect('Reading chunk: left_len=72', timeout=10)  # After second 64-byte read
        dut.expect('Sent fragmented chunk: len=64, final=0', timeout=10)  # Third chunk (not final)
        dut.expect('Reading chunk: left_len=8', timeout=10)  # After third 64-byte read
        dut.expect(
            'Sent fragmented chunk: len=8, final=1', timeout=10
        )  # Last chunk (final - required by WebSocket protocol)
        dut.expect('Packet type: 1', timeout=10)  # HTTPD_WS_TYPE_TEXT = 1

        # Read the echo response (websocket client will reassemble fragmented frames)
        result = ws_partial.recv_data(control_frame=True)
        if result is None:
            raise RuntimeError('WebSocket recv_data returned None (timeout or connection closed)')
        if not isinstance(result, tuple) or len(result) != 2:
            raise RuntimeError(f'WebSocket recv_data returned unexpected format: {result}')
        opcode, data = result
        logging.info(f'Testing partial read: Received opcode:{opcode}, data length:{len(data)}')
        data = data.decode()

        if opcode != OPCODE_TEXT or data != large_data:
            raise RuntimeError(
                f'Failed to receive correct echo for partial read. Expected length:{len(large_data)}, got:{len(data)}'
            )

        logging.info(f'Partial read test passed: received {len(data)} bytes correctly')
    finally:
        ws_partial.close()


@pytest.mark.wifi_router
@pytest.mark.parametrize(
    'config',
    [
        'default',  # mbedTLS crypto backend
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_ws_auth_handshake(dut: Dut) -> None:
    """
    Test that connecting to /ws does NOT print the handshake success log.
    This is used to verify ws_pre_handshake_cb can reject the handshake.
    Tests mbedTLS crypto backend.
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
