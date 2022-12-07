#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import division, print_function, unicode_literals

import logging
import os
import threading
import time
from types import TracebackType
from typing import Any, Optional

import pytest
import websocket
from common_test_methods import get_env_config_variable
from pytest_embedded import Dut

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
                            logging.info('Thread {} obtained correct async message'.format(self.name))
                    # Keep sending pong to update the keepalive in the server
                    if (time.time() - self.start_time) > 20:
                        break
                except Exception as e:
                    logging.info('Failed to connect to the client and read async data')
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
            logging.info('Error: unable to start thread')
        # keep delay of 5 seconds between two connections to avoid handshake timeout
        time.sleep(5)

    for t in threads:
        t.join()


@pytest.mark.esp32
@pytest.mark.wifi_router
def test_examples_protocol_https_wss_server(dut: Dut) -> None:

    # Get binary file
    binary_file = os.path.join(dut.app.binary_path, 'wss_server.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('https_wss_server_bin_size : {}KB'.format(bin_size // 1024))

    logging.info('Starting wss_server test app')

    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    # Parse IP address of STA
    got_port = int(dut.expect(r'Server listening on port (\d+)', timeout=30)[1].decode())
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()

    logging.info('Got IP   : {}'.format(got_ip))
    logging.info('Got Port : {}'.format(got_port))

    ca_file = os.path.join(os.path.dirname(__file__), 'main', 'certs', 'servercert.pem')
    # Start ws server test
    with WsClient(got_ip, int(got_port), ca_file) as ws:
        # Check for echo
        DATA = 'Espressif'
        dut.expect('performing session handshake')
        client_fd = int(dut.expect(r'New client connected (\d+)', timeout=30)[1].decode())
        ws.write(data=DATA, opcode=OPCODE_TEXT)
        dut.expect(r'Received packet with message: {}'.format(DATA))
        opcode, data = ws.read()
        data = data.decode('UTF-8')
        if data != DATA:
            raise RuntimeError(f'Failed to receive the correct echo response.')
        logging.info('Correct echo response obtained from the wss server')

        # Test for PING
        logging.info('Testing for send PING')
        ws.write(data=DATA, opcode=OPCODE_PING)
        dut.expect('Got a WS PING frame, Replying PONG')
        opcode, data = ws.read()
        data = data.decode('UTF-8')
        if data != DATA or opcode != OPCODE_PONG:
            raise RuntimeError('Failed to receive the PONG response')
        logging.info('Passed the test for PING')

        # Test for keepalive
        logging.info('Testing for keep alive (approx time = 20s)')
        start_time = time.time()
        while True:
            try:
                opcode, data = ws.read()
                if opcode == OPCODE_PING:
                    ws.write(data='Espressif', opcode=OPCODE_PONG)
                    logging.info('Received PING, replying PONG (to update the keepalive)')
                # Keep sending pong to update the keepalive in the server
                if (time.time() - start_time) > 20:
                    break
            except Exception:
                logging.info('Failed the test for keep alive,\nthe client got abruptly disconnected')
                raise

        # keepalive timeout is 10 seconds so do not respond for (10 + 1) senconds
        logging.info('Testing if client is disconnected if it does not respond for 10s i.e. keep_alive timeout (approx time = 11s)')
        try:
            dut.expect('Client not alive, closing fd {}'.format(client_fd), timeout=20)
            dut.expect('Client disconnected {}'.format(client_fd))
        except Exception:
            logging.info('ENV_ERROR:Failed the test for keep alive,\nthe connection was not closed after timeout')

        time.sleep(11)
        logging.info('Passed the test for keep alive')

    # Test keep alive and async response for multiple simultaneous client connections
    logging.info('Testing for multiple simultaneous client connections (approx time = 30s)')
    test_multiple_client_keep_alive_and_async_response(got_ip, int(got_port), ca_file)
    logging.info('Passed the test for multiple simultaneous client connections')
