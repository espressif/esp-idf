# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'target', ['esp32', 'esp32c2', 'esp32c3', 'esp32c6', 'esp32s2', 'esp32s3', 'esp32c5'], indirect=['target']
)
def test_i2c_wifi_startup(dut: IdfDut) -> None:
    dut.expect_exact('I2C-WIFI test success')
