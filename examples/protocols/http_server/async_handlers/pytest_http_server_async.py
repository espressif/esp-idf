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
    logging.info(f'http_server_bin_size : {bin_size // 1024}KB')
    logging.info('Waiting to connect with Ethernet')

    # Parse IP address of Ethernet
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
    got_port = 80  # Assuming the server is running on port 80
    logging.info(f'Got IP   : {got_ip}')
    dut.expect('starting async req task worker', timeout=30)
    dut.expect('starting async req task worker', timeout=30)
    dut.expect(f"Starting server on port: '{got_port}'", timeout=30)
    dut.expect('Registering URI handlers', timeout=30)
    logging.info(f'Connecting to server at {got_ip}:{got_port}')

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

    logging.info(f'Response status for first long URI: {response_long1.status}')
    logging.info(f'Response status for second long URI: {response_long2.status}')

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
    logging.info(f'http_server_bin_size : {bin_size // 1024}KB')
    logging.info('Waiting to connect with Ethernet')

    # Parse IP address of Ethernet
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
    got_port = 80  # Assuming the server is running on port 80
    logging.info(f'Got IP   : {got_ip}')
    dut.expect('starting async req task worker', timeout=30)
    dut.expect('starting async req task worker', timeout=30)
    dut.expect(f"Starting server on port: '{got_port}'", timeout=30)
    dut.expect('Registering URI handlers', timeout=30)
    logging.info(f'Connecting to server at {got_ip}:{got_port}')

    # Create HTTP connection
    conn_long = http.client.HTTPConnection(got_ip, got_port, timeout=15)

    # Test long URI
    long_uri = '/long'
    logging.info(f'Sending request to long URI: {long_uri}')
    conn_long.request('GET', long_uri)
    dut.expect('uri: /long', timeout=30)
    response_long = conn_long.getresponse()
    logging.info(f'Response status for long URI: {response_long.status}')
    assert response_long.status == 200, 'Failed to access long URI'

    # Test quick URI
    for i in range(3):
        conn_quick = http.client.HTTPConnection(got_ip, got_port, timeout=15)
        quick_uri = '/quick'
        logging.info(f'Sending request to quick URI: {quick_uri}')
        conn_quick.request('GET', quick_uri)
        time.sleep(1)  # Adding a delay of 1 second before getting the response
        response_quick = conn_quick.getresponse()
        dut.expect('uri: /quick', timeout=30)
        logging.info(f'Response status for quick URI: {response_quick.status}')
        assert response_quick.status == 200, 'Failed to access quick URI'
        conn_quick.close()

    conn_long.close()


@pytest.mark.ethernet
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_http_server_async_handler_same_session_sequential(dut: Dut) -> None:
    """
    Test that verifies async completion fix:
    1. Send /long request (async, 60 seconds)
    2. Wait for completion
    3. Send another request on same session
    4. Verify second request works (doesn't get stuck)
    """
    # Get binary file
    binary_file = os.path.join(dut.app.binary_path, 'simple.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info(f'http_server_bin_size : {bin_size // 1024}KB')
    logging.info('Waiting to connect with Ethernet')

    # Parse IP address of Ethernet
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
    got_port = 80  # Assuming the server is running on port 80
    logging.info(f'Got IP   : {got_ip}')
    dut.expect('starting async req task worker', timeout=30)
    dut.expect('starting async req task worker', timeout=30)
    dut.expect(f"Starting server on port: '{got_port}'", timeout=30)
    dut.expect('Registering URI handlers', timeout=30)
    logging.info(f'Connecting to server at {got_ip}:{got_port}')

    # Create HTTP connection for same session testing
    conn = http.client.HTTPConnection(got_ip, got_port, timeout=70)  # Longer timeout for async

    # Test 1: Send /long request (async, 60 seconds)
    logging.info('=== Test 1: Sending /long request (async) ===')
    conn.request('GET', '/long?test=sequential1')

    # Verify request is received and processed
    dut.expect('uri: /long', timeout=30)
    dut.expect('Found query string => test=sequential1', timeout=30)

    # Wait for async completion (60 seconds + buffer)
    logging.info('Waiting for async /long request to complete (60 seconds)...')
    start_time = time.time()

    # Get response (this will block until async handler completes)
    response_long = conn.getresponse()
    completion_time = time.time() - start_time

    logging.info(f'Response status for /long: {response_long.status}')
    logging.info(f'Async request completed in {completion_time:.2f} seconds')
    assert response_long.status == 200, 'Failed to access /long URI'

    # Verify we got the full response (should contain 60 ticks)
    response_data = response_long.read().decode()
    assert 'req: 1' in response_data, 'Expected request count in response'
    assert '59' in response_data, 'Expected final tick (59) in response'

    # Test 3: Send another /long request on same session
    logging.info('=== Test 2: Sending another /long request on same session ===')
    conn.request('GET', '/long?test=sequential3')

    # Verify third request is processed
    dut.expect('uri: /long', timeout=30)
    dut.expect('Found query string => test=sequential3', timeout=30)

    # Get response for third request
    response_long2 = conn.getresponse()
    logging.info(f'Response status for second /long: {response_long2.status}')
    assert response_long2.status == 200, 'Failed to access second /long URI on same session'

    # Verify we got the full response
    response_data2 = response_long2.read().decode()
    assert 'req: 2' in response_data2, 'Expected request count 2 in response'

    conn.close()
    logging.info('=== Test completed successfully: Same session sequential requests work ===')


@pytest.mark.ethernet
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_http_server_async_handler_force_close_and_recovery(dut: Dut) -> None:
    """
    Test that verifies server robustness:
    1. Send /long request and forcefully close before completion
    2. Verify server doesn't crash
    3. Send another /long request and verify it completes successfully
    4. While second /long is running, hit /quick to verify concurrent handling
    """
    # Get binary file
    binary_file = os.path.join(dut.app.binary_path, 'simple.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info(f'http_server_bin_size : {bin_size // 1024}KB')
    logging.info('Waiting to connect with Ethernet')

    # Parse IP address of Ethernet
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
    got_port = 80  # Assuming the server is running on port 80
    logging.info(f'Got IP   : {got_ip}')
    dut.expect('starting async req task worker', timeout=30)
    dut.expect('starting async req task worker', timeout=30)
    dut.expect(f"Starting server on port: '{got_port}'", timeout=30)
    dut.expect('Registering URI handlers', timeout=30)
    logging.info(f'Connecting to server at {got_ip}:{got_port}')

    # Test 1: Send /long request and forcefully close connection
    logging.info('=== Test 1: Sending /long request and forcefully closing connection ===')
    conn_force_close = http.client.HTTPConnection(got_ip, got_port, timeout=10)
    conn_force_close.request('GET', '/long?test=force_close')

    # Verify request is received
    dut.expect('uri: /long', timeout=30)
    dut.expect('Found query string => test=force_close', timeout=30)

    # Wait a bit to let the async handler start processing
    time.sleep(10)

    # Forcefully close the connection without reading response
    logging.info('Forcefully closing connection before /long completes')
    conn_force_close.close()

    # Verify server doesn't crash (check for SW_CPU_RESET which indicates a crash)
    try:
        dut.expect('SW_CPU_RESET', timeout=10)
        pytest.fail('Server crashed with SW_CPU_RESET after force close - test failed')
    except Exception:
        # No SW_CPU_RESET found - this is expected, server should remain stable
        logging.info('Server remained stable after force close (no SW_CPU_RESET detected)')

    # Test 2: Verify server is still functional by sending another /long request
    logging.info('=== Test 2: Sending another /long request to verify server recovery ===')
    conn_recovery = http.client.HTTPConnection(got_ip, got_port, timeout=70)
    conn_recovery.request('GET', '/long?test=recovery')

    # Verify request is received
    dut.expect('uri: /long', timeout=30)
    dut.expect('Found query string => test=recovery', timeout=30)

    # Test 3: While /long is running, hit /quick endpoint
    logging.info('=== Test 3: Hitting /quick while /long is running ===')
    time.sleep(5)  # Let /long run for a bit

    conn_quick = http.client.HTTPConnection(got_ip, got_port, timeout=10)
    conn_quick.request('GET', '/quick?test=concurrent')
    dut.expect('uri: /quick', timeout=30)

    # Get /quick response (should complete immediately)
    response_quick = conn_quick.getresponse()
    logging.info(f'Response status for /quick: {response_quick.status}')
    assert response_quick.status == 200, 'Failed to access /quick URI while /long is running'
    quick_data = response_quick.read().decode()
    assert 'random:' in quick_data, 'Expected random number in /quick response'
    conn_quick.close()
    logging.info('/quick request completed successfully while /long was running')

    # Now wait for the /long request to complete
    logging.info('Waiting for /long request to complete...')
    response_long = conn_recovery.getresponse()
    logging.info(f'Response status for /long: {response_long.status}')
    assert response_long.status == 200, 'Failed to complete /long URI after force close recovery'

    # Verify we got the full response
    response_data = response_long.read().decode()
    assert 'req:' in response_data, 'Expected request count in response'
    assert '59' in response_data, 'Expected final tick (59) in response'

    conn_recovery.close()
    logging.info(
        '=== Test completed successfully: Server recovered from force close and handled concurrent requests ==='
    )
