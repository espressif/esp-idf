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
from builtins import str
from builtins import range
import re
import os
import sys
import random

try:
    import IDF
    from IDF.IDFDUT import ESP32DUT
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
client = Utility.load_source("client", expath + "/scripts/adder.py")


@IDF.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_http_server_persistence(env, extra_data):
    # Acquire DUT
    dut1 = env.get_dut("http_server", "examples/protocols/http_server/persistent_sockets", dut_class=ESP32DUT)

    # Get binary file
    binary_file = os.path.join(dut1.app.binary_path, "persistent_sockets.bin")
    bin_size = os.path.getsize(binary_file)
    IDF.log_performance("http_server_bin_size", "{}KB".format(bin_size // 1024))
    IDF.check_performance("http_server_bin_size", bin_size // 1024)

    # Upload binary and start testing
    Utility.console_log("Starting http_server persistance test app")
    dut1.start_app()

    # Parse IP address of STA
    Utility.console_log("Waiting to connect with AP")
    got_ip = dut1.expect(re.compile(r"(?:[\s\S]*)IPv4 address: (\d+.\d+.\d+.\d+)"), timeout=30)[0]
    got_port = dut1.expect(re.compile(r"(?:[\s\S]*)Starting server on port: '(\d+)'"), timeout=30)[0]

    Utility.console_log("Got IP   : " + got_ip)
    Utility.console_log("Got Port : " + got_port)

    # Expected Logs
    dut1.expect("Registering URI handlers", timeout=30)

    # Run test script
    conn = client.start_session(got_ip, got_port)
    visitor = 0
    adder = 0

    # Test PUT request and initialize session context
    num = random.randint(0,100)
    client.putreq(conn, "/adder", str(num))
    visitor += 1
    dut1.expect("/adder visitor count = " + str(visitor), timeout=30)
    dut1.expect("/adder PUT handler read " + str(num), timeout=30)
    dut1.expect("PUT allocating new session", timeout=30)

    # Retest PUT request and change session context value
    num = random.randint(0,100)
    Utility.console_log("Adding: " + str(num))
    client.putreq(conn, "/adder", str(num))
    visitor += 1
    adder += num
    dut1.expect("/adder visitor count = " + str(visitor), timeout=30)
    dut1.expect("/adder PUT handler read " + str(num), timeout=30)
    try:
        # Re allocation shouldn't happen
        dut1.expect("PUT allocating new session", timeout=30)
        # Not expected
        raise RuntimeError
    except Exception:
        # As expected
        pass

    # Test POST request and session persistence
    random_nums = [random.randint(0,100) for _ in range(100)]
    for num in random_nums:
        Utility.console_log("Adding: " + str(num))
        client.postreq(conn, "/adder", str(num))
        visitor += 1
        adder += num
        dut1.expect("/adder visitor count = " + str(visitor), timeout=30)
        dut1.expect("/adder handler read " + str(num), timeout=30)

    # Test GET request and session persistence
    Utility.console_log("Matching final sum: " + str(adder))
    if client.getreq(conn, "/adder").decode() != str(adder):
        raise RuntimeError
    visitor += 1
    dut1.expect("/adder visitor count = " + str(visitor), timeout=30)
    dut1.expect("/adder GET handler send " + str(adder), timeout=30)

    Utility.console_log("Ending session")
    # Close connection and check for invocation of context "Free" function
    client.end_session(conn)
    dut1.expect("/adder Free Context function called", timeout=30)

    Utility.console_log("Validating user context data")
    # Start another session to check user context data
    client.start_session(got_ip, got_port)
    num = random.randint(0,100)
    client.putreq(conn, "/adder", str(num))
    visitor += 1
    dut1.expect("/adder visitor count = " + str(visitor), timeout=30)
    dut1.expect("/adder PUT handler read " + str(num), timeout=30)
    dut1.expect("PUT allocating new session", timeout=30)
    client.end_session(conn)
    dut1.expect("/adder Free Context function called", timeout=30)


if __name__ == '__main__':
    test_examples_protocol_http_server_persistence()
