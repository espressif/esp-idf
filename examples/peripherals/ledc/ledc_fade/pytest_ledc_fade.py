# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_LEDC_SUPPORTED == 1'), indirect=['target'])
def test_ledc_fade_example(dut: IdfDut) -> None:
    # repeat to test at least two rounds of fade
    dut.expect_exact('1. LEDC fade up to duty = 4000')
    dut.expect_exact('2. LEDC fade down to duty = 0')
    dut.expect_exact('3. LEDC set duty = 4000 without fade')
    dut.expect_exact('4. LEDC set duty = 0 without fade')
    dut.expect_exact('1. LEDC fade up to duty = 4000')
    dut.expect_exact('2. LEDC fade down to duty = 0')
    dut.expect_exact('3. LEDC set duty = 4000 without fade')
    dut.expect_exact('4. LEDC set duty = 0 without fade')
