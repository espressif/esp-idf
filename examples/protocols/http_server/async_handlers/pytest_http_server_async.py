# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import http.client
import logging
import os
import time

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.ethernet
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_http_server_async_handler(dut: Dut) -> None:
    # Get binary file
    binary_file = os.path.join(dut.app.binary_path, 'simple.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('http_server_bin_size : {}KB'.format(bin_size // 1024))
    logging.info('Waiting to connect with Ethernet')

    # Parse IP address of Ethernet
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
    got_port = 80  # Assuming the server is running on port 80
    logging.info('Got IP   : {}'.format(got_ip))
    logging.info('Connecting to server at {}:{}'.format(got_ip, got_port))

    # Create HTTP connection
    conn_long = http.client.HTTPConnection(got_ip, got_port, timeout=15)

    # Test long URI
    long_uri = '/long'
    logging.info('Sending request to long URI: {}'.format(long_uri))
    conn_long.request('GET', long_uri)
    dut.expect('uri: /long', timeout=30)
    response_long = conn_long.getresponse()
    logging.info('Response status for long URI: {}'.format(response_long.status))
    assert response_long.status == 200, 'Failed to access long URI'

    # Test quick URI
    for i in range(3):
        conn_quick = http.client.HTTPConnection(got_ip, got_port, timeout=15)
        quick_uri = '/quick'
        logging.info('Sending request to quick URI: {}'.format(quick_uri))
        conn_quick.request('GET', quick_uri)
        time.sleep(1)  # Adding a delay of 1 second before getting the response
        response_quick = conn_quick.getresponse()
        dut.expect('uri: /quick', timeout=30)
        logging.info('Response status for quick URI: {}'.format(response_quick.status))
        assert response_quick.status == 200, 'Failed to access quick URI'
        conn_quick.close()

    conn_long.close()
