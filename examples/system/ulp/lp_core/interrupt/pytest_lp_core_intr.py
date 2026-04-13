# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_LP_CORE_SUPPORTED == 1'), indirect=['target'])
def test_lp_core_intr(dut: Dut) -> None:
    dut.expect('Triggered 10 interrupts on the LP-Core, LP-Core received 10 interrupts')
