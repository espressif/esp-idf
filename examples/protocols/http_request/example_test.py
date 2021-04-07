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

import os
import re

import tiny_test_fw
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_EthKitV1')
def test_examples_protocol_http_request(env, extra_data):  # type: (tiny_test_fw.Env.Env, None) -> None # pylint: disable=unused-argument
    """
    steps: |
      1. join AP
      2. connect to example.com
      3. check conneciton success
    """
    dut1 = env.get_dut('http_request', 'examples/protocols/http_request', dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'http-request.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('http_request_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    dut1.start_app()
    dut1.expect(re.compile(r'DNS lookup succeeded.'))
    # check if connected or not
    dut1.expect(' ... connected', timeout=60)
    dut1.expect(' ... socket send success')
    dut1.expect(' ... set socket receiving timeout success')
    # check server response
    dut1.expect(re.compile(r'HTTP/1.0 200 OK'))
    # read from the socket completed
    dut1.expect('... done reading from socket. Last read return=0 errno=128')
    dut1.expect(re.compile(r'(\d)...'))


if __name__ == '__main__':
    test_examples_protocol_http_request()  # pylint: disable=no-value-for-parameter
