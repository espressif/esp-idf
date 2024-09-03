# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32c5
@pytest.mark.esp32c6
@pytest.mark.esp32c61
@pytest.mark.esp32h2
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.generic
def test_gptimer_wiegand(dut: Dut) -> None:
    dut.expect_exact('example: Configure wiegand interface')
    dut.expect_exact('example: send wiegand data')
