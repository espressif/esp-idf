# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32p4
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='lack of runners for now')
@pytest.mark.lp_i2s
def test_efuse(dut: Dut) -> None:
    dut.run_all_single_board_cases()
