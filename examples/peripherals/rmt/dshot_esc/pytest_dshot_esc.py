# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'target',
    ['esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4'],
    indirect=['target'],
)
def test_dshot_esc_example(dut: Dut) -> None:
    dut.expect_exact('example: Create RMT TX channel')
    dut.expect_exact('example: Install Dshot ESC encoder')
    dut.expect_exact('example: Enable RMT TX channel')
    dut.expect_exact('example: Start ESC by sending zero throttle for a while...')
    dut.expect_exact('example: Increase throttle, no telemetry')
