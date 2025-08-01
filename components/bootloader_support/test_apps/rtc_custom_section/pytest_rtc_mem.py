# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_RTC_MEM_SUPPORTED == 1'),
    indirect=['target'],
)
def test_rtc_reserved_memory(dut: Dut) -> None:
    dut.expect_exact('SUCCESS: data were saved across reboot', timeout=10)
