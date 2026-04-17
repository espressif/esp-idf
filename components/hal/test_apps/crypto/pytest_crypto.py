# SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_crypto(dut: Dut) -> None:
    # if the env variable IDF_FPGA_ENV is set, we would need a longer timeout
    # as tests for efuses burning security peripherals would be run
    timeout = 600 if os.environ.get('IDF_ENV_FPGA') else 60

    dut.expect('Tests finished', timeout=timeout)


@pytest.mark.generic
@pytest.mark.parametrize('config', ['long_aes_operations'], indirect=True)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_crypto_long_aes_operations(dut: Dut) -> None:
    # if the env variable IDF_FPGA_ENV is set, we would need a longer timeout
    # as tests for efuses burning security peripherals would be run
    timeout = 600 if os.environ.get('IDF_ENV_FPGA') else 60

    dut.expect('Tests finished', timeout=timeout)
