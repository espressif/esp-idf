# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import pytest
from pytest_embedded_idf import IdfDut


@pytest.mark.parametrize('offset', [
    pytest.param('0x40007d54', marks=[pytest.mark.esp32]),
    # pytest.param('0x4000f6e2', marks=[pytest.mark.esp32s2]),
    # pytest.param('0x40047654', marks=[pytest.mark.esp32c3]),
])
@pytest.mark.parametrize('embedded_services, skip_autoflash, erase_all', [
    ('esp,idf,jtag', 'y', 'y'),
], indirect=True)
@pytest.mark.jtag
def test_loadable_elf(dut: IdfDut, offset: str) -> None:
    dut.gdb.write('mon reset halt')
    dut.gdb.write(f'thb *{offset}')
    assert 'Temporary breakpoint 1, 0x40007d54' in dut.gdb.write('c')
    dut.gdb.write('load')
    dut.gdb.write('tb app_main')
    assert 'Thread 2 "main" hit Temporary breakpoint 2, app_main ()' in dut.gdb.write('c')
    dut.gdb.write('tb esp_restart')
    assert 'Thread 2 "main" hit Temporary breakpoint 3, esp_restart ()' in dut.gdb.write('c')
    dut.expect_exact('Restarting now.')
