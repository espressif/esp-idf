# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32s3', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32h2'], indirect=['target'])
def test_i2s_es7210_tdm_example(dut: Dut) -> None:
    dut.expect_exact('example: Create I2S receive channel')
    dut.expect_exact('example: Configure I2S receive channel to TDM mode')
    dut.expect_exact('example: Init I2C used to configure ES7210')
    dut.expect_exact('example: Configure ES7210 codec parameters')
