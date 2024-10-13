# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


TEST_APP_IN_FLASH = [pytest.param('app_in_flash', marks=pytest.mark.esp32p4)]


@pytest.mark.esp32p4
@pytest.mark.generic
@pytest.mark.parametrize('config', TEST_APP_IN_FLASH, indirect=True)
def test_ld_non_contiguous_memory(dut: Dut) -> None:
    dut.expect_exact('buf2 placed in low sram')
    dut.expect_exact('buf1 placed in high sram')
    for _ in range(0, 4):
        dut.expect(r'Testing at 0x[0-9a-f]+ ... OK!')
