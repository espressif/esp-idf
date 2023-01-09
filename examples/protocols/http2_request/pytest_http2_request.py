#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import http.client
import logging
import os

import pytest
from pytest_embedded import Dut

HTTP_OK = 200
TEST_SERVER = 'http2.github.io'


def is_test_server_available():  # type: () -> bool
    # 443 - default https port
    try:
        conn = http.client.HTTPSConnection(TEST_SERVER, 443, timeout=10)
        conn.request('GET', '/')
        resp = conn.getresponse()
        return True if resp.status == HTTP_OK else False
    except Exception as msg:
        logging.info('Exception occurred when connecting to {}: {}'.format(TEST_SERVER, msg))
        return False
    finally:
        conn.close()


@pytest.mark.esp32
@pytest.mark.ethernet
def test_examples_protocol_http2_request(dut: Dut) -> None:
    """
    steps: |
      1. join AP
      2. connect to http2.github.io
      3. send http2 request
      4. send http2 put response
    """
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'http2_request.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('http2_request_bin_size : {}KB'.format(bin_size // 1024))
    # start the test
    # check if test server is avilable
    test_server_available = is_test_server_available()
    # Skip the test if the server test server (http2.github.io) is not available at the moment.
    if test_server_available:
        logging.info('test server \"{}\" is available'.format(TEST_SERVER))
        # check for connection
        dut.expect('Connection done', timeout=30)
        # check for get response
        dut.expect('Frame fully received')
    else:
        logging.info('test server \"{0}\" is not available at the moment.\nSkipping the test with status = success.'.format(TEST_SERVER))
