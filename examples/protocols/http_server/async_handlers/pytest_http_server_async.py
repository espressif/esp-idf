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
def test_http_server_async_handler_multiple_long_requests(dut: Dut) -> None:
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

    # Create two HTTP connections for long requests
    conn_long1 = http.client.HTTPConnection(got_ip, got_port, timeout=30)
    conn_long2 = http.client.HTTPConnection(got_ip, got_port, timeout=30)

    # Test first long URI with Host header and query param
    long_uri1 = '/long?param=async1'
    headers1 = {'Host': 'testhost1'}
    logging.info('Sending first long request with Host header and query param')
    conn_long1.request('GET', long_uri1, headers=headers1)

    # Test second long URI with Host header and query param
    long_uri2 = '/long?param=async2'
    headers2 = {'Host': 'testhost2'}
    logging.info('Sending second long request with Host header and query param')
    conn_long2.request('GET', long_uri2, headers=headers2)

    # Verify both requests are processed
    dut.expect('Found header => Host: testhost1', timeout=30)
    dut.expect('Found query string => param=async1', timeout=30)
    dut.expect('Found header => Host: testhost2', timeout=30)
    dut.expect('Found query string => param=async2', timeout=30)

    # Get responses
    response_long1 = conn_long1.getresponse()
    response_long2 = conn_long2.getresponse()

    logging.info('Response status for first long URI: {}'.format(response_long1.status))
    logging.info('Response status for second long URI: {}'.format(response_long2.status))

    assert response_long1.status == 200, 'Failed to access first long URI'
    assert response_long2.status == 200, 'Failed to access second long URI'

    conn_long1.close()
    conn_long2.close()


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
