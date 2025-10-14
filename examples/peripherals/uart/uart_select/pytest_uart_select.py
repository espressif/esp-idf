# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_uart_select_example(dut: Dut) -> None:
    dut.expect_exact('uart_select_example: Timeout has been reached and nothing has been received')
    dut.write('a')
    dut.expect_exact('uart_select_example: Received: a')
