#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import hashlib
import http.client
import logging
import os
import sys

import pytest
from pytest_embedded_idf.utils import idf_parametrize

try:
    from idf_http_server_test import adder as client
except ModuleNotFoundError:
    sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', 'tools', 'ci', 'python_packages'))
    from idf_http_server_test import adder as client

from common_test_methods import get_env_config_variable
from pytest_embedded import Dut


@pytest.mark.wifi_router
@pytest.mark.parametrize(
    'config',
    [
        'spiffs',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32c3', 'esp32s3'], indirect=['target'])
def test_examples_protocol_http_server_file_serving(dut: Dut) -> None:
    # Get binary file
    binary_file = os.path.join(dut.app.binary_path, 'file_server.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info(f'file_server_bin_size : {bin_size // 1024}KB')
    logging.info('Erasing the storage partition on the chip')
    dut.serial.erase_partition('storage')
    # Upload binary and start testing
    logging.info('Starting http file serving simple test app')

    dut.expect('Initializing SPIFFS', timeout=60)

    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')

    # Parse IP address of STA
    logging.info('Waiting to connect with AP')
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=60)[1].decode()
    # Expected logs
    got_port = dut.expect(r"Starting HTTP Server on port: '(\d+)'", timeout=30)[1].decode()
    logging.info(f'Got IP   : {got_ip}')
    logging.info(f'Got Port : {got_port}')

    # Run test script
    conn = client.start_session(got_ip, got_port)

    # upload a file onto the server
    upload_data = 'Test data to be sent to the server'

    upload_file_name = 'example.txt'
    upload_file_hash = hashlib.md5(upload_data.encode('UTF-8'))
    upload_file_digest = upload_file_hash.digest()
    logging.info('\nTesting the uploading of file on the file server')
    client.postreq(conn, '/upload/' + str(upload_file_name), upload_data)

    try:
        dut.expect('File reception complete', timeout=10)
    except Exception:
        logging.info('Failed the test to upload file on the file server')
        raise
    logging.info('Passed the test to uploaded file on the file server')

    # Download the uploaded file from the file server
    logging.info('\nTesting for Download of "existing" file from the file server')

    download_data = client.getreq(conn, '/' + str(upload_file_name))

    try:
        dut.expect('File sending complete', timeout=20)
    except Exception:
        logging.info('Failed the test to download existing file from the file server')
        raise
    logging.info('Passed the test to downloaded existing file from the file server')

    download_file_hash = hashlib.md5(download_data)
    download_file_digest = download_file_hash.digest()

    if download_file_digest != upload_file_digest:
        raise RuntimeError('The md5 hash of the downloaded file does not match with that of the uploaded file')

    # Upload existing file on the file server
    logging.info('\nTesting the upload of "already existing" file on the file server')
    client.postreq(conn, '/upload/' + str(upload_file_name), data=None)
    try:
        dut.expect('File already exists : /data/' + str(upload_file_name), timeout=20)
    except Exception:
        logging.info('Failed the test for uploading existing file on the file server')
        raise
    logging.info('Passed the test for uploading existing file on the file server')
    # Previous URI was an invalid URI so the server should have closed the connection.
    # Trying to send request to the server
    try:
        client.getreq(conn, '/')
    except http.client.RemoteDisconnected:
        # It is correct behavior that the connection was closed by the server
        pass
    except Exception:
        logging.info('Connection was not closed successfully by the server after last invalid URI')
        raise

    conn = client.start_session(got_ip, got_port)
    # Delete the existing file from the file server
    logging.info('\nTesting the deletion of "existing" file on the file server')
    client.postreq(conn, '/delete/' + str(upload_file_name), data=None)
    try:
        dut.expect('Deleting file : /' + str(upload_file_name), timeout=20)
    except Exception:
        logging.info('Failed the test for deletion of existing file on the file server')
        raise
    logging.info('Passed the test for deletion of existing file on the file server')

    conn = client.start_session(got_ip, got_port)
    # Try to delete non existing file from the file server
    logging.info('\nTesting the deletion of "non existing" file on the file server')
    client.postreq(conn, '/delete/' + str(upload_file_name), data=None)
    try:
        dut.expect('File does not exist : /' + str(upload_file_name), timeout=20)
    except Exception:
        logging.info('Failed the test for deleting non existing file on the file server')
        raise
    logging.info('Passed the test for deleting non existing file on the file server')

    conn = client.start_session(got_ip, got_port)
    # Try to download non existing file from the file server
    logging.info('\nTesting for Download of "non existing" file from the file server')

    download_data = client.getreq(conn, '/' + str(upload_file_name))

    try:
        dut.expect('Failed to stat file : /data/' + str(upload_file_name), timeout=20)
    except Exception:
        logging.info('Failed the test to download non existing file from the file server')
        raise
    logging.info('Passed the test to downloaded non existing file from the file server')
