# Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

""" example of writing test with TinyTestFW """
import re
import os
import sys

# if we want to run test case outside `tiny-test-fw` folder,
# we need to insert tiny-test-fw path into sys path
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
    dut1.start_app()
    dut1.expect(re.compile(r"Connecting to www.howsmyssl.com:443"), timeout=30)
    dut1.expect("Performing the SSL/TLS handshake")
    dut1.expect("Certificate verified.", timeout=15)
    dut1.expect_all(re.compile(r"Cipher suite is TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256"),
                    "Reading HTTP response",
                    timeout=20)
    dut1.expect(re.compile(r"Completed (\d) requests"))


if __name__ == '__main__':
    TinyFW.set_default_config(config_file="EnvConfigTemplate.yml", dut=IDF.IDFDUT)
    test_examples_protocol_https_request()
