# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.generic
def test_i2s_basic_example(dut: Dut) -> None:
    dut.expect_exact('I2S tx and rx channels have been initialized to standard duplex mode', timeout=30)
    dut.expect_exact('I2S tx and rx channels enabled', timeout=30)
    dut.expect_exact('[i2s write] 1440 bytes are written successfully', timeout=30)
    dut.expect_exact('', timeout=30)
    dut.expect_exact('[i2s read] 8192 bytes are read successfully', timeout=30)
