# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_PPA_SUPPORTED == 1'), indirect=['target'])
def test_ppa_rgb_lcd(dut: Dut) -> None:
    dut.expect_exact('Calling app_main()')
    dut.expect_exact('ppa_rgb: Install RGB LCD panel driver')
    dut.expect_exact('ppa_rgb: Initialize RGB LCD panel')
    # ...
    dut.expect_exact('start srm operations')
    dut.expect_exact('start blend operations')
    dut.expect_exact('start fill operations')
    dut.expect_exact('Returned from app_main()')
