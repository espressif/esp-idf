# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os
import re
import typing

import pexpect
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

if typing.TYPE_CHECKING:
    from conftest import OpenOCD


def _test_idf_gdb(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    # Need to wait a moment to connect via OpenOCD after the hard reset happened.
    # Along with this check that app runs ok
    dut.expect('Hello world!')

    # Don't need to have output from UART anymore
    dut.serial.stop_redirect_thread()

    with openocd_dut.run(), open(os.path.join(dut.logdir, 'gdb.txt'), 'w') as gdb_log, pexpect.spawn(
        f'idf.py -B {dut.app.binary_path} gdb --batch',
        timeout=60,
        logfile=gdb_log,
        encoding='utf-8',
        codec_errors='ignore',
    ) as p:
        p.expect(re.compile(r'add symbol table from file.*bootloader.elf'))
        p.expect(
            re.compile(r'add symbol table from file.*rom.elf')
        )  # if fail here: add target support here https://github.com/espressif/esp-rom-elfs
        p.expect_exact('hit Temporary breakpoint 1, app_main ()')


@pytest.mark.jtag
@idf_parametrize('target', ['esp32', 'esp32c2', 'esp32s2'], indirect=['target'])
def test_idf_gdb(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    _test_idf_gdb(openocd_dut, dut)


@pytest.mark.usb_serial_jtag
@idf_parametrize('target', ['esp32s3', 'esp32c3', 'esp32c6', 'esp32h2'], indirect=['target'])
def test_idf_gdb_usj(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    _test_idf_gdb(openocd_dut, dut)
