#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import http.client
import os

import tiny_test_fw
import ttfw_idf
from tiny_test_fw import Utility

HTTP_OK = 200
TEST_SERVER = 'http2.github.io'


def is_test_server_available():  # type: () -> bool
    # 443 - default https port
    conn = http.client.HTTPSConnection(TEST_SERVER, 443, timeout=10)
    conn.request('GET', '/')
    resp = conn.getresponse()
    conn.close()
    if resp.status == HTTP_OK:
        return True
    return False


# Disabling the Test in CI as the leaf certificate of http2.golang.org is expired from 8 July.
# There is no timeline when the cert will be updated.
# Disabling this test till an alternative is found for testing the http2 support.
@ttfw_idf.idf_example_test(env_tag='Example_EthKitV1', ignore=True)
def test_examples_protocol_http2_request(env, extra_data):  # type: (tiny_test_fw.Env.Env, None) -> None # pylint: disable=unused-argument
    """
    steps: |
      1. join AP
      2. connect to http2.github.io
      3. send http2 request
      4. send http2 put response
    """
    dut1 = env.get_dut('http2_request', 'examples/protocols/http2_request', dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'http2_request.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('http2_request_bin_size', '{}KB'.format(bin_size // 1024))
    # start the test
    # check if test server is avilable
    test_server_available = is_test_server_available()
    # Skip the test if the server test server (http2.github.io) is not available at the moment.
    if test_server_available:
        Utility.console_log('test server \"{}\" is available'.format(TEST_SERVER))
        dut1.start_app()
        # check for connection
        dut1.expect('Connection done', timeout=30)
        # check for get response
        dut1.expect('[get-response] Frame fully received')
    else:
        Utility.console_log('test server \"{0}\" is not available at the moment.\nSkipping the test with status = success.'.format(TEST_SERVER))


if __name__ == '__main__':
    test_examples_protocol_http2_request()  # pylint: disable=no-value-for-parameter
