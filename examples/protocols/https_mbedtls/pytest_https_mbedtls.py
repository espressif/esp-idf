#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import os

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.ethernet
def test_examples_protocol_https_mbedtls(dut: Dut) -> None:
    """
    steps: |
      1. join AP
      2. connect to www.howsmyssl.com:443
      3. send http request
    """
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'https_mbedtls.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('https_mbedtls_bin_size : {}KB'.format(bin_size // 1024))
    # start test
    dut.expect('Connected.', timeout=30)
    logging.info('TCP connection established with the server\n performing SSL/TLS handshake')
    dut.expect('Performing the SSL/TLS handshake...')
    dut.expect('Certificate verified.')
    logging.info('SSL/TLS handshake successful')
    dut.expect('Writing HTTP request...')
    dut.expect('Reading HTTP response...')
    dut.expect(r'Completed (\d) requests')

    # Read free heap size
    res = dut.expect(r'Minimum free heap size: (\d+)')[1].decode()
    if not res:
        raise ValueError('Maximum heap size info not found')
