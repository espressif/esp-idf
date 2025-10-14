# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import logging
import os

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.ethernet
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_examples_protocol_https_x509_bundle(dut: Dut) -> None:
    """
    steps: |
      1. join AP
      2. connect to multiple URLs
      3. send http request
    """
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'https_x509_bundle.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('https_x509_bundle_bin_size : {}KB'.format(bin_size // 1024))
    dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)
    # start test
    num_URLS = int(dut.expect(r'Connecting to (\d+) URLs', timeout=30)[1].decode())
    for _ in range(num_URLS):
        dut.expect(r'Connection established to ([\s\S]*)', timeout=30)
    dut.expect('Completed {} connections'.format(num_URLS), timeout=60)


@pytest.mark.ethernet
@pytest.mark.parametrize(
    'config',
    [
        'ssldyn',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_examples_protocol_https_x509_bundle_dynamic_buffer(dut: Dut) -> None:
    # test mbedtls dynamic resource
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'https_x509_bundle.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('https_x509_bundle_bin_size : {}KB'.format(bin_size // 1024))
    dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)
    # start test
    num_URLS = int(dut.expect(r'Connecting to (\d+) URLs', timeout=30)[1].decode())
    dut.expect(r'Connection established to ([\s\S]*)', timeout=30)
    dut.expect('Completed {} connections'.format(num_URLS), timeout=60)


@pytest.mark.qemu
@pytest.mark.host_test
@pytest.mark.parametrize(
    'config',
    [
        'default_crt_bundle',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_examples_protocol_https_x509_bundle_default_crt_bundle_stress_test(dut: Dut) -> None:
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'https_x509_bundle.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('https_x509_bundle_bin_size : {}KB'.format(bin_size // 1024))
    dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)
    # start test
    num_URLS = int(dut.expect(r'Connecting to (\d+) URLs', timeout=30)[1].decode())
    for _ in range(num_URLS):
        dut.expect(r'Connection established to ([\s\S]*)', timeout=30)
    dut.expect('Completed {} connections'.format(num_URLS), timeout=60)
