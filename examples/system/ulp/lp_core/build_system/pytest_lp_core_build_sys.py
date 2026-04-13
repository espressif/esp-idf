# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_LP_CORE_SUPPORTED == 1'), indirect=['target'])
def test_lp_core_build_sys(dut: IdfDut) -> None:
    dut.expect('Sum calculated by ULP using external library func: 11')
