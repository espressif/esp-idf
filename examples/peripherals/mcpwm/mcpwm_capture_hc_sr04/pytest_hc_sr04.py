# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.generic
def test_hc_sr04_example(dut: Dut) -> None:
    dut.expect_exact('example: Install capture timer')
    dut.expect_exact('example: Install capture channel')
    dut.expect_exact('example: Register capture callback')
    dut.expect_exact('example: Enable capture channel')
    dut.expect_exact('example: Configure Trig pin')
    dut.expect_exact('example: Enable and start capture timer')
