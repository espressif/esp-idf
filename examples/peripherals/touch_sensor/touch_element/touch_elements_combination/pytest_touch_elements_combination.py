# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32s2', 'esp32s3'], indirect=['target'])
def test_touch_elements_combination(dut: Dut) -> None:
    dut.expect_exact('Touch Elements Combination Example: Touch element library installed')
    dut.expect_exact('Touch Elements Combination Example: Touch button installed')
    dut.expect_exact('Touch Elements Combination Example: Touch buttons created')
    dut.expect_exact('Touch Elements Combination Example: Touch slider installed')
    dut.expect_exact('Touch Elements Combination Example: Touch slider created')
    dut.expect_exact('Touch Elements Combination Example: Touch element library start')
