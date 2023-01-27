#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import random
import socket
import string
import sys
import threading
import time

import pytest

try:
    from idf_http_server_test import client
except ModuleNotFoundError:
    sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', 'tools', 'ci', 'python_packages'))
    from idf_http_server_test import client

from common_test_methods import get_env_config_variable
from pytest_embedded import Dut


class http_client_thread(threading.Thread):
    def __init__(self, ip: str, port: int, delay: int) -> None:
        threading.Thread.__init__(self)
        self.ip = ip
        self.port = port
        self.delay = delay
        self.exc = 0

    # Thread function used to open a socket and wait for specific amount of time before returning
    def open_connection(self, ip: str, port: int, delay: int) -> None:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(delay)
        s.connect((ip, port))
        time.sleep(delay)

    def run(self) -> None:
        try:
            self.open_connection(self.ip, self.port, self.delay)
        except socket.timeout:
            self.exc = 1

    def join(self, timeout=None):   # type: ignore
        threading.Thread.join(self)
        if self.exc:
            raise socket.timeout


# When running on local machine execute the following before running this script
# > make app bootloader
# > make print_flash_cmd | tail -n 1 > build/download.config


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.wifi_router
def test_examples_protocol_http_server_simple(dut: Dut) -> None:

    # Get binary file
    binary_file = os.path.join(dut.app.binary_path, 'simple.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('http_server_bin_size : {}KB'.format(bin_size // 1024))

    # Upload binary and start testing
    logging.info('Starting http_server simple test app')

    # Parse IP address of STA
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(' '.join([ap_ssid, ap_password]))
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
    got_port = dut.expect(r"(?:[\s\S]*)Starting server on port: '(\d+)'", timeout=30)[1].decode()

    logging.info('Got IP   : {}'.format(got_ip))
    logging.info('Got Port : {}'.format(got_port))

    # Expected Logs
    dut.expect('Registering URI handlers', timeout=30)

    # Run test script
    # If failed raise appropriate exception
    logging.info('Test /hello GET handler')
    if not client.test_get_handler(got_ip, str(got_port)):
        raise RuntimeError

    # Acquire host IP. Need a way to check it
    dut.expect(r'(?:[\s\S]*)Found header => Host: (\d+.\d+.\d+.\d+)', timeout=30)

    # Match additional headers sent in the request
    dut.expect('Found header => Test-Header-2: Test-Value-2', timeout=30)
    dut.expect('Found header => Test-Header-1: Test-Value-1', timeout=30)
    dut.expect('Found URL query parameter => query1=value1', timeout=30)
    dut.expect('Found URL query parameter => query3=value3', timeout=30)
    dut.expect('Found URL query parameter => query2=value2', timeout=30)
    dut.expect('Request headers lost', timeout=30)

    logging.info('Test /ctrl PUT handler and realtime handler de/registration')
    if not client.test_put_handler(got_ip, got_port):
        raise RuntimeError
    dut.expect('Unregistering /hello and /echo URIs', timeout=30)
    dut.expect('Registering /hello and /echo URIs', timeout=30)

    # Generate random data of 10KB
    random_data = ''.join(string.printable[random.randint(0,len(string.printable)) - 1] for _ in range(10 * 1024))
    logging.info('Test /echo POST handler with random data')
    if not client.test_post_handler(got_ip, got_port, random_data):
        raise RuntimeError

    queries = 'query1=http%3A%2F%2Ffoobar&query3=abcd%2B1234%20xyz&query2=Esp%21%40%20%23%2471'
    logging.info('Test /hello with custom query')
    if not client.test_custom_uri_query(got_ip, got_port, queries):
        raise RuntimeError

    dut.expect_exact('Found URL query => query1=http%3A%2F%2Ffoobar&query3=abcd%2B1234%20xyz&query2=Esp%21%40%20%23%2471', timeout=30)
    dut.expect_exact('Found URL query parameter => query1=http%3A%2F%2Ffoobar', timeout=30)
    dut.expect_exact('Decoded query parameter => http://foobar', timeout=30)
    dut.expect_exact('Found URL query parameter => query3=abcd%2B1234%20xyz', timeout=30)
    dut.expect_exact('Decoded query parameter => abcd+1234 xyz', timeout=30)
    dut.expect_exact('Found URL query parameter => query2=Esp%21%40%20%23%2471', timeout=30)
    dut.expect_exact('Decoded query parameter => Esp!@ #$71', timeout=30)


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.wifi_router
def test_examples_protocol_http_server_lru_purge_enable(dut: Dut) -> None:

    # Get binary file
    binary_file = os.path.join(dut.app.binary_path, 'simple.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('http_server_bin_size : {}KB'.format(bin_size // 1024))

    # Upload binary and start testing
    logging.info('Starting http_server simple test app')

    # Parse IP address of STA
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
    got_port = dut.expect(r"(?:[\s\S]*)Starting server on port: '(\d+)'", timeout=30)[1].decode()

    logging.info('Got IP   : {}'.format(got_ip))
    logging.info('Got Port : {}'.format(got_port))

    # Expected Logs
    dut.expect('Registering URI handlers', timeout=30)
    threads = []
    # Open 20 sockets, one from each thread
    for _ in range(20):
        try:
            thread = http_client_thread(got_ip, (int(got_port)), 20)
            thread.start()
            threads.append(thread)
        except OSError as err:
            logging.info('Error: unable to start thread, {}'.format(err))

    for t in threads:
        t.join()
