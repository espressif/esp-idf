# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded_idf.dut import IdfDut


@pytest.mark.supported_targets
@pytest.mark.temp_skip_ci(targets=['esp32h2', 'esp32p4'], reason='h2/p4 does not support wifi')
@pytest.mark.generic
def test_i2c_wifi_startup(dut: IdfDut) -> None:
    dut.expect_exact('I2C-WIFI test success')
