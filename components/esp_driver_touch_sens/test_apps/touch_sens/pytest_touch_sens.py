# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32p4
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='esp32p4 runners do not support touch pins')
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
        'iram_safe',
    ],
    indirect=True,
)
def test_touch_sens(dut: Dut) -> None:
    dut.run_all_single_board_cases()
