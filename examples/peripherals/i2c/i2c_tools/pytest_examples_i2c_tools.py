# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded_idf import IdfDut

EXPECT_TIMEOUT = 20


@pytest.mark.esp32
@pytest.mark.ccs811
def test_i2ctools_example(dut: IdfDut) -> None:
    dut.expect_exact('i2c-tools>', timeout=EXPECT_TIMEOUT)
    # Get i2c address
    dut.write('i2cdetect')
    dut.expect_exact('5b', timeout=EXPECT_TIMEOUT)
    # Get chip ID
    dut.write('i2cget -c 0x5b -r 0x20 -l 1')
    dut.expect_exact('0x81', timeout=EXPECT_TIMEOUT)
    # Reset sensor
    dut.write('i2cset -c 0x5b -r 0xFF 0x11 0xE5 0x72 0x8A')
    dut.expect_exact('OK', timeout=EXPECT_TIMEOUT)
    # Get status
    dut.write('i2cget -c 0x5b -r 0x00 -l 1')
    dut.expect_exact('0x10', timeout=EXPECT_TIMEOUT)
    # Change work mode
    dut.write('i2cset -c 0x5b -r 0xF4')
    dut.expect_exact('OK', timeout=EXPECT_TIMEOUT)
    dut.write('i2cset -c 0x5b -r 0x01 0x10')
    dut.expect_exact('OK', timeout=EXPECT_TIMEOUT)
    # Get new status
    dut.write('i2cget -c 0x5b -r 0x00 -l 1')
    dut.expect_exact(['0x98', '0x90'], timeout=EXPECT_TIMEOUT)
