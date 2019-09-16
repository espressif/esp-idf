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
import re
import os
import sys

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
client = Utility.load_source("client", expath + "/scripts/test.py")


# Due to connectivity issues (between runner host and DUT) in the runner environment,
# some of the `advanced_tests` are ignored. These tests are intended for verifying
# the expected limits of the http_server capabilities, and implement sending and receiving
# of large HTTP packets and malformed requests, running multiple parallel sessions, etc.
# It is advised that all these tests be run locally, when making changes or adding new
# features to this component.
@IDF.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_http_server_advanced(env, extra_data):
    # Acquire DUT
    dut1 = env.get_dut("http_server", "examples/protocols/http_server/advanced_tests", dut_class=ESP32DUT)

    # Get binary file
    binary_file = os.path.join(dut1.app.binary_path, "tests.bin")
    bin_size = os.path.getsize(binary_file)
    IDF.log_performance("http_server_bin_size", "{}KB".format(bin_size // 1024))
    IDF.check_performance("http_server_bin_size", bin_size // 1024)

    # Upload binary and start testing
    Utility.console_log("Starting http_server advanced test app")
    dut1.start_app()

    # Parse IP address of STA
    Utility.console_log("Waiting to connect with AP")
    got_ip = dut1.expect(re.compile(r"(?:[\s\S]*)IPv4 address: (\d+.\d+.\d+.\d+)"), timeout=30)[0]

    got_port = dut1.expect(re.compile(r"(?:[\s\S]*)Started HTTP server on port: '(\d+)'"), timeout=15)[0]
    result = dut1.expect(re.compile(r"(?:[\s\S]*)Max URI handlers: '(\d+)'(?:[\s\S]*)Max Open Sessions: "  # noqa: W605
                                    r"'(\d+)'(?:[\s\S]*)Max Header Length: '(\d+)'(?:[\s\S]*)Max URI Length: "
                                    r"'(\d+)'(?:[\s\S]*)Max Stack Size: '(\d+)'"), timeout=15)
    # max_uri_handlers = int(result[0])
    max_sessions = int(result[1])
    max_hdr_len = int(result[2])
    max_uri_len = int(result[3])
    max_stack_size = int(result[4])

    Utility.console_log("Got IP   : " + got_ip)
    Utility.console_log("Got Port : " + got_port)

    # Run test script
    # If failed raise appropriate exception
    failed = False

    Utility.console_log("Sessions and Context Tests...")
    if not client.spillover_session(got_ip, got_port, max_sessions):
        Utility.console_log("Ignoring failure")
    if not client.parallel_sessions_adder(got_ip, got_port, max_sessions):
        Utility.console_log("Ignoring failure")
    if not client.leftover_data_test(got_ip, got_port):
        failed = True
    if not client.async_response_test(got_ip, got_port):
        failed = True
    if not client.recv_timeout_test(got_ip, got_port):
        failed = True
    if not client.arbitrary_termination_test(got_ip, got_port):
        failed = True

    # This test fails a lot! Enable when connection is stable
    # test_size = 50*1024 # 50KB
    # if not client.packet_size_limit_test(got_ip, got_port, test_size):
    #    Utility.console_log("Ignoring failure")

    Utility.console_log("Getting initial stack usage...")
    if not client.get_hello(got_ip, got_port):
        failed = True

    inital_stack = int(dut1.expect(re.compile(r"(?:[\s\S]*)Free Stack for server task: '(\d+)'"), timeout=15)[0])

    if inital_stack < 0.1 * max_stack_size:
        Utility.console_log("More than 90% of stack being used on server start")
        failed = True

    Utility.console_log("Basic HTTP Client Tests...")
    if not client.get_hello(got_ip, got_port):
        failed = True
    if not client.post_hello(got_ip, got_port):
        failed = True
    if not client.put_hello(got_ip, got_port):
        failed = True
    if not client.post_echo(got_ip, got_port):
        failed = True
    if not client.get_echo(got_ip, got_port):
        failed = True
    if not client.put_echo(got_ip, got_port):
        failed = True
    if not client.get_hello_type(got_ip, got_port):
        failed = True
    if not client.get_hello_status(got_ip, got_port):
        failed = True
    if not client.get_false_uri(got_ip, got_port):
        failed = True
    if not client.get_test_headers(got_ip, got_port):
        failed = True

    Utility.console_log("Error code tests...")
    if not client.code_500_server_error_test(got_ip, got_port):
        failed = True
    if not client.code_501_method_not_impl(got_ip, got_port):
        failed = True
    if not client.code_505_version_not_supported(got_ip, got_port):
        failed = True
    if not client.code_400_bad_request(got_ip, got_port):
        failed = True
    if not client.code_404_not_found(got_ip, got_port):
        failed = True
    if not client.code_405_method_not_allowed(got_ip, got_port):
        failed = True
    if not client.code_408_req_timeout(got_ip, got_port):
        failed = True
    if not client.code_414_uri_too_long(got_ip, got_port, max_uri_len):
        Utility.console_log("Ignoring failure")
    if not client.code_431_hdr_too_long(got_ip, got_port, max_hdr_len):
        Utility.console_log("Ignoring failure")
    if not client.test_upgrade_not_supported(got_ip, got_port):
        failed = True

    Utility.console_log("Getting final stack usage...")
    if not client.get_hello(got_ip, got_port):
        failed = True

    final_stack = int(dut1.expect(re.compile(r"(?:[\s\S]*)Free Stack for server task: '(\d+)'"), timeout=15)[0])

    if final_stack < 0.05 * max_stack_size:
        Utility.console_log("More than 95% of stack got used during tests")
        failed = True

    if failed:
        raise RuntimeError


if __name__ == '__main__':
    test_examples_protocol_http_server_advanced()
