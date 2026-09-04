# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32s2'], indirect=['target'])
def test_dac_cosine_wave_example(dut: Dut) -> None:
    dut.expect_exact('DAC cosine wave started', timeout=10)
