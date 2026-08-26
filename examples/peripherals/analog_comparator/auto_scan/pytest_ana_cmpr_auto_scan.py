# SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'intl',
        'ext',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_ANA_CMPR_SUPPORT_AUTO_SCAN == 1'),
    indirect=['target'],
)
def test_ana_cmpr_auto_scan(dut: Dut) -> None:
    sdkconfig = dut.app.sdkconfig
    if sdkconfig['EXAMPLE_REF_FROM_INTERNAL']:
        dut.expect_exact('Allocate Analog Comparator with internal reference voltage')
    elif sdkconfig['EXAMPLE_REF_FROM_EXTERNAL']:
        dut.expect_exact('Allocate Analog Comparator with external reference from GPIO')
    dut.expect_exact('Analog comparator enabled')
