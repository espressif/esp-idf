#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import random
import sys

import pytest

try:
    from idf_http_server_test import adder as client
except ModuleNotFoundError:
    sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', 'tools', 'ci', 'python_packages'))
    from idf_http_server_test import adder as client

from common_test_methods import get_env_config_variable
from pytest_embedded import Dut

# When running on local machine execute the following before running this script
# > make app bootloader
# > make print_flash_cmd | tail -n 1 > build/download.config


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.wifi_router
def test_examples_protocol_http_server_persistence(dut: Dut) -> None:

    # Get binary file
    binary_file = os.path.join(dut.app.binary_path, 'persistent_sockets.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('http_server_bin_size : {}KB'.format(bin_size // 1024))

    # Upload binary and start testing
    logging.info('Starting http_server persistance test app')

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

    # Run test script
    conn = client.start_session(got_ip, got_port)
    visitor = 0
    adder = 0

    # Test login and logout
    if client.getreq(conn, '/login').decode() != str(1):
        raise RuntimeError
    visitor += 1
    dut.expect('/login visitor count = ' + str(visitor), timeout=30)
    dut.expect('/login GET handler send ' + str(1), timeout=30)
    if client.getreq(conn, '/logout').decode() != str(1):
        raise RuntimeError
    dut.expect('Logging out', timeout=30)

    # Test PUT request and initialize session context
    num = random.randint(0,100)
    client.putreq(conn, '/adder', str(num))
    visitor += 1
    dut.expect('/adder visitor count = ' + str(visitor), timeout=30)
    dut.expect('/adder PUT handler read ' + str(num), timeout=30)
    dut.expect('PUT allocating new session', timeout=30)

    # Retest PUT request and change session context value
    num = random.randint(0,100)
    logging.info('Adding: {}'.format(num))
    client.putreq(conn, '/adder', str(num))
    visitor += 1
    adder += num
    dut.expect('/adder visitor count = ' + str(visitor), timeout=30)
    dut.expect('/adder PUT handler read ' + str(num), timeout=30)
    try:
        # Re allocation shouldn't happen
        dut.expect('PUT allocating new session', timeout=30)
        # Not expected
        raise RuntimeError
    except Exception:
        # As expected
        pass

    # Test POST request and session persistence
    random_nums = [random.randint(0,100) for _ in range(100)]
    for num in random_nums:
        logging.info('Adding: {}'.format(num))
        client.postreq(conn, '/adder', str(num))
        visitor += 1
        adder += num
        dut.expect('/adder visitor count = ' + str(visitor), timeout=30)
        dut.expect('/adder handler read ' + str(num), timeout=30)

    # Test GET request and session persistence
    logging.info('Matching final sum: {}'.format(adder))
    if client.getreq(conn, '/adder').decode() != str(adder):
        raise RuntimeError
    visitor += 1
    dut.expect('/adder visitor count = ' + str(visitor), timeout=30)
    dut.expect('/adder GET handler send ' + str(adder), timeout=30)

    logging.info('Ending session')
    # Close connection and check for invocation of context "Free" function
    client.end_session(conn)
    dut.expect('/adder Free Context function called', timeout=30)

    logging.info('Validating user context data')
    # Start another session to check user context data
    client.start_session(got_ip, got_port)
    num = random.randint(0,100)
    client.putreq(conn, '/adder', str(num))
    visitor += 1
    dut.expect('/adder visitor count = ' + str(visitor), timeout=30)
    dut.expect('/adder PUT handler read ' + str(num), timeout=30)
    dut.expect('PUT allocating new session', timeout=30)
    client.end_session(conn)
    dut.expect('/adder Free Context function called', timeout=30)
