# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.ir_transceiver
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_ir_nec_example(dut: Dut) -> None:
    dut.expect_exact('example: create RMT RX channel')
    dut.expect_exact('example: register RX done callback')
    dut.expect_exact('example: create RMT TX channel')
    dut.expect_exact('example: modulate carrier to TX channel')
    dut.expect_exact('example: install IR NEC encoder')
    dut.expect_exact('example: enable RMT TX and RX channels')
    dut.expect_exact('NEC frame start---')
    dut.expect_exact('---NEC frame end: Address=0440, Command=3003')
    dut.expect_exact('---NEC frame end: Address=0440, Command=3003')
