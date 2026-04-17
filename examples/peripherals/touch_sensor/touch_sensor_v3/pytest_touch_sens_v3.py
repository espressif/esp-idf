# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='esp32p4 runners do not support touch pins')
@pytest.mark.generic
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_touch_sens_v3(dut: Dut) -> None:
    dut.expect_exact('Initial benchmark and new threshold are:')
    dut.expect(r'\[CH [0-9]+\] 0: [0-9]+, [0-9]+')
    dut.expect(r'benchmark \[CH [0-9]+\]: [0-9]+')
    dut.expect(r'chan_data \[CH [0-9]+\]: [0-9]+')
