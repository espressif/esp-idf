# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.generic
def test_efuse(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.qemu
@pytest.mark.esp32
@pytest.mark.host_test
def test_efuse_qemu(dut: Dut) -> None:
    dut.run_all_single_board_cases()
