import re
import os
import sys

# this is a test case write with tiny-test-fw.
# to run test cases outside tiny-test-fw,
# we need to set environment variable `TEST_FW_PATH`,
# then get and insert `TEST_FW_PATH` to sys path before import FW module
test_fw_path = os.getenv("TEST_FW_PATH")
if test_fw_path and test_fw_path not in sys.path:
    sys.path.insert(0, test_fw_path)

import TinyFW
import IDF


@IDF.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_https_request(env, extra_data):
    """
    steps: |
      1. join AP
      2. connect to www.howsmyssl.com:443
      3. send http request
    """
    dut1 = env.get_dut("https_request", "examples/protocols/https_request")
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "https-request.bin")
    bin_size = os.path.getsize(binary_file)
    IDF.log_performance("https_request_bin_size", "{}KB".format(bin_size//1024))
    IDF.check_performance("https_request_bin_size", bin_size//1024)
    # start test
    dut1.start_app()
    dut1.expect("Connecting to www.howsmyssl.com:443", timeout=30)
    dut1.expect("Performing the SSL/TLS handshake")
    dut1.expect("Certificate verified.", timeout=15)
    dut1.expect("Cipher suite is TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256", timeout=20)
    dut1.expect(re.compile(r"Completed (\d) requests"))


if __name__ == '__main__':
    test_examples_protocol_https_request()
