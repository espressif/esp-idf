import os
import re

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_Protocols', ignore=True)
def test_examples_protocol_https_x509_bundle(env, extra_data):
    """
    steps: |
      1. join AP
      2. connect to multiple URLs
      3. send http request
    """
    dut1 = env.get_dut('https_x509_bundle', 'examples/protocols/https_x509_bundle')
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'https_x509_bundle.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('https_x509_bundle_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    dut1.start_app()
    num_URLS = dut1.expect(re.compile(r'Connecting to (\d+) URLs'), timeout=30)
    dut1.expect(re.compile(r'Connection established to ([\s\S]*)'), timeout=30)
    dut1.expect('Completed {} connections'.format(num_URLS[0]), timeout=60)

    # test mbedtls dynamic resource
    dut1 = env.get_dut('https_x509_bundle', 'examples/protocols/https_x509_bundle', app_config_name='ssldyn')
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'https_x509_bundle.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('https_x509_bundle_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    dut1.start_app()
    num_URLS = dut1.expect(re.compile(r'Connecting to (\d+) URLs'), timeout=30)
    dut1.expect(re.compile(r'Connection established to ([\s\S]*)'), timeout=30)
    dut1.expect('Completed {} connections'.format(num_URLS[0]), timeout=60)


if __name__ == '__main__':
    test_examples_protocol_https_x509_bundle()
