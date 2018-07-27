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

import imp
import re
import os
import sys
import string
import random
import socket

# This environment variable is expected on the host machine
test_fw_path = os.getenv("TEST_FW_PATH")
if test_fw_path and test_fw_path not in sys.path:
    sys.path.insert(0, test_fw_path)

# When running on local machine execute the following before running this script
# > export TEST_FW_PATH='~/esp/esp-idf/tools/tiny-test-fw'
# > make print_flash_cmd | tail -n 1 > build/download.config
# > make app bootloader

import TinyFW
import IDF

# Import client module
expath = os.path.dirname(os.path.realpath(__file__))
client = imp.load_source("client", expath + "/scripts/test.py")

@IDF.idf_example_test(env_tag="Example_WIFI", ignore=True)
def test_examples_protocol_http_server_advanced(env, extra_data):
    # Acquire DUT
    dut1 = env.get_dut("http_server", "examples/protocols/http_server/advanced_tests")

    # Get binary file
    binary_file = os.path.join(dut1.app.binary_path, "tests.bin")
    bin_size = os.path.getsize(binary_file)
    IDF.log_performance("http_server_bin_size", "{}KB".format(bin_size//1024))
    IDF.check_performance("http_server_bin_size", bin_size//1024)

    # Upload binary and start testing
    dut1.start_app()

    # Parse IP address of STA
    got_ip = dut1.expect(re.compile(r"(?:[\s\S]*)Got IP: (\d+.\d+.\d+.\d+)"), timeout=30)[0]

    print "Leak Tests..."
    # Expected Leak test Logs
    dut1.expect("Leak Test Started...");
    dut1.expect("Leak Test Passed");

    got_port = dut1.expect(re.compile(r"(?:[\s\S]*)Started HTTP server on port: (\d+)"))[0]
    result = dut1.expect(re.compile(r"(?:[\s\S]*)Max URI handlers: (\d+)(?:[\s\S]*)Max Open Sessions: (\d+)(?:[\s\S]*)Max Header Length: (\d+)(?:[\s\S]*)Max URI Length: (\d+)(?:[\s\S]*)Max Stack Size: (\d+)"))
    max_uri_handlers = int(result[0])
    max_sessions = int(result[1])
    max_hdr_len = int(result[2])
    max_uri_len = int(result[3])
    max_stack_size = int(result[4])

    print "Got IP   : " + got_ip
    print "Got Port : " + got_port

    print "Handler Tests..."
    # Expected Handler Test Logs
    dut1.expect("Test: Register Max URI handlers")
    dut1.expect("Success")
    dut1.expect("Test: Register Max URI + 1 handlers")
    dut1.expect("no slots left for registering handler")
    dut1.expect("Success")
    dut1.expect("Test: Unregister 0th handler")
    dut1.expect("Success")
    dut1.expect("Test: Again unregister 0th handler not registered")
    dut1.expect("handler 0 with method 1 not found")
    dut1.expect("Success")
    dut1.expect("Test: Register back 0th handler")
    dut1.expect("Success")
    dut1.expect("Test: Register 0th handler again after registering")
    dut1.expect("handler 0 with method 1 already registered")
    dut1.expect("Success")
    dut1.expect("Test: Register 1 more handler")
    dut1.expect("no slots left for registering handler")
    dut1.expect("Success")
    dut1.expect("Test: Unregister all handlers")
    dut1.expect("Success")
    dut1.expect("Registering basic handlers")
    dut1.expect("Success")

    # Run test script
    # If failed raise appropriate exception

    print "Basic HTTP Client Tests..."
    if not client.get_hello(got_ip, got_port):
        raise RuntimeError

    inital_stack = int(dut1.expect(re.compile(r"(?:[\s\S]*)Free Stack for server task: (\d+)"))[0])

    if inital_stack < 0.1*max_stack_size:
        print "More than 90% of stack being used on server start"
        raise RuntimeError

    if not client.post_hello(got_ip, got_port):
        raise RuntimeError
    if not client.put_hello(got_ip, got_port):
        raise RuntimeError
    if not client.post_echo(got_ip, got_port):
        raise RuntimeError
    if not client.get_echo(got_ip, got_port):
        raise RuntimeError
    if not client.put_echo(got_ip, got_port):
        raise RuntimeError
    if not client.get_hello_type(got_ip, got_port):
        raise RuntimeError
    if not client.get_hello_status(got_ip, got_port):
        raise RuntimeError
    if not client.get_false_uri(got_ip, got_port):
        raise RuntimeError

    print "Error code tests..."
    if not client.code_500_server_error_test(got_ip, got_port):
        raise RuntimeError
    if not client.code_501_method_not_impl(got_ip, got_port):
        raise RuntimeError
    if not client.code_505_version_not_supported(got_ip, got_port):
        raise RuntimeError
    if not client.code_400_bad_request(got_ip, got_port):
        raise RuntimeError
    if not client.code_404_not_found(got_ip, got_port):
        raise RuntimeError
    if not client.code_405_method_not_allowed(got_ip, got_port):
        raise RuntimeError
    if not client.code_408_req_timeout(got_ip, got_port):
        raise RuntimeError
    if not client.code_414_uri_too_long(got_ip, got_port, max_uri_len):
        raise RuntimeError
    if not client.code_431_hdr_too_long(got_ip, got_port, max_hdr_len):
        raise RuntimeError
    if not client.test_upgrade_not_supported(got_ip, got_port):
        raise RuntimeError

    print "Sessions and Context Tests..."
    if not client.parallel_sessions_adder(got_ip, got_port, max_sessions):
        raise RuntimeError
    if not client.leftover_data_test(got_ip, got_port):
        raise RuntimeError
    if not client.async_response_test(got_ip, got_port):
        raise RuntimeError
    if not client.spillover_session(got_ip, got_port, max_sessions):
        raise RuntimeError
    if not client.recv_timeout_test(got_ip, got_port):
        raise RuntimeError

    # May timeout in case requests are sent slower than responses are read.
    # Instead use httperf stress test
    #if not client.pipeline_test(got_ip, got_port, max_sessions):
    #    raise RuntimeError

    test_size = 50*1024 # 50KB
    if not client.packet_size_limit_test(got_ip, got_port, test_size):
        raise RuntimeError

    if not client.get_hello(got_ip, got_port):
        raise RuntimeError

    final_stack = int(dut1.expect(re.compile(r"(?:[\s\S]*)Free Stack for server task: (\d+)"))[0])

    if final_stack < 0.05*max_stack_size:
        print "More than 95% of stack got used during tests"
        raise RuntimeError

if __name__ == '__main__':
    test_examples_protocol_http_server_advanced()
