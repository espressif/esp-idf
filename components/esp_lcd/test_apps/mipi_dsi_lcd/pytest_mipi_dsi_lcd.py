# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='no runner')
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_dsi_lcd(dut: Dut) -> None:
    dut.run_all_single_board_cases()
