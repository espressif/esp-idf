# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'cache_safe',
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32h2', 'esp32p4', 'esp32c5', 'esp32c61'], indirect=['target'])
def test_ana_cmpr(dut: Dut) -> None:
    dut.run_all_single_board_cases()
