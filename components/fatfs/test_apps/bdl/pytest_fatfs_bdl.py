# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.generic
def test_fatfs_bdl(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)
