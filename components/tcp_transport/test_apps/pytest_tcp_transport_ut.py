# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut

CONFIGS = [
    pytest.param('default', marks=[pytest.mark.esp32, pytest.mark.esp32c3]),
    pytest.param('psram_esp32', marks=[pytest.mark.esp32]),
]


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
def test_tcp_transport_client(dut: Dut) -> None:
    dut.expect_unity_test_output()
