# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_uart_events_example(dut: Dut) -> None:
    dut.expect_exact('Returned from app_main()')
    dut.write('a')
    dut.expect_exact('uart_events: [UART DATA]: 2')  # dut.write will add an extra '\n'
    dut.write('HA')
    dut.expect_exact('uart_events: [UART DATA]: 3')  # dut.write will add an extra '\n'
