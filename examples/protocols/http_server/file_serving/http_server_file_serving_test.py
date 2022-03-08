#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import hashlib
import http.client
import os
import re

import tiny_test_fw
import ttfw_idf
from idf_http_server_test import adder as client
from tiny_test_fw import Utility


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_Protocols')
def test_examples_protocol_http_server_file_serving(env, _):  # type: (tiny_test_fw.Env.Env, None) -> None
    # Acquire DUT
    dut1 = env.get_dut('http file_serving', 'examples/protocols/http_server/file_serving', dut_class=ttfw_idf.ESP32DUT, app_config_name='spiffs')

    # Get binary file
    binary_file = os.path.join(dut1.app.binary_path, 'file_server.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('file_server_bin_size', '{}KB'.format(bin_size // 1024))
    Utility.console_log('Erasing the flash on the chip')
    # erase the flash
    dut1.erase_flash()
    # Upload binary and start testing
    Utility.console_log('Starting http file serving simple test app')
    dut1.start_app()

    dut1.expect('Initializing SPIFFS', timeout=30)
    # Parse IP address of STA
    Utility.console_log('Waiting to connect with AP')
    got_ip = dut1.expect(re.compile(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)'), timeout=30)[0]
    # Expected logs
    got_port = dut1.expect(re.compile(r"Starting HTTP Server on port: '(\d+)'"), timeout=30)[0]
    Utility.console_log('Got IP   : ' + got_ip)
    Utility.console_log('Got Port : ' + got_port)

    # Run test script
    conn = client.start_session(got_ip, got_port)

    # upload a file onto the server
    upload_data = 'Test data to be sent to the server'

    upload_file_name = 'example.txt'
    upload_file_hash = hashlib.md5(upload_data.encode('UTF-8'))
    upload_file_digest = upload_file_hash.digest()
    Utility.console_log('\nTesting the uploading of file on the file server')
    client.postreq(conn, '/upload/' + str(upload_file_name), upload_data)

    try:
        dut1.expect('File reception complete', timeout=10)
    except Exception:
        Utility.console_log('Failed the test to upload file on the file server')
        raise
    Utility.console_log('Passed the test to uploaded file on the file server')

    # Download the uploaded file from the file server
    Utility.console_log("\nTesting for Download of \"existing\" file from the file server")

    download_data = client.getreq(conn, '/' + str(upload_file_name))

    try:
        dut1.expect('File sending complete', timeout=10)
    except Exception:
        Utility.console_log('Failed the test to download existing file from the file server')
        raise
    Utility.console_log('Passed the test to downloaded existing file from the file server')

    download_file_hash = hashlib.md5(download_data)
    download_file_digest = download_file_hash.digest()

    if download_file_digest != upload_file_digest:
        raise RuntimeError('The md5 hash of the downloaded file does not match with that of the uploaded file')

    # Upload existing file on the file server
    Utility.console_log("\nTesting the upload of \"already existing\" file on the file server")
    client.postreq(conn, '/upload/' + str(upload_file_name), data=None)
    try:
        dut1.expect('File already exists : /data/' + str(upload_file_name), timeout=10)
    except Exception:
        Utility.console_log('Failed the test for uploading existing file on the file server')
        raise
    Utility.console_log('Passed the test for uploading existing file on the file server')
    # Previous URI was an invalid URI so the server should have closed the connection.
    # Trying to send request to the server
    try:
        client.getreq(conn, '/')
    except http.client.RemoteDisconnected:
        # It is correct behavior that the connection was closed by the server
        pass
    except Exception:
        Utility.console_log('Connection was not closed successfully by the server after last invalid URI')
        raise

    conn = client.start_session(got_ip, got_port)
    # Delete the existing file from the file server
    Utility.console_log("\nTesting the deletion of \"existing\" file on the file server")
    client.postreq(conn, '/delete/' + str(upload_file_name), data=None)
    try:
        dut1.expect('Deleting file : /' + str(upload_file_name), timeout=10)
    except Exception:
        Utility.console_log('Failed the test for deletion of existing file on the file server')
        raise
    Utility.console_log('Passed the test for deletion of existing file on the file server')

    conn = client.start_session(got_ip, got_port)
    # Try to delete non existing file from the file server
    Utility.console_log("\nTesting the deletion of \"non existing\" file on the file server")
    client.postreq(conn, '/delete/' + str(upload_file_name), data=None)
    try:
        dut1.expect('File does not exist : /' + str(upload_file_name), timeout=10)
    except Exception:
        Utility.console_log('Failed the test for deleting non existing file on the file server')
        raise
    Utility.console_log('Passed the test for deleting non existing file on the file server')

    conn = client.start_session(got_ip, got_port)
    # Try to download non existing file from the file server
    Utility.console_log("\nTesting for Download of \"non existing\" file from the file server")

    download_data = client.getreq(conn, '/' + str(upload_file_name))

    try:
        dut1.expect('Failed to stat file : /data/' + str(upload_file_name), timeout=10)
    except Exception:
        Utility.console_log('Failed the test to download non existing file from the file server')
        raise
    Utility.console_log('Passed the test to downloaded non existing file from the file server')


if __name__ == '__main__':
    test_examples_protocol_http_server_file_serving()
