# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.generic
def test_i2s_es7210_tdm_example(dut: Dut) -> None:
    dut.expect_exact('example: Create I2S receive channel')
    dut.expect_exact('example: Configure I2S receive channel to TDM mode')
    dut.expect_exact('example: Init I2C used to configure ES7210')
    dut.expect_exact('example: Configure ES7210 codec parameters')
