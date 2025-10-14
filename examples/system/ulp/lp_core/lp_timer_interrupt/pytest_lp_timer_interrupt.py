# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import time

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32c5', 'esp32c6', 'esp32p4'], indirect=['target'])
def test_lp_timer_interrupt(dut: Dut) -> None:
    # Wait for LP core to be loaded and running
    dut.expect_exact('LP core loaded with firmware and running successfully')

    # Add a delay to allow interrupts to occur
    time.sleep(2)

    # Wait for the interrupt count line to be printed
    interrupt_count = dut.expect(r'Interrupt count: (\d+)')
    count = int(interrupt_count.group(1).decode('utf8'))
    assert count > 0, f'Expected interrupt count to be greater than 0, got {count}'
