# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.lp_i2s
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_lp_i2s(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=200)
