# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32c6
@pytest.mark.generic
def test_example_tee_secure_storage(dut: Dut) -> None:
    # Start test
    dut.expect('TEE Secure Storage', timeout=30)
    dut.expect('Signature verified successfully', timeout=30)
    dut.expect('Done with encryption/decryption!', timeout=30)
