# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut


@pytest.mark.parametrize('config', [
    pytest.param('default', marks=[pytest.mark.esp32, pytest.mark.esp32c2, pytest.mark.generic]),
    pytest.param('iram', marks=[pytest.mark.esp32c2, pytest.mark.generic]),
    pytest.param('psram', marks=[pytest.mark.esp32, pytest.mark.psram]),
], indirect=True)
def test_bt_memory_release(dut: Dut) -> None:
    dut.expect_exact('BLE Host Task Started', timeout=6)
    dut.expect_exact('BLE Host Task Stopped', timeout=8)
    dut.expect_exact('Comprehensive heap check: SUCCESS', timeout=10)
