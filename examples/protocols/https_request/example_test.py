import os
import re

import ttfw_idf
from tiny_test_fw import Utility


@ttfw_idf.idf_example_test(env_tag='Example_EthKitV1')
def test_examples_protocol_https_request(env, extra_data):
    """
    steps: |
      1. join AP
      2. establish TLS connection to www.howsmyssl.com:443 with multiple
         certificate verification options
      3. send http request
    """
    dut1 = env.get_dut('https_request', 'examples/protocols/https_request', dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'https_request.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('https_request_bin_size', '{}KB'.format(bin_size // 1024))
    # start tes
    Utility.console_log('Starting https_request simple test app')
    dut1.start_app()

    # Check for connection using crt bundle
    Utility.console_log("Testing for \"https_request using crt bundle\"")
    try:
        dut1.expect(re.compile('https_request using crt bundle'), timeout=30)
        dut1.expect_all('Certificate validated',
                        'Connection established...',
                        'Reading HTTP response...',
                        'HTTP/1.1 200 OK',
                        re.compile('connection closed'))
    except Exception:
        Utility.console_log("Failed the test for \"https_request using crt bundle\"")
        raise
    Utility.console_log("Passed the test for \"https_request using crt bundle\"")

    # Check for connection using cacert_buf
    Utility.console_log("Testing for \"https_request using cacert_buf\"")
    try:
        dut1.expect(re.compile('https_request using cacert_buf'), timeout=20)
        dut1.expect_all('Connection established...',
                        'Reading HTTP response...',
                        'HTTP/1.1 200 OK',
                        re.compile('connection closed'))
    except Exception:
        Utility.console_log("Passed the test for \"https_request using cacert_buf\"")
        raise
    Utility.console_log("Passed the test for \"https_request using cacert_buf\"")

    # Check for connection using global ca_store
    Utility.console_log("Testing for \"https_request using global ca_store\"")
    try:
        dut1.expect(re.compile('https_request using global ca_store'), timeout=20)
        dut1.expect_all('Connection established...',
                        'Reading HTTP response...',
                        'HTTP/1.1 200 OK',
                        re.compile('connection closed'))
    except Exception:
        Utility.console_log("Failed the test for \"https_request using global ca_store\"")
        raise
    Utility.console_log("Passed the test for \"https_request using global ca_store\"")

    # Check for connection using already saved client session
    Utility.console_log("Testing for \"https_request using saved client session\"")
    try:
        dut1.expect(re.compile('https_request using saved client session'), timeout=20)
        dut1.expect_all('Connection established...',
                        'Reading HTTP response...',
                        'HTTP/1.1 200 OK',
                        re.compile('connection closed'))
    except Exception:
        Utility.console_log("Failed the test for \"https_request using saved client session\"")
        raise
    Utility.console_log("Passed the test for \"https_request using saved client session\"")

    # Check for connection using crt bundle with mbedtls dynamic resource enabled
    dut1 = env.get_dut('https_request', 'examples/protocols/https_request', dut_class=ttfw_idf.ESP32DUT, app_config_name='ssldyn')
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'https_request.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('https_request_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    dut1.start_app()
    # only check if one connection is established
    Utility.console_log("Testing for \"https_request using crt bundle\" with mbedtls dynamic resource enabled")
    try:
        dut1.expect(re.compile('https_request using crt bundle'), timeout=30)
        dut1.expect_all('Connection established...',
                        'Reading HTTP response...',
                        'HTTP/1.1 200 OK',
                        re.compile('connection closed'))
    except Exception:
        Utility.console_log("Failed the test for \"https_request using crt bundle\" when mbedtls dynamic resource was enabled")
        raise
    Utility.console_log("Passed the test for \"https_request using crt bundle\" when mbedtls dynamic resource was enabled")


if __name__ == '__main__':
    test_examples_protocol_https_request()
