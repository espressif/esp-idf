# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import logging
import os

import pytest
from common_test_methods import get_env_config_variable
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.ethernet
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
    # Connect to AP
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_ap'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)
    # start test
    num_URLS = int(dut.expect(r'Connecting to (\d+) URLs', timeout=30)[1].decode())
    dut.expect(r'Connection established to ([\s\S]*)', timeout=30)
    dut.expect('Completed {} connections'.format(num_URLS), timeout=60)


@pytest.mark.esp32
@pytest.mark.ethernet
@pytest.mark.parametrize('config', ['ssldyn',], indirect=True)
def test_examples_protocol_https_x509_bundle_dynamic_buffer(dut: Dut) -> None:
    # test mbedtls dynamic resource
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'https_x509_bundle.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('https_x509_bundle_bin_size : {}KB'.format(bin_size // 1024))
    # Connect to AP
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_ap'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)
    # start test
    num_URLS = int(dut.expect(r'Connecting to (\d+) URLs', timeout=30)[1].decode())
    dut.expect(r'Connection established to ([\s\S]*)', timeout=30)
    dut.expect('Completed {} connections'.format(num_URLS), timeout=60)
