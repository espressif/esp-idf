# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import logging
import os

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_example_tee_basic(dut: Dut) -> None:
    # Logging example binary details
    binary_files = [
        ('tee_basic.bin', '[REE] tee_basic_bin_size'),
        ('esp_tee/esp_tee.bin', '[TEE] tee_basic_bin_size'),
    ]
    for file_name, log_label in binary_files:
        binary_file = os.path.join(dut.app.binary_path, file_name)
        bin_size = os.path.getsize(binary_file)
        logging.info(f'{log_label}: {bin_size // 1024}KB')

    # Start test
    dut.expect('AES-256-GCM operations in TEE', timeout=10)
    dut.expect('Secure service call: PROTECTED M-mode', timeout=10)
    dut.expect('AES-256-GCM encryption', timeout=10)
    dut.expect('Secure service call: PROTECTED M-mode', timeout=10)
    dut.expect('AES-256-GCM decryption', timeout=10)
    dut.expect('Returned from app_main()', timeout=10)
