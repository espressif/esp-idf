# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
"""
Pytest for Key Manager Signing Example

This test verifies ECDSA and RSA-DS signing operations using keys deployed
via the Key Manager peripheral.
"""

import logging
import os

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32c5'], indirect=['target'])
def test_key_manager(dut: Dut) -> None:
    # Log binary size
    binary_file = os.path.join(dut.app.binary_path, 'key_manager.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info(f'key_manager_bin_size: {bin_size // 1024}KB')

    # Verify example starts
    dut.expect('Key Manager Signing Example', timeout=30)

    # Test ECDSA signing flow
    dut.expect('ECDSA Signing with Key Manager', timeout=30)
    match = dut.expect(
        r'No ECDSA key in NVS, deploying new key via Key Manager|Using existing ECDSA key from NVS', timeout=30
    )
    if b'No ECDSA key' in match.group(0):
        dut.expect('Key deployed successfully in Key Manager', timeout=30)
    dut.expect('Step 2: Creating PSA opaque key reference', timeout=30)
    dut.expect('Step 3: Signing hash using Key Manager key', timeout=30)
    dut.expect(r'Signature generated successfully \(\d+ bytes\)', timeout=30)
    dut.expect('Step 4: Verifying signature with known public key', timeout=30)
    dut.expect('ECDSA signature verification PASSED!', timeout=30)
    dut.expect('ECDSA Signing Example Complete', timeout=30)

    # Test RSA-DS signing flow
    dut.expect('RSA-DS Signing with Key Manager', timeout=30)
    match = dut.expect(
        r'No DS key in NVS, deploying new key via Key Manager|Using existing DS key from NVS', timeout=30
    )
    if b'No DS key' in match.group(0):
        dut.expect('Key deployed successfully in Key Manager', timeout=30)
    dut.expect('Step 2: Preparing DS data context', timeout=30)
    dut.expect('Step 3: Creating PSA opaque key reference for RSA-DS', timeout=30)
    dut.expect('Step 4: Signing hash using RSA-DS with Key Manager key', timeout=30)
    dut.expect(r'RSA-DS signature generated successfully \(\d+ bytes\)', timeout=30)
    dut.expect('RSA-DS Signing Example Complete', timeout=30)

    # Verify example completes
    dut.expect('Key Manager Signing Example finished.', timeout=30)
