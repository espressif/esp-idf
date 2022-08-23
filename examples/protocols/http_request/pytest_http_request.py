#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import os

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.ethernet
def test_examples_protocol_http_request(dut: Dut) -> None:
    """
    steps: |
      1. join AP
      2. connect to example.com
      3. check conneciton success
    """
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'http_request.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('http_request_bin_size : {}KB'.format(bin_size // 1024))
    # start test
    dut.expect(r'DNS lookup succeeded.', timeout=30)
    # check if connected or not
    dut.expect(' ... connected', timeout=60)
    dut.expect(' ... socket send success')
    dut.expect(' ... set socket receiving timeout success')
    # check server response
    dut.expect(r'HTTP/1.0 200 OK')
    # read from the socket completed
    dut.expect('... done reading from socket. Last read return=0 errno=128')
    dut.expect(r'(\d)...')
