# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32c3', 'esp32p4', 'esp32s2'], indirect=['target'])
def test_rt_mqueue(dut: Dut) -> None:
    dut.run_all_single_board_cases()
