# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_uart_async_rxtxtasks_example(dut: Dut) -> None:
    dut.expect_exact('TX_TASK: Wrote 11 bytes')
