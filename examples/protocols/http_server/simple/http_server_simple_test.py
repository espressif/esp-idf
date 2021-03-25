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

from __future__ import division, print_function, unicode_literals

import os
import random
import re
import socket
import string
import threading
import time
from builtins import range

import ttfw_idf
from idf_http_server_test import client
from tiny_test_fw import Utility


class http_client_thread(threading.Thread):
    def __init__(self, ip, port, delay):
        threading.Thread.__init__(self)
        self.ip = ip
        self.port = port
        self.delay = delay
        self.exc = None

    # Thread function used to open a socket and wait for specific amount of time before returning
    def open_connection(self, ip, port, delay):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(delay)
        s.connect((ip, port))
        time.sleep(delay)

    def run(self):
        try:
            self.open_connection(self.ip, self.port, self.delay)
        except socket.timeout as e:
            self.exc = e

    def join(self, timeout=None):
        threading.Thread.join(self)
        if self.exc:
            raise self.exc


# When running on local machine execute the following before running this script
# > make app bootloader
# > make print_flash_cmd | tail -n 1 > build/download.config


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_Protocols')
def test_examples_protocol_http_server_simple(env, extra_data):
    # Acquire DUT
    dut1 = env.get_dut('http_server', 'examples/protocols/http_server/simple', dut_class=ttfw_idf.ESP32DUT)

    # Get binary file
    binary_file = os.path.join(dut1.app.binary_path, 'simple.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('http_server_bin_size', '{}KB'.format(bin_size // 1024))

    # Upload binary and start testing
    Utility.console_log('Starting http_server simple test app')
    dut1.start_app()

    # Parse IP address of STA
    Utility.console_log('Waiting to connect with AP')
    got_ip = dut1.expect(re.compile(r'(?:[\s\S]*)IPv4 address: (\d+.\d+.\d+.\d+)'), timeout=30)[0]
    got_port = dut1.expect(re.compile(r"(?:[\s\S]*)Starting server on port: '(\d+)'"), timeout=30)[0]

    Utility.console_log('Got IP   : ' + got_ip)
    Utility.console_log('Got Port : ' + got_port)

    # Expected Logs
    dut1.expect('Registering URI handlers', timeout=30)

    # Run test script
    # If failed raise appropriate exception
    Utility.console_log('Test /hello GET handler')
    if not client.test_get_handler(got_ip, got_port):
        raise RuntimeError

    # Acquire host IP. Need a way to check it
    dut1.expect(re.compile(r'(?:[\s\S]*)Found header => Host: (\d+.\d+.\d+.\d+)'), timeout=30)[0]

    # Match additional headers sent in the request
    dut1.expect('Found header => Test-Header-2: Test-Value-2', timeout=30)
    dut1.expect('Found header => Test-Header-1: Test-Value-1', timeout=30)
    dut1.expect('Found URL query parameter => query1=value1', timeout=30)
    dut1.expect('Found URL query parameter => query3=value3', timeout=30)
    dut1.expect('Found URL query parameter => query2=value2', timeout=30)
    dut1.expect('Request headers lost', timeout=30)

    Utility.console_log('Test /ctrl PUT handler and realtime handler de/registration')
    if not client.test_put_handler(got_ip, got_port):
        raise RuntimeError
    dut1.expect('Unregistering /hello and /echo URIs', timeout=30)
    dut1.expect('Registering /hello and /echo URIs', timeout=30)

    # Generate random data of 10KB
    random_data = ''.join(string.printable[random.randint(0,len(string.printable)) - 1] for _ in range(10 * 1024))
    Utility.console_log('Test /echo POST handler with random data')
    if not client.test_post_handler(got_ip, got_port, random_data):
        raise RuntimeError

    query = 'http://foobar'
    Utility.console_log('Test /hello with custom query : ' + query)
    if not client.test_custom_uri_query(got_ip, got_port, query):
        raise RuntimeError
    dut1.expect('Found URL query => ' + query, timeout=30)

    query = 'abcd+1234%20xyz'
    Utility.console_log('Test /hello with custom query : ' + query)
    if not client.test_custom_uri_query(got_ip, got_port, query):
        raise RuntimeError
    dut1.expect('Found URL query => ' + query, timeout=30)


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_Protocols')
def test_examples_protocol_http_server_lru_purge_enable(env, extra_data):
    # Acquire DUT
    dut1 = env.get_dut('http_server', 'examples/protocols/http_server/simple', dut_class=ttfw_idf.ESP32DUT)

    # Get binary file
    binary_file = os.path.join(dut1.app.binary_path, 'simple.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('http_server_bin_size', '{}KB'.format(bin_size // 1024))

    # Upload binary and start testing
    Utility.console_log('Starting http_server simple test app')
    dut1.start_app()

    # Parse IP address of STA
    Utility.console_log('Waiting to connect with AP')
    got_ip = dut1.expect(re.compile(r'(?:[\s\S]*)IPv4 address: (\d+.\d+.\d+.\d+)'), timeout=30)[0]
    got_port = dut1.expect(re.compile(r"(?:[\s\S]*)Starting server on port: '(\d+)'"), timeout=30)[0]

    Utility.console_log('Got IP   : ' + got_ip)
    Utility.console_log('Got Port : ' + got_port)

    # Expected Logs
    dut1.expect('Registering URI handlers', timeout=30)
    threads = []
    # Open 20 sockets, one from each thread
    for _ in range(20):
        try:
            thread = http_client_thread(got_ip, (int(got_port)), 20)
            thread.start()
            threads.append(thread)
        except OSError as err:
            Utility.console_log('Error: unable to start thread, ' + err)

    for t in threads:
        t.join()


if __name__ == '__main__':
    test_examples_protocol_http_server_simple()
    test_examples_protocol_http_server_lru_purge_enable()
