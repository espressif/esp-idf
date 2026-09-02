# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32s2'], indirect=['target'])
def test_dac_oneshot_example(dut: Dut) -> None:
    dut.expect_exact('DAC oneshot example started', timeout=10)
    dut.expect_exact('dac_chan0 = 0', timeout=10)
    dut.expect_exact('dac_chan1 = 0', timeout=10)
    # next round
    dut.expect_exact('dac_chan0 = 0', timeout=10)
    dut.expect_exact('dac_chan1 = 0', timeout=10)
