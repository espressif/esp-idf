# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.psram
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_pthread_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=10)
