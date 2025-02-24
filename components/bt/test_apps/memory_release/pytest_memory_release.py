# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@idf_parametrize(
    'config,target,markers',
    [
        ('default', 'esp32', (pytest.mark.generic,)),
        ('default', 'esp32c2', (pytest.mark.generic,)),
        ('iram', 'esp32c2', (pytest.mark.generic,)),
        ('psram', 'esp32', (pytest.mark.psram,)),
    ],
    indirect=['config', 'target'],
)
def test_bt_memory_release(dut: Dut) -> None:
    dut.expect_exact('BLE Host Task Started', timeout=6)
    dut.expect_exact('BLE Host Task Stopped', timeout=8)
    dut.expect_exact('Comprehensive heap check: SUCCESS', timeout=10)
