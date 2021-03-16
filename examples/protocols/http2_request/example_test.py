#!/usr/bin/env python
#
# Copyright 2021 Espressif Systems (Shanghai) CO LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import http.client
import os

import tiny_test_fw
import ttfw_idf
from tiny_test_fw import Utility

HTTP_OK = 200
TEST_SERVER = 'http2.golang.org'


def is_test_server_available():  # type: () -> bool
    # 443 - default https port
    conn = http.client.HTTPSConnection(TEST_SERVER, 443, timeout=10)
    conn.request('GET', '/')
    resp = conn.getresponse()
    conn.close()
    if resp.status == HTTP_OK:
        return True
    return False


@ttfw_idf.idf_example_test(env_tag='Example_EthKitV1')
def test_examples_protocol_http2_request(env, extra_data):  # type: (tiny_test_fw.Env.Env, None) -> None # pylint: disable=unused-argument
    """
    steps: |
      1. join AP
      2. connect to http2.golang.org
      3. send http2 request
      4. send http2 put response
    """
    dut1 = env.get_dut('http2_request', 'examples/protocols/http2_request', dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'http2-request.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('http2_request_bin_size', '{}KB'.format(bin_size // 1024))
    # start the test
    # check if test server is avilable
    test_server_available = is_test_server_available()
    # Skip the test if the server test server (http2.golang.org) is not available at the moment.
    if test_server_available:
        dut1.start_app()
        # check for connection
        dut1.expect('Connection done', timeout=30)
        # check for echo response
        dut1.expect('[echo-response] HELLO WORLD', timeout=30)
        dut1.expect('[echo-response] Frame fully received')
        dut1.expect('[echo-response] Stream Closed')
        # check for get response
        dut1.expect('[get-response] Frame fully received')
    else:
        Utility.console_log('test server \"{0}\" is not available at the moment.\nSkipping the test with status = success.'.format(TEST_SERVER))


if __name__ == '__main__':
    test_examples_protocol_http2_request()  # pylint: disable=no-value-for-parameter
