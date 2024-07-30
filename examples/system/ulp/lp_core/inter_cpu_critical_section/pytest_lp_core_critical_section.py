# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32c5
@pytest.mark.esp32c6
@pytest.mark.generic
def test_lp_core_critical_section_main_1_task(dut: Dut) -> None:
    dut.expect("LP CPU's increment starts, shared counter = 0")
    dut.expect(r'core 0 started, cnt = \d+')
    # Either LP and HP can stop increasing earlier
    dut.expect('increment ends, shared counter = 200000')


@pytest.mark.esp32p4
@pytest.mark.generic
def test_lp_core_critical_section_main_2_tasks(dut: Dut) -> None:
    dut.expect("LP CPU's increment starts, shared counter = 0")
    dut.expect(r'core 0 started, cnt = \d+')
    dut.expect(r'core 1 started, cnt = \d+')
    # Either LP and HP can stop increasing earlier
    dut.expect('increment ends, shared counter = 300000')
