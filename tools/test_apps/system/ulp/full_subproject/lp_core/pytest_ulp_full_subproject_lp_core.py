# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32c5', 'esp32c6', 'esp32p4', 'esp32s31'], indirect=['target'])
def test_ulp_full_subproject_lp_core(dut: Dut) -> None:
    dut.expect_exact('Sum calculated by ULP (ulp_sum component): 11')
    dut.expect_exact('Product calculated by ULP (ulp_math component): 21')
    dut.expect_exact('Example finished')
