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
from tiny_test_fw import Utility


@ttfw_idf.idf_example_test(env_tag='Example_EthKitV1')
def test_examples_protocol_https_mbedtls(env, extra_data):  # type: (tiny_test_fw.Env.Env, None) -> None # pylint: disable=unused-argument
    """
    steps: |
      1. join AP
      2. connect to www.howsmyssl.com:443
      3. send http request
    """
    app_name = 'https_mbedtls'
    dut1 = env.get_dut(app_name, 'examples/protocols/https_mbedtls', dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'https-mbedtls.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('https_mbedtls_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    dut1.start_app()
    dut1.expect('Connected.', timeout=30)
    Utility.console_log('TCP connection established with the server\n performing SSL/TLS handshake')
    dut1.expect('Performing the SSL/TLS handshake...')
    dut1.expect('Certificate verified.')
    Utility.console_log('SSL/TLS handshake successful')
    dut1.expect('Writing HTTP request...')
    dut1.expect('Reading HTTP response...')
    dut1.expect(re.compile(r'Completed (\d) requests'))

    # Read free heap size
    res = dut1.expect(ttfw_idf.MINIMUM_FREE_HEAP_SIZE_RE)
    if not res:
        raise ValueError('Maximum heap size info not found')
    ttfw_idf.print_heap_size(app_name, dut1.app.config_name, dut1.TARGET, res[0])


if __name__ == '__main__':
    test_examples_protocol_https_mbedtls()  # pylint: disable=no-value-for-parameter
