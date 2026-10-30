# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.host_test
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_int_wdt_linux(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('"int_wdt_fires_on_tick_starvation"')
    dut.expect_exact('Entering critical section to starve interrupt WDT')
    dut.expect_exact('----- interrupt watchdog reset triggered ----', timeout=10)


@pytest.mark.host_test
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_task_wdt_linux(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('"task_wdt_fires_on_idle_starvation"')
    dut.expect_exact('Busy-looping to starve idle task (task WDT)')
    dut.expect_exact('Task watchdog got triggered.', timeout=10)
