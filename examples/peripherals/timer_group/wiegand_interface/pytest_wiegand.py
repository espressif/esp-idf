# SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_GPTIMER_SUPPORTED == 1 and IDF_TARGET != "esp32c2"'),
    indirect=['target'],
)
def test_gptimer_wiegand(dut: Dut) -> None:
    dut.expect_exact('example: Configure wiegand interface')
    dut.expect_exact('example: send wiegand data')
