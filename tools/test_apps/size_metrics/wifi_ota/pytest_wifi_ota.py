# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.skip(reason='Build-only configuration used for size metrics')
@pytest.mark.parametrize('config', ['default'], indirect=True)
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_WIFI_SUPPORTED == 1', targets='supported_targets'),
    indirect=['target'],
)
def test_wifi_ota_build_only(dut: IdfDut) -> None:
    pass
