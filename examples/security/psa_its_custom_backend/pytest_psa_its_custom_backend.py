# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
"""Pytest for PSA ITS custom storage backend example."""

import logging
import os

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_psa_its_custom_backend(dut: Dut) -> None:
    binary_file = os.path.join(dut.app.binary_path, 'psa_its_custom_backend.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('psa_its_custom_backend_bin_size: %dKB', bin_size // 1024)

    dut.expect(r'example: === PSA ITS Custom Storage Backend ===', timeout=60)
    dut.expect(r'example: Custom ITS backend registered', timeout=60)

    dut.expect(r'example: --- Provisioning keys ---', timeout=60)

    # Default NVS-backed key
    match = dut.expect(
        r'example: \[NVS\] Generated persistent key 0x1|example: \[NVS\] Key 0x1 already exists',
        timeout=60,
    )
    if b'Generated' in match.group(0):
        logging.info('NVS key generated for the first time')
    else:
        logging.info('NVS key already existed in storage')

    # Custom-backend key
    match = dut.expect(
        r'example: \[CUSTOM\] Generated persistent key 0x30000001|'
        r'example: \[CUSTOM\] Key 0x30000001 already exists',
        timeout=60,
    )
    if b'Generated' in match.group(0):
        logging.info('Custom-backend key generated for the first time')
    else:
        logging.info('Custom-backend key already existed in storage')

    dut.expect(r'example: --- Testing encryption/decryption ---', timeout=60)
    dut.expect(r'example: \[NVS\] Encrypt/decrypt OK with key 0x1', timeout=60)
    dut.expect(r'example: \[CUSTOM\] Encrypt/decrypt OK with key 0x30000001', timeout=60)

    dut.expect(r'example: === All tests passed ===', timeout=60)
