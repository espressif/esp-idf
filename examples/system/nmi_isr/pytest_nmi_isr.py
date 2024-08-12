# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_nmi_isr(dut: Dut) -> None:
    dut.expect_exact('example: Start')
    dut.expect_exact('example: Success')
