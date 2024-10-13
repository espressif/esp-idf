# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut


@pytest.mark.esp32
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c5
@pytest.mark.generic
def test_i2c_wifi_startup(dut: IdfDut) -> None:
    dut.expect_exact('I2C-WIFI test success')
