# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.generic
def test_himem(dut: Dut) -> None:

    mem = dut.expect(r'esp_himem: Initialized. Using last \d+ 32KB address blocks for bank '
                     r'switching on (\d+) KB of physical memory.').group(1).decode('utf8')

    dut.expect(r'Himem has {}KiB of memory, \d+KiB of which is free.'.format(mem), timeout=10)
    dut.expect_exact('Testing the free memory...')
    dut.expect_exact('Done!')
