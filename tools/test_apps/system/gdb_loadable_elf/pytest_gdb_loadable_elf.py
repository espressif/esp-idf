# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.jtag
@idf_parametrize('embedded_services', ['esp,idf,jtag'], indirect=['embedded_services'])
@idf_parametrize('erase_all', ['y'], indirect=['erase_all'])
@idf_parametrize('offset', ['0x40007d54'])
@idf_parametrize('skip_autoflash', ['y'], indirect=['skip_autoflash'])
@idf_parametrize('target', ['esp32'], indirect=['target'])
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
