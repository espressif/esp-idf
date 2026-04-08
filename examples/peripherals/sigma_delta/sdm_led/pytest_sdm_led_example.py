# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_SDM_SUPPORTED == 1'),
    indirect=['target'],
)
def test_sdm_led_example(dut: Dut) -> None:
    dut.expect_exact('sdm_led: Install sigma delta channel')
    dut.expect_exact('sdm_led: Enable sigma delta channel')
    dut.expect_exact('sdm_led: Change duty cycle continuously')
