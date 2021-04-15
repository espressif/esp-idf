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
import re
import ssl

import tiny_test_fw
import ttfw_idf
from tiny_test_fw import Utility

server_cert_pem = '-----BEGIN CERTIFICATE-----\n'\
                  'MIIDKzCCAhOgAwIBAgIUBxM3WJf2bP12kAfqhmhhjZWv0ukwDQYJKoZIhvcNAQEL\n'\
                  'BQAwJTEjMCEGA1UEAwwaRVNQMzIgSFRUUFMgc2VydmVyIGV4YW1wbGUwHhcNMTgx\n'\
                  'MDE3MTEzMjU3WhcNMjgxMDE0MTEzMjU3WjAlMSMwIQYDVQQDDBpFU1AzMiBIVFRQ\n'\
                  'UyBzZXJ2ZXIgZXhhbXBsZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n'\
                  'ALBint6nP77RCQcmKgwPtTsGK0uClxg+LwKJ3WXuye3oqnnjqJCwMEneXzGdG09T\n'\
                  'sA0SyNPwrEgebLCH80an3gWU4pHDdqGHfJQa2jBL290e/5L5MB+6PTs2NKcojK/k\n'\
                  'qcZkn58MWXhDW1NpAnJtjVniK2Ksvr/YIYSbyD+JiEs0MGxEx+kOl9d7hRHJaIzd\n'\
                  'GF/vO2pl295v1qXekAlkgNMtYIVAjUy9CMpqaQBCQRL+BmPSJRkXBsYk8GPnieS4\n'\
                  'sUsp53DsNvCCtWDT6fd9D1v+BB6nDk/FCPKhtjYOwOAZlX4wWNSZpRNr5dfrxKsb\n'\
                  'jAn4PCuR2akdF4G8WLUeDWECAwEAAaNTMFEwHQYDVR0OBBYEFMnmdJKOEepXrHI/\n'\
                  'ivM6mVqJgAX8MB8GA1UdIwQYMBaAFMnmdJKOEepXrHI/ivM6mVqJgAX8MA8GA1Ud\n'\
                  'EwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBADiXIGEkSsN0SLSfCF1VNWO3\n'\
                  'emBurfOcDq4EGEaxRKAU0814VEmU87btIDx80+z5Dbf+GGHCPrY7odIkxGNn0DJY\n'\
                  'W1WcF+DOcbiWoUN6DTkAML0SMnp8aGj9ffx3x+qoggT+vGdWVVA4pgwqZT7Ybntx\n'\
                  'bkzcNFW0sqmCv4IN1t4w6L0A87ZwsNwVpre/j6uyBw7s8YoJHDLRFT6g7qgn0tcN\n'\
                  'ZufhNISvgWCVJQy/SZjNBHSpnIdCUSJAeTY2mkM4sGxY0Widk8LnjydxZUSxC3Nl\n'\
                  'hb6pnMh3jRq4h0+5CZielA4/a+TdrNPv/qok67ot/XJdY3qHCCd8O2b14OVq9jo=\n'\
                  '-----END CERTIFICATE-----\n'

success_response = '<h1>Hello Secure World!</h1>'


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_Protocols')
def test_examples_protocol_https_server_simple(env, extra_data):  # type: (tiny_test_fw.Env.Env, None) -> None # pylint: disable=unused-argument
    """
    steps: |
      1. join AP
      2. connect to www.howsmyssl.com:443
      3. send http request
    """
    dut1 = env.get_dut('https_server_simple', 'examples/protocols/https_server/simple', dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'https_server.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('https_server_simple_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    dut1.start_app()
    # Parse IP address and port of the server
    dut1.expect(re.compile(r'Starting server'))
    got_port = dut1.expect(re.compile(r'Server listening on port (\d+)'), timeout=30)[0]
    Utility.console_log('Waiting to connect with AP')

    got_ip = dut1.expect(re.compile(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)'), timeout=30)[0]
    # Expected logs

    Utility.console_log('Got IP   : ' + got_ip)
    Utility.console_log('Got Port : ' + got_port)

    Utility.console_log('Performing GET request over an SSL connection with the server')

    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLSv1)
    ssl_context.verify_mode = ssl.CERT_REQUIRED
    ssl_context.check_hostname = False
    ssl_context.load_verify_locations(cadata=server_cert_pem)
    conn = http.client.HTTPSConnection(got_ip, got_port, context=ssl_context)
    Utility.console_log('Performing SSL handshake with the server')
    conn.request('GET','/')
    resp = conn.getresponse()
    dut1.expect('performing session handshake')
    got_resp = resp.read().decode('utf-8')
    # Close the connection
    if got_resp != success_response:
        Utility.console_log('Response obtained does not match with correct response')
        raise RuntimeError('Failed to test SSL connection')

    Utility.console_log('Correct response obtained')
    Utility.console_log('SSL connection test successful\nClosing the connection')
    conn.close()


if __name__ == '__main__':
    test_examples_protocol_https_server_simple()  # pylint: disable=no-value-for-parameter
