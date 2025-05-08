# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32c6'], indirect=['target'])
def test_example_tee_secure_storage(dut: Dut) -> None:
    # Start test
    dut.expect('TEE Secure Storage', timeout=30)
    dut.expect('Signature verified successfully', timeout=30)
    dut.expect('Done with encryption/decryption!', timeout=30)
