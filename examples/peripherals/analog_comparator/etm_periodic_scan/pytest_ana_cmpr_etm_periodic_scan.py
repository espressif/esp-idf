# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_ANA_CMPR_SUPPORT_ETM_SCAN == 1'),
    indirect=['target'],
)
def test_ana_cmpr_etm_periodic_scan(dut: Dut) -> None:
    dut.expect(r'Monitor GPIO \d+')
    dut.expect(r'Analog comparator source GPIO \d+')
    dut.expect_exact('Analog comparator internal reference 50% VDD')
    dut.expect(r'GPTimer scan period \d+ us')
    dut.expect_exact('Periodic ETM-driven comparator scan started')
