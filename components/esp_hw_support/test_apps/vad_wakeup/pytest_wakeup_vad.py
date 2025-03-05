# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='lack of runners for now')
@pytest.mark.lp_i2s
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_efuse_component_ut(dut: Dut) -> None:
    dut.run_all_single_board_cases()
