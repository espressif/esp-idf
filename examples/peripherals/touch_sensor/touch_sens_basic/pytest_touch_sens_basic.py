# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TOUCH_SENSOR_SUPPORTED == 1'), indirect=['target'])
def test_touch_sens(dut: Dut) -> None:
    dut.expect(r'Touch \[CH [0-9]+\] enabled on GPIO[0-9]+')
    dut.expect_exact('Initial benchmark and new threshold are:')
    dut.expect(r'Touch \[CH [0-9]+\] 0: [0-9]+, [0-9]+')
    dut.expect(r'smooth    \[CH [0-9]+\]: [0-9]+')
