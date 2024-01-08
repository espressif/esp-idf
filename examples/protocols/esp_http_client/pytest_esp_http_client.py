# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import logging
import os

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.httpbin
def test_examples_protocol_esp_http_client(dut: Dut) -> None:
    """
    steps: |
      1. join AP/Ethernet
      2. Send HTTP request to httpbin.org
    """
    binary_file = os.path.join(dut.app.binary_path, 'esp_http_client_example.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('esp_http_client_bin_size : {}KB'.format(bin_size // 1024))
    # start test
    dut.expect('Connected to AP, begin http example', timeout=30)
    dut.expect(r'HTTP GET Status = 200, content_length = (\d)')
    dut.expect(r'HTTP POST Status = 200, content_length = (\d)')
    dut.expect(r'HTTP PUT Status = 200, content_length = (\d)')
    dut.expect(r'HTTP PATCH Status = 200, content_length = (\d)')
    dut.expect(r'HTTP DELETE Status = 200, content_length = (\d)')
    dut.expect(r'HTTP HEAD Status = 200, content_length = (\d)')
    dut.expect(r'HTTP GET Status = 200, content_length = (\d)')
    dut.expect(r'HTTP POST Status = 200, content_length = (\d)')
    dut.expect(r'HTTP PUT Status = 200, content_length = (\d)')
    dut.expect(r'HTTP PATCH Status = 200, content_length = (\d)')
    dut.expect(r'HTTP DELETE Status = 200, content_length = (\d)')
    dut.expect(r'HTTP HEAD Status = 200, content_length = (\d)')
    dut.expect(r'HTTP Basic Auth Status = 200, content_length = (\d)')
    dut.expect(r'HTTP Basic Auth redirect Status = 200, content_length = (\d)')
    dut.expect(r'HTTP MD5 Digest Auth Status = 200, content_length = (\d)')
    dut.expect(r'HTTP SHA256 Digest Auth Status = 200, content_length = (\d)')
    dut.expect(r'HTTP Relative path redirect Status = 200, content_length = (\d)')
    dut.expect(r'HTTP Absolute path redirect Status = 200, content_length = (\d)')
    dut.expect(r'HTTP Absolute path redirect \(manual\) Status = 200, content_length = (\d)')
    dut.expect(r'HTTPS Status = 200, content_length = (\d)')
    dut.expect(r'HTTPS Status = 200, content_length = (\d)')
    dut.expect(r'HTTP redirect to HTTPS Status = 200, content_length = (\d)')
    dut.expect(r'HTTP chunk encoding Status = 200, content_length = (-?\d)')
    # content-len for chunked encoding is typically -1, could be a positive length in some cases
    dut.expect(r'HTTP Stream reader Status = 200, content_length = (\d)')
    dut.expect(r'HTTPS Status = 200, content_length = (\d)')
    dut.expect(r'HTTPS Status = 200, content_length = (\d)')
    dut.expect(r'Last esp error code: 0x8001')
    dut.expect(r'HTTP GET Status = 200, content_length = (\d)')
    dut.expect(r'HTTP POST Status = 200, content_length = (\d)')
    dut.expect(r'HTTP Status = 206, content_length = (\d)')
    dut.expect(r'HTTP Status = 206, content_length = 10')
    dut.expect(r'HTTP Status = 206, content_length = 10')
    dut.expect('Finish http example')


@pytest.mark.esp32
@pytest.mark.httpbin
@pytest.mark.parametrize('config', [
    'ssldyn',
], indirect=True)
def test_examples_protocol_esp_http_client_dynamic_buffer(dut: Dut) -> None:
    # test mbedtls dynamic resource
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'esp_http_client_example.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('esp_http_client_bin_size : {}KB'.format(bin_size // 1024))

    dut.expect('Connected to AP, begin http example', timeout=30)
    dut.expect(r'HTTP GET Status = 200, content_length = (\d)')
    dut.expect(r'HTTP POST Status = 200, content_length = (\d)')
    dut.expect(r'HTTP PUT Status = 200, content_length = (\d)')
    dut.expect(r'HTTP PATCH Status = 200, content_length = (\d)')
    dut.expect(r'HTTP DELETE Status = 200, content_length = (\d)')
    dut.expect(r'HTTP HEAD Status = 200, content_length = (\d)')
    dut.expect(r'HTTP GET Status = 200, content_length = (\d)')
    dut.expect(r'HTTP POST Status = 200, content_length = (\d)')
    dut.expect(r'HTTP PUT Status = 200, content_length = (\d)')
    dut.expect(r'HTTP PATCH Status = 200, content_length = (\d)')
    dut.expect(r'HTTP DELETE Status = 200, content_length = (\d)')
    dut.expect(r'HTTP HEAD Status = 200, content_length = (\d)')
    dut.expect(r'HTTP Basic Auth Status = 200, content_length = (\d)')
    dut.expect(r'HTTP Basic Auth redirect Status = 200, content_length = (\d)')
    dut.expect(r'HTTP Relative path redirect Status = 200, content_length = (\d)')
    dut.expect(r'HTTP Absolute path redirect Status = 200, content_length = (\d)')
    dut.expect(r'HTTP Absolute path redirect \(manual\) Status = 200, content_length = (\d)')
    dut.expect(r'HTTPS Status = 200, content_length = (\d)')
    dut.expect(r'HTTPS Status = 200, content_length = (\d)')
    dut.expect(r'HTTP redirect to HTTPS Status = 200, content_length = (\d)')
    dut.expect(r'HTTP chunk encoding Status = 200, content_length = (-?\d)')
    # content-len for chunked encoding is typically -1, could be a positive length in some cases
    dut.expect(r'HTTP Stream reader Status = 200, content_length = (\d)')
    dut.expect(r'HTTPS Status = 200, content_length = (\d)')
    dut.expect(r'HTTPS Status = 200, content_length = (\d)')
    dut.expect(r'Last esp error code: 0x8001')
    dut.expect(r'HTTP GET Status = 200, content_length = (\d)')
    dut.expect(r'HTTP POST Status = 200, content_length = (\d)')
    dut.expect(r'HTTP Status = 206, content_length = (\d)')
    dut.expect(r'HTTP Status = 206, content_length = 10')
    dut.expect(r'HTTP Status = 206, content_length = 10')
    dut.expect('Finish http example')


@pytest.mark.linux
@pytest.mark.host_test
# Currently we are just testing the build for esp_http_client on Linux target. So skipping the test run.
# Later we will enable the test run for Linux target as well.
@pytest.mark.skipif('config.getvalue("target") == "linux"', reason='Do not run on Linux')
@pytest.mark.parametrize('config', [
    'default', 'ssldyn',
], indirect=True)
def test_examples_protocol_esp_http_client_linux(dut: Dut) -> None:
    dut.expect('Finish http example', timeout=60)
