# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32p4
@pytest.mark.esp32c5
@pytest.mark.generic
def test_bitscrambler(dut: Dut) -> None:
    dut.run_all_single_board_cases()
