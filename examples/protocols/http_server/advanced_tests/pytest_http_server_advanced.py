#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import division, print_function, unicode_literals

import logging
import os
import sys

import pytest

try:
    from idf_http_server_test import test as client
except ModuleNotFoundError:
    sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', 'tools', 'ci', 'python_packages'))
    from idf_http_server_test import test as client

from common_test_methods import get_env_config_variable
from pytest_embedded import Dut

# When running on local machine execute the following before running this script
# > make app bootloader
# > make print_flash_cmd | tail -n 1 > build/download.config

# Due to connectivity issues (between runner host and DUT) in the runner environment,
# some of the `advanced_tests` are ignored. These tests are intended for verifying
# the expected limits of the http_server capabilities, and implement sending and receiving
# of large HTTP packets and malformed requests, running multiple parallel sessions, etc.
# It is advised that all these tests be run locally, when making changes or adding new
# features to this component.


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.wifi_router
def test_examples_protocol_http_server_advanced(dut: Dut) -> None:

    # Get binary file
    binary_file = os.path.join(dut.app.binary_path, 'tests.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('http_server_bin_size : {}KB'.format(bin_size // 1024))

    logging.info('Starting http_server advanced test app')

    # Parse IP address of STA
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()

    got_port = dut.expect(r"(?:[\s\S]*)Started HTTP server on port: '(\d+)'", timeout=30)[1].decode()

    result = dut.expect(r"(?:[\s\S]*)Max URI handlers: '(\d+)'(?:[\s\S]*)Max Open Sessions: "  # noqa: W605
                        r"'(\d+)'(?:[\s\S]*)Max Header Length: '(\d+)'(?:[\s\S]*)Max URI Length: "
                        r"'(\d+)'(?:[\s\S]*)Max Stack Size: '(\d+)'", timeout=15)
    # max_uri_handlers = int(result[1])
    max_sessions = int(result[2])
    max_hdr_len = int(result[3])
    max_uri_len = int(result[4])
    max_stack_size = int(result[5])

    logging.info('Got Port : {}'.format(got_port))
    logging.info('Got IP   : {}'.format(got_ip))

    # Run test script
    # If failed raise appropriate exception
    failed = False

    logging.info('Sessions and Context Tests...')
    if not client.spillover_session(got_ip, got_port, max_sessions):
        logging.info('Ignoring failure')
    if not client.parallel_sessions_adder(got_ip, got_port, max_sessions):
        logging.info('Ignoring failure')
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
    #    logging.info("Ignoring failure")

    logging.info('Getting initial stack usage...')
    if not client.get_hello(got_ip, got_port):
        failed = True

    inital_stack = int(dut.expect(r"(?:[\s\S]*)Free Stack for server task: '(\d+)'", timeout=15)[1])

    if inital_stack < 0.1 * max_stack_size:
        logging.info('More than 90% of stack being used on server start')
        failed = True

    logging.info('Basic HTTP Client Tests...')
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

    logging.info('Error code tests...')
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
        logging.info('Ignoring failure')
    if not client.code_431_hdr_too_long(got_ip, got_port, max_hdr_len):
        logging.info('Ignoring failure')
    if not client.test_upgrade_not_supported(got_ip, got_port):
        failed = True

    logging.info('Getting final stack usage...')
    if not client.get_hello(got_ip, got_port):
        failed = True

    final_stack = int(dut.expect(r"(?:[\s\S]*)Free Stack for server task: '(\d+)'", timeout=15)[1])

    if final_stack < 0.05 * max_stack_size:
        logging.info('More than 95% of stack got used during tests')
        failed = True

    if failed:
        raise RuntimeError
