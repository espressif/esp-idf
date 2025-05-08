# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

CONFIGS = [
    pytest.param('default', marks=[pytest.mark.esp32, pytest.mark.esp32c3]),
    pytest.param('psram_esp32', marks=[pytest.mark.esp32]),
]


@pytest.mark.generic
@idf_parametrize(
    'config,target',
    [('default', 'esp32'), ('default', 'esp32c3'), ('psram_esp32', 'esp32')],
    indirect=['config', 'target'],
)
def test_tcp_transport_client(dut: Dut) -> None:
    dut.expect_unity_test_output()
