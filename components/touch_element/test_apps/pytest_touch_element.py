# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'opt_o0',
        'opt_o2'
    ],
    indirect=True,
)
def test_touch_element(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)
