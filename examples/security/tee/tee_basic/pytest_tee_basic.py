# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import logging
import os

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32c6'], indirect=['target'])
def test_example_tee_basic(dut: Dut) -> None:
    # Logging example binary details
    binary_files = [
        ('tee_basic.bin', '[REE] tee_basic_bin_size'),
        ('esp_tee/esp_tee.bin', '[TEE] tee_basic_bin_size'),
    ]
    for file_name, log_label in binary_files:
        binary_file = os.path.join(dut.app.binary_path, file_name)
        bin_size = os.path.getsize(binary_file)
        logging.info('{}: {}KB'.format(log_label, bin_size // 1024))

    # Start test
    dut.expect('AES-256-CBC operations in TEE', timeout=30)
    dut.expect('TEE: In PROTECTED M-mode', timeout=30)
    dut.expect('AES encryption successful!', timeout=30)
    dut.expect('ee 04 9b ee 95 6f 25 04 1e 8c e4 4e 8e 4e 7a d3', timeout=30)
    dut.expect('AES decryption successful!', timeout=30)
