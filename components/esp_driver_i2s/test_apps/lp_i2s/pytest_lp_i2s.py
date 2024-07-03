# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32p4
@pytest.mark.lp_i2s
def test_lp_i2s(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=200)
