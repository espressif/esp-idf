# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_crypto(dut: Dut) -> None:
    # if the env variable IDF_FPGA_ENV is set, we would need a longer timeout
    # as tests for efuses burning security peripherals would be run
    timeout = 600 if os.environ.get('IDF_ENV_FPGA') else 60

    test_numbers = dut.expect(r'(\d+) Tests (\d+) Failures (\d+) Ignored', timeout=timeout)
    failures = test_numbers.group(2).decode()
    ignored = test_numbers.group(3).decode()
    assert failures == '0', f'No of failures must be 0 (is {failures})'
    assert ignored == '0', f'No of Ignored test must be 0 (is {ignored})'
    dut.expect('Tests finished', timeout=timeout)


@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize('config', ['long_aes_operations'], indirect=True)
def test_crypto_long_aes_operations(dut: Dut) -> None:
    # if the env variable IDF_FPGA_ENV is set, we would need a longer timeout
    # as tests for efuses burning security peripherals would be run
    timeout = 600 if os.environ.get('IDF_ENV_FPGA') else 60

    dut.expect('Tests finished', timeout=timeout)
