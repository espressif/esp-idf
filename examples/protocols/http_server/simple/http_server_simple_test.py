#!/usr/bin/env python
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

from __future__ import division
from __future__ import print_function
from __future__ import unicode_literals
from builtins import range
import re
import os
import sys
import string
import random

try:
    import IDF
except ImportError:
    # This environment variable is expected on the host machine
    test_fw_path = os.getenv("TEST_FW_PATH")
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)

    import IDF

import Utility

# When running on local machine execute the following before running this script
# > make app bootloader
# > make print_flash_cmd | tail -n 1 > build/download.config
# > export TEST_FW_PATH=~/esp/esp-idf/tools/tiny-test-fw

# Import client module
expath = os.path.dirname(os.path.realpath(__file__))
client = Utility.load_source("client", expath + "/scripts/client.py")


@IDF.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_http_server_simple(env, extra_data):
    # Acquire DUT
    dut1 = env.get_dut("http_server", "examples/protocols/http_server/simple")

    # Get binary file
    binary_file = os.path.join(dut1.app.binary_path, "simple.bin")
    bin_size = os.path.getsize(binary_file)
    IDF.log_performance("http_server_bin_size", "{}KB".format(bin_size // 1024))
    IDF.check_performance("http_server_bin_size", bin_size // 1024)

    # Upload binary and start testing
    Utility.console_log("Starting http_server simple test app")
    dut1.start_app()

    # Parse IP address of STA
    Utility.console_log("Waiting to connect with AP")
    got_ip   = dut1.expect(re.compile(r"(?:[\s\S]*)Got IP: '(\d+.\d+.\d+.\d+)'"), timeout=120)[0]
    got_port = dut1.expect(re.compile(r"(?:[\s\S]*)Starting server on port: '(\d+)'"), timeout=30)[0]

    Utility.console_log("Got IP   : " + got_ip)
    Utility.console_log("Got Port : " + got_port)

    # Expected Logs
    dut1.expect("Registering URI handlers", timeout=30)

    # Run test script
    # If failed raise appropriate exception
    Utility.console_log("Test /hello GET handler")
    if not client.test_get_handler(got_ip, got_port):
        raise RuntimeError

    # Acquire host IP. Need a way to check it
    dut1.expect(re.compile(r"(?:[\s\S]*)Found header => Host: (\d+.\d+.\d+.\d+)"), timeout=30)[0]

    # Match additional headers sent in the request
    dut1.expect("Found header => Test-Header-2: Test-Value-2", timeout=30)
    dut1.expect("Found header => Test-Header-1: Test-Value-1", timeout=30)
    dut1.expect("Found URL query parameter => query1=value1", timeout=30)
    dut1.expect("Found URL query parameter => query3=value3", timeout=30)
    dut1.expect("Found URL query parameter => query2=value2", timeout=30)
    dut1.expect("Request headers lost", timeout=30)

    Utility.console_log("Test /ctrl PUT handler and realtime handler de/registration")
    if not client.test_put_handler(got_ip, got_port):
        raise RuntimeError
    dut1.expect("Unregistering /hello and /echo URIs", timeout=30)
    dut1.expect("Registering /hello and /echo URIs", timeout=30)

    # Generate random data of 10KB
    random_data = ''.join(string.printable[random.randint(0,len(string.printable)) - 1] for _ in range(10 * 1024))
    Utility.console_log("Test /echo POST handler with random data")
    if not client.test_post_handler(got_ip, got_port, random_data):
        raise RuntimeError

    query = "http://foobar"
    Utility.console_log("Test /hello with custom query : " + query)
    if not client.test_custom_uri_query(got_ip, got_port, query):
        raise RuntimeError
    dut1.expect("Found URL query => " + query, timeout=30)

    query = "abcd+1234%20xyz"
    Utility.console_log("Test /hello with custom query : " + query)
    if not client.test_custom_uri_query(got_ip, got_port, query):
        raise RuntimeError
    dut1.expect("Found URL query => " + query, timeout=30)

    query = "abcd\nyz"
    Utility.console_log("Test /hello with invalid query")
    if client.test_custom_uri_query(got_ip, got_port, query):
        raise RuntimeError
    dut1.expect("400 Bad Request - Server unable to understand request due to invalid syntax", timeout=30)


if __name__ == '__main__':
    test_examples_protocol_http_server_simple()
