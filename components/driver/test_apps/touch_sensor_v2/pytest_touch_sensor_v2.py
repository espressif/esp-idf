# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s2', 'esp32s3'], indirect=['target'])
def test_touch_sensor_v2(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)
