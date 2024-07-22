# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
def test_ana_cmpr(dut: Dut) -> None:
    dut.run_all_single_board_cases()
