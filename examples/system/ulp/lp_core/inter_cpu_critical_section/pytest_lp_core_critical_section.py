# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32c5', 'esp32c6'], indirect=['target'])
def test_lp_core_critical_section_main_1_task(dut: Dut) -> None:
    dut.expect("LP CPU's increment starts, shared counter = 0")
    dut.expect(r'core 0 started, cnt = \d+')
    # Either LP and HP can stop increasing earlier
    dut.expect('increment ends, shared counter = 200000')


@pytest.mark.generic
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_lp_core_critical_section_main_2_tasks(dut: Dut) -> None:
    dut.expect("LP CPU's increment starts, shared counter = 0")
    dut.expect(r'core 0 started, cnt = \d+')
    dut.expect(r'core 1 started, cnt = \d+')
    # Either LP and HP can stop increasing earlier
    dut.expect('increment ends, shared counter = 300000')
