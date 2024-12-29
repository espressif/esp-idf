# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.qemu
@pytest.mark.host_test
@pytest.mark.esp32
@pytest.mark.esp32c3
def test_std_filesystem(dut: Dut) -> None:
    dut.expect_exact('All tests passed', timeout=200)
