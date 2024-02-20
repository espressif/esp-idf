# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded.dut import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.generic
def test_rotary_encoder(dut: Dut) -> None:
    dut.expect_exact('install pcnt unit')
    dut.expect_exact('set glitch filter')
    dut.expect_exact('install pcnt channels')
    dut.expect_exact('set edge and level actions for pcnt channels')
    dut.expect_exact('add watch points and register callbacks')
    dut.expect_exact('clear pcnt unit')
    dut.expect_exact('start pcnt unit')
    res = dut.expect(r'Pulse count: (\d+)')
    count_val = res.group(1).decode('utf8')
    assert -100 <= int(count_val) <= 100
