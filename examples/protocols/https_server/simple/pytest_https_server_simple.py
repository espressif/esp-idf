#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import http.client
import logging
import os
import ssl

import pytest
from common_test_methods import get_env_config_variable
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

CURRENT_FILE_PATH = os.path.dirname(__file__)

CACERT_FILE_PATH = os.path.join(CURRENT_FILE_PATH, './main/certs/cacert.pem')
CLIENT_CERT_PATH = os.path.join(CURRENT_FILE_PATH, './main/certs/client_cert.pem')
CLIENT_KEY_PATH = os.path.join(CURRENT_FILE_PATH, './main/certs/client_key.pem')

success_response = '<h1>Hello Secure World!</h1>'


@pytest.mark.wifi_router
@idf_parametrize('target', ['esp32', 'esp32c3', 'esp32s3'], indirect=['target'])
def test_examples_protocol_https_server_simple(dut: Dut) -> None:
    """
    steps: |
      1. join AP
      2. connect to www.howsmyssl.com:443
      3. send http request
    """
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'https_server.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info(f'https_server_simple_bin_size : {bin_size // 1024}KB')
    # start test
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    # Parse IP address and port of the server
    dut.expect(r'Starting server')
    got_port = int(dut.expect(r'Server listening on port (\d+)', timeout=30)[1].decode())
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=60)[1].decode()

    # Expected logs

    logging.info(f'Got IP   : {got_ip}')
    logging.info(f'Got Port : {got_port}')

    # Try requesting without client certificate and it should fail if client cert is required
    logging.info('Performing GET request over an SSL connection with the server without client certificate')
    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
    ssl_context.verify_mode = ssl.CERT_REQUIRED
    ssl_context.check_hostname = False
    # Load the CA certificate from the path
    ssl_context.load_verify_locations(cafile=CACERT_FILE_PATH)
    conn = http.client.HTTPSConnection(got_ip, got_port, context=ssl_context)
    logging.info('Performing SSL handshake with the server without client certificate')
    try:
        conn.request('GET', '/')
        resp = conn.getresponse()
        resp.read().decode('utf-8')
    except Exception as e:
        if dut.app.sdkconfig.get('EXAMPLE_ENABLE_SKIP_CLIENT_CERT') is True:
            logging.info('SSL handshake failed without client certificate but was expected to be successful')
            raise RuntimeError('Failed to test SSL connection') from e
        else:
            logging.info('SSL handshake failed without client certificate as expected')
    finally:
        conn.close()

    logging.info('Performing GET request over an SSL connection with the server')

    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
    ssl_context.verify_mode = ssl.CERT_REQUIRED
    ssl_context.check_hostname = False
    ssl_context.load_verify_locations(cafile=CACERT_FILE_PATH)

    ssl_context.load_cert_chain(certfile=CLIENT_CERT_PATH, keyfile=CLIENT_KEY_PATH)

    conn = http.client.HTTPSConnection(got_ip, got_port, context=ssl_context)
    logging.info('Performing SSL handshake with the server')
    conn.request('GET', '/')
    resp = conn.getresponse()
    dut.expect('performing session handshake')
    got_resp = resp.read().decode('utf-8')
    if got_resp != success_response:
        logging.info('Response obtained does not match with correct response')
        raise RuntimeError('Failed to test SSL connection')

    if dut.app.sdkconfig.get('EXAMPLE_ENABLE_HTTPS_USER_CALLBACK') is True:
        current_cipher = dut.expect(r'Current Ciphersuite(.*)', timeout=5)[0]
        logging.info(f'Current Ciphersuite {current_cipher}')

        conn.close()

        logging.info('Checking user callback: Obtaining client certificate...')

        serial_number = dut.expect(r'serial number\s*:([^\n]*)', timeout=5)[0]
        issuer_name = dut.expect(r'issuer name\s*:([^\n]*)', timeout=5)[0]
        expiry = dut.expect(r'expires on ((.*)\d{4}\-(0?[1-9]|1[012])\-(0?[1-9]|[12][0-9]|3[01])*)', timeout=5)[
            1
        ].decode()

        logging.info(f'Serial No. {serial_number}')
        logging.info(f'Issuer Name {issuer_name}')
        logging.info(f'Expires on {expiry}')
    logging.info('Correct response obtained')
    logging.info('SSL connection test successful\nClosing the connection')


@pytest.mark.wifi_router
@pytest.mark.parametrize(
    'config',
    [
        'dynamic_buffer',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32c3', 'esp32s3'], indirect=['target'])
def test_examples_protocol_https_server_simple_dynamic_buffers(dut: Dut) -> None:
    # Test with mbedTLS dynamic buffer feature

    # start test
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    # Parse IP address and port of the server
    dut.expect(r'Starting server')
    got_port = int(dut.expect(r'Server listening on port (\d+)', timeout=30)[1].decode())
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=60)[1].decode()

    # Expected logs

    logging.info(f'Got IP   : {got_ip}')
    logging.info(f'Got Port : {got_port}')

    logging.info('Performing GET request over an SSL connection with the server')

    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
    ssl_context.verify_mode = ssl.CERT_REQUIRED
    ssl_context.check_hostname = False
    ssl_context.load_verify_locations(cafile=CACERT_FILE_PATH)

    ssl_context.load_cert_chain(certfile=CLIENT_CERT_PATH, keyfile=CLIENT_KEY_PATH)

    conn = http.client.HTTPSConnection(got_ip, got_port, context=ssl_context)
    logging.info('Performing SSL handshake with the server')
    conn.request('GET', '/')
    resp = conn.getresponse()
    dut.expect('performing session handshake')
    got_resp = resp.read().decode('utf-8')
    if got_resp != success_response:
        logging.info('Response obtained does not match with correct response')
        raise RuntimeError('Failed to test SSL connection')

    if dut.app.sdkconfig.get('EXAMPLE_ENABLE_HTTPS_USER_CALLBACK') is True:
        current_cipher = dut.expect(r'Current Ciphersuite(.*)', timeout=5)[0]
        logging.info(f'Current Ciphersuite {current_cipher}')

        # Close the connection
        conn.close()

        logging.info('Checking user callback: Obtaining client certificate...')

        serial_number = dut.expect(r'serial number\s*:([^\n]*)', timeout=5)[0]
        issuer_name = dut.expect(r'issuer name\s*:([^\n]*)', timeout=5)[0]
        expiry = dut.expect(r'expires on\s*:((.*)\d{4}\-(0?[1-9]|1[012])\-(0?[1-9]|[12][0-9]|3[01])*)', timeout=5)[
            1
        ].decode()

        logging.info(f'Serial No. : {serial_number}')
        logging.info(f'Issuer Name : {issuer_name}')
        logging.info(f'Expires on : {expiry}')

    logging.info('Correct response obtained')
    logging.info('SSL connection test successful\nClosing the connection')


@pytest.mark.wifi_router
@pytest.mark.parametrize(
    'config',
    [
        'tls1_3',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32c3', 'esp32s3'], indirect=['target'])
def test_examples_protocol_https_server_tls1_3(dut: Dut) -> None:
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    # Parse IP address and port of the server
    dut.expect(r'Starting server')
    got_port = int(dut.expect(r'Server listening on port (\d+)', timeout=30)[1].decode())
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=60)[1].decode()

    # Expected logs
    logging.info(f'Got IP   : {got_ip}')
    logging.info(f'Got Port : {got_port}')
    logging.info('Performing GET request over an SSL connection with the server using TLSv1.3')

    # First try requesting without client certificate and it should also pass if client cert is optional or none
    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    ssl_context.minimum_version = ssl.TLSVersion.TLSv1_3
    ssl_context.maximum_version = ssl.TLSVersion.TLSv1_3
    ssl_context.check_hostname = False
    ssl_context.verify_mode = ssl.CERT_REQUIRED
    ssl_context.load_verify_locations(cafile=CACERT_FILE_PATH)

    conn = http.client.HTTPSConnection(got_ip, got_port, context=ssl_context)
    logging.info('Performing SSL handshake with the server without client certificate')
    try:
        conn.request('GET', '/')
        resp = conn.getresponse()
        resp.read().decode('utf-8')
        if dut.app.sdkconfig.get('EXAMPLE_ENABLE_SKIP_CLIENT_CERT') is True:
            dut.expect('performing session handshake')
            logging.info('SSL handshake successful without client certificate as expected')
        else:
            logging.info('SSL handshake successful without client certificate but was expected to fail')
            raise RuntimeError('Failed to test SSL connection')
    except Exception as e:
        if dut.app.sdkconfig.get('EXAMPLE_ENABLE_SKIP_CLIENT_CERT') is True:
            logging.info(f'SSL handshake failed without client certificate but was expected to be successful: {e}')
            raise RuntimeError('Failed to test SSL connection')
        else:
            logging.info('SSL handshake failed without client certificate as expected')
    finally:
        conn.close()

    # First try with TLSv1.2 and that should fail
    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    ssl_context.minimum_version = ssl.TLSVersion.TLSv1_2
    ssl_context.maximum_version = ssl.TLSVersion.TLSv1_2
    ssl_context.verify_mode = ssl.CERT_REQUIRED
    ssl_context.check_hostname = False
    ssl_context.load_verify_locations(cafile=CACERT_FILE_PATH)
    ssl_context.load_cert_chain(certfile=CLIENT_CERT_PATH, keyfile=CLIENT_KEY_PATH)
    conn = http.client.HTTPSConnection(got_ip, got_port, context=ssl_context)
    try:
        conn.request('GET', '/')
    except ssl.SSLError as e:
        logging.info(f'SSL handshake failed with TLSv1.2: {e}')
    else:
        logging.info('SSL handshake succeeded with TLSv1.2')
        raise RuntimeError('This should have failed')

    ssl_context.minimum_version = ssl.TLSVersion.TLSv1_3
    ssl_context.maximum_version = ssl.TLSVersion.TLSv1_3

    conn = http.client.HTTPSConnection(got_ip, got_port, context=ssl_context)
    logging.info('Performing SSL handshake with the server')
    conn.request('GET', '/')
    resp = conn.getresponse()
    dut.expect('performing session handshake')
    got_resp = resp.read().decode('utf-8')
    if got_resp != success_response:
        logging.info('Response obtained does not match with correct response')
        raise RuntimeError('Failed to test SSL connection')

    if dut.app.sdkconfig.get('EXAMPLE_ENABLE_HTTPS_USER_CALLBACK') is True:
        current_cipher = dut.expect(r'Current Ciphersuite(.*)', timeout=5)[0]
        logging.info(f'Current Ciphersuite {current_cipher}')

        conn.close()

        logging.info('Checking user callback: Obtaining client certificate...')

        serial_number = dut.expect(r'serial number\s*:([^\n]*)', timeout=5)[0]
        issuer_name = dut.expect(r'issuer name\s*:([^\n]*)', timeout=5)[0]
        expiry = dut.expect(
            r'expires on\s*:((.*)\d{4}\-(0?[1-9]|1[012])\-(0?[1-9]|[12][0-9]|3[01])*)',
            timeout=5,
        )[1].decode()

        logging.info(f'Serial No. : {serial_number}')
        logging.info(f'Issuer Name : {issuer_name}')
        logging.info(f'Expires on : {expiry}')

    logging.info('Correct response obtained')
    logging.info('SSL connection test successful\nClosing the connection')


@pytest.mark.wifi_router
@pytest.mark.parametrize(
    'config',
    [
        'tls1_2_only',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32c3', 'esp32s3'], indirect=['target'])
def test_examples_protocol_https_server_tls1_2_only(dut: Dut) -> None:
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    # Parse IP address and port of the server
    dut.expect(r'Starting server')
    got_port = int(dut.expect(r'Server listening on port (\d+)', timeout=30)[1].decode())
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=60)[1].decode()

    # Expected logs
    logging.info(f'Got IP   : {got_ip}')
    logging.info(f'Got Port : {got_port}')
    logging.info('Performing GET request over an SSL connection with the server using TLSv1.2')

    # First try with TLSv1.3 and that should fail
    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    ssl_context.minimum_version = ssl.TLSVersion.TLSv1_3
    ssl_context.maximum_version = ssl.TLSVersion.TLSv1_3
    ssl_context.verify_mode = ssl.CERT_REQUIRED
    ssl_context.check_hostname = False
    ssl_context.load_verify_locations(cafile=CACERT_FILE_PATH)
    # ssl_context.load_verify_locations(cadata=server_cert_pem)
    ssl_context.load_cert_chain(certfile=CLIENT_CERT_PATH, keyfile=CLIENT_KEY_PATH)
    conn = http.client.HTTPSConnection(got_ip, got_port, context=ssl_context)
    try:
        conn.request('GET', '/')
    except ssl.SSLError as e:
        logging.info(f'SSL handshake failed with TLSv1.3: {e}')
    else:
        logging.info('SSL handshake succeeded with TLSv1.3')
        raise RuntimeError('This should have failed')

    ssl_context.minimum_version = ssl.TLSVersion.TLSv1_2
    ssl_context.maximum_version = ssl.TLSVersion.TLSv1_2

    # Also now with TLS1.2, try with a non matching ciphersuite and that should fail
    # Server only accepts: DHE-RSA-AES128-SHA256, DHE-RSA-AES256-SHA256,
    # ECDHE-RSA-AES256-SHA384, ECDHE-RSA-AES128-SHA256
    # Try AES128-GCM-SHA256 which is NOT in the list
    ssl_context.set_ciphers('AES128-GCM-SHA256')

    conn = http.client.HTTPSConnection(got_ip, got_port, context=ssl_context)
    try:
        logging.info('Trying SSL handshake with non-matching ciphersuite (should fail)')
        conn.request('GET', '/')
    except ssl.SSLError as e:
        logging.info(f'SSL handshake failed with non-matching ciphersuite (expected): {e}')
    else:
        logging.info('SSL handshake succeeded with non-matching ciphersuite')
        raise RuntimeError('This should have failed - custom ciphersuites not enforced')
    finally:
        conn.close()

    # Now try with the matching ciphersuite
    ssl_context.set_ciphers('ECDHE-RSA-AES128-SHA256')

    conn = http.client.HTTPSConnection(got_ip, got_port, context=ssl_context)
    logging.info('Performing SSL handshake with the server')
    conn.request('GET', '/')
    resp = conn.getresponse()
    dut.expect('performing session handshake')
    got_resp = resp.read().decode('utf-8')
    if got_resp != success_response:
        logging.info('Response obtained does not match with correct response')
        raise RuntimeError('Failed to test SSL connection')

    if dut.app.sdkconfig.get('EXAMPLE_ENABLE_HTTPS_USER_CALLBACK') is True:
        current_cipher = dut.expect(r'Current Ciphersuite(.*)', timeout=5)[0]
        logging.info(f'Current Ciphersuite {current_cipher}')

        logging.info('Checking user callback: Obtaining client certificate...')

        serial_number = dut.expect(r'serial number\s*:([^\n]*)', timeout=5)[0]
        issuer_name = dut.expect(r'issuer name\s*:([^\n]*)', timeout=5)[0]
        expiry = dut.expect(
            r'expires on\s*:((.*)\d{4}\-(0?[1-9]|1[012])\-(0?[1-9]|[12][0-9]|3[01])*)',
            timeout=5,
        )[1].decode()

        logging.info(f'Serial No. : {serial_number}')
        logging.info(f'Issuer Name : {issuer_name}')
        logging.info(f'Expires on : {expiry}')

    logging.info('Correct response obtained')
    logging.info('SSL connection test successful\nClosing the connection')
