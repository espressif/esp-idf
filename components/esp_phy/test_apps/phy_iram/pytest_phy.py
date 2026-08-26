# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.esp32h2_rev1_2
@idf_parametrize('target', ['esp32h2'], indirect=['target'])
def test_phy(dut: Dut) -> None:
    dut.run_all_single_board_cases()
