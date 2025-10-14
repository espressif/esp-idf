# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os
import sys
import time
from typing import Any

import pexpect
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


def check_output(p: Any) -> None:
    text_mode = isinstance(p, Dut)
    p.expect_exact('example: Start')
    p.expect_exact('example: char_var A 0x41')
    p.expect_exact('example: var8 240 -120')
    p.expect_exact('example: var16 65303 -16954 0xff17')
    p.expect_exact('example: const int var -1024')
    p.expect_exact('example: const long long unsigned var 2095')
    p.expect_exact('example: int_var 4095 -4095')
    p.expect_exact('example: var32 9000 -2000')
    p.expect_exact('example: var64 10700 -29468')
    p.expect_exact('example: 64 32 64 vars 10700 9000 10800')
    p.expect_exact('example: var_size 96843 4')
    p.expect_exact('example: float var 1.600000')
    p.expect_exact('example: double var -70.199997')
    p.expect_exact('example: int, double, var32, float, float, char 4095 -70.199997 9000 1.600000 1.600000 A')
    p.expect(r'example: str_array 0x[0-9a-fA-F]+')

    p.expect(r'example: str_ptr 0x[0-9a-fA-F]+')
    p.expect_exact('example: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f')
    p.expect_exact('example: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f')
    p.expect_exact('example: 20 21 21 21 21 21 21 21 21 21 21 21 21 21 21 21')
    p.expect_exact('example: 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35')
    p.expect_exact('example: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f')
    p.expect_exact('example: 50 51 52 53 54 55 56 57 58 59')

    match = p.expect(r'example: buffer addr = 0x([0-9a-fA-F]+)')
    addr = int((match.group(1) if text_mode else p.match.group(1)), 16)
    p.expect_exact(f'example: {addr + 0:#x}   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|')
    p.expect_exact(f'example: {addr + 16:#x}   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|')
    p.expect_exact(f'example: {addr + 32:#x}   20 21 21 21 21 21 21 21  21 21 21 21 21 21 21 21  | !!!!!!!!!!!!!!!|')
    p.expect_exact(f'example: {addr + 48:#x}   30 31 32 33 34 35 36 37  38 39 30 31 32 33 34 35  |0123456789012345|')
    p.expect_exact(f'example: {addr + 64:#x}   40 41 42 43 44 45 46 47  48 49 4a 4b 4c 4d 4e 4f  |@ABCDEFGHIJKLMNO|')
    p.expect_exact(f'example: {addr + 80:#x}   50 51 52 53 54 55 56 57  58 59                    |PQRSTUVWXY|')
    p.expect_exact('example: Text1:The way to')
    p.expect_exact('example:  get started is ')
    p.expect_exact('example: to quit talking ')
    p.expect_exact('example: and begin doing.')
    p.expect_exact('example:  - Walt Disney')
    p.expect_exact('example: Text2:The way to')
    p.expect_exact('example:  get started is ')
    p.expect_exact('example: to quit talking ')
    p.expect_exact('example: and begin doing.')
    p.expect_exact('example:  - Walt Disney')

    match = p.expect(r'example: buffer2 addr = 0x([0-9a-fA-F]+)')
    addr = int((match.group(1) if text_mode else p.match.group(1)), 16)
    p.expect_exact(f'example: {addr + 0:#x}   00 00 00 00 01 00 00 00  02 00 00 00 03 00 00 00  |................|')
    p.expect_exact(f'example: {addr + 16:#x}   04 00 00 00 05 00 00 00  06 00 00 00 07 00 00 00  |................|')
    p.expect_exact(f'example: {addr + 32:#x}   08 00 00 00 09 00 00 00  0a 00 00 00 0b 00 00 00  |................|')
    p.expect_exact(f'example: {addr + 48:#x}   0c 00 00 00 0d 00 00 00  0e 00 00 00 0f 00 00 00  |................|')
    p.expect_exact(f'example: {addr + 64:#x}   10 00 00 00 11 00 00 00  12 00 00 00 13 00 00 00  |................|')

    match = p.expect(r'example: buffer3 addr = 0x([0-9a-fA-F]+)')
    addr = int((match.group(1) if text_mode else p.match.group(1)), 16)
    p.expect_exact(f'example: {addr + 0:#x}   00 00 00 00 00 00 00 00  01 00 00 00 00 00 00 00  |................|')
    p.expect_exact(f'example: {addr + 16:#x}   02 00 00 00 00 00 00 00  03 00 00 00 00 00 00 00  |................|')
    p.expect_exact(f'example: {addr + 32:#x}   04 00 00 00 00 00 00 00  05 00 00 00 00 00 00 00  |................|')
    p.expect_exact(f'example: {addr + 48:#x}   06 00 00 00 00 00 00 00  07 00 00 00 00 00 00 00  |................|')
    p.expect_exact(f'example: {addr + 64:#x}   08 00 00 00 00 00 00 00  09 00 00 00 00 00 00 00  |................|')
    p.expect_exact(f'example: {addr + 80:#x}   0a 00 00 00 00 00 00 00                           |........|')
    p.expect_exact('example: >>> String Formatting Tests <<<')
    p.expect_exact('example: |Hello_world|')
    p.expect_exact('example: |     ESP32|')
    p.expect_exact('example: |ESP32     |')
    p.expect_exact('example: |Hello|')

    p.expect_exact('example: >>> Character Formatting Tests <<<')
    p.expect_exact('example: |A|')

    p.expect_exact('example: >>> Integer Formatting Tests <<<')
    p.expect_exact('example: |123|')
    p.expect_exact('example: |   42|')
    p.expect_exact('example: |00042|')
    p.expect_exact('example: |42   |')
    p.expect_exact('example: |00042|')
    p.expect_exact('example: |+42|')
    p.expect_exact('example: | 42|')
    p.expect_exact('example: |123456789|')
    p.expect_exact('example: |1234567890123456789|')
    p.expect_exact('example: |       +42|')
    p.expect_exact('example: |        42|')
    p.expect_exact('example: |+42       |')
    p.expect_exact('example: | 42       |')

    p.expect_exact('example: >>> Pointer Formatting Tests <<<')
    p.expect_exact('example: |0x3ff26523|')

    p.expect_exact('example: >>> Hexadecimal Formatting Tests <<<')
    p.expect_exact('example: |ff|')
    p.expect_exact('example: |FF|')
    p.expect_exact('example: |000ff|')
    p.expect_exact('example: |000ff|')
    p.expect_exact('example: |ff   |')
    p.expect_exact('example: |ffff|')
    p.expect_exact('example: |ff|')
    p.expect_exact('example: |0x2a|')
    p.expect_exact('example: |0XFF|')
    p.expect_exact('example: |      0x2a|')
    p.expect_exact('example: |0x2a      |')

    p.expect_exact('example: >>> Octal Formatting Tests <<<')
    p.expect_exact('example: |10|')
    p.expect_exact('example: |010|')
    p.expect_exact('example: |777|')
    p.expect_exact('example: |0777|')
    p.expect_exact('example: |       052|')
    p.expect_exact('example: |052       |')

    p.expect_exact('example: >>> Float Formatting Tests <<<')
    p.expect_exact('example: |123.456000|')
    p.expect_exact('example: |123.46|')
    p.expect_exact('example: |-123.46|')
    p.expect_exact('example: |      3.14|')
    p.expect_exact('example: |3.14      |')
    p.expect_exact('example: |-123.456789|')
    p.expect_exact('example: |-123.456789|')

    p.expect_exact('example: >>> Scientific Float Formatting Tests <<<')
    p.expect_exact('example: |123456.789000|')
    p.expect_exact('example: |1.234568e+05|')
    p.expect_exact('example: |1.234568E+05|')
    p.expect_exact('example: |123457|')
    p.expect_exact('example: |123457|')

    p.expect_exact('example: >>> Literal Percent Sign Tests <<<')
    p.expect_exact('example: |%|')
    p.expect_exact('example: |%| |Hello_world|')
    p.expect_exact('example: } |Hello_world|')

    p.expect_exact('example: >>> Multiple Variables in One Log <<<')
    p.expect_exact('example: string width |     ESP32| |ESP32     | |ESP32|')
    p.expect_exact('example: shorts |ffff| |ff|')
    p.expect_exact('example:   flag | 42| |-42|')
    p.expect_exact('example: + flag |+42| |-42|')
    p.expect_exact('example: # flag long |0x499602d2| |011145401322|')
    p.expect_exact('example: longs |1234567890| |123456789012345|')
    p.expect_exact('example: float width |      3.14| |3.14      | |3.1416|')
    p.expect_exact('example: double width |-123.456789| |-123.456789| |-123.46|')

    p.expect_exact('example: Success')
    p.expect_exact('main_task: Returned from app_main()')


@pytest.mark.generic
@pytest.mark.require_elf
@pytest.mark.parametrize(
    'config',
    [
        'v1',
        'v2_bin',
        'v2_bin_with_txt',
        'v2_txt',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_log(dut: Dut) -> None:
    if dut.app.sdkconfig.get('LOG_MODE_BINARY_EN'):
        time.sleep(3)  # Gives time for output logs by dut.
        dut.serial.close()
        monitor_cmd = f'{sys.executable} -m esp_idf_monitor --port {dut.serial.port} {dut.serial.app.elf_file}'
        if dut.app.sdkconfig.get('BOOTLOADER_LOG_MODE_BINARY_EN'):
            monitor_cmd += ' ' + os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.elf')
        monitor_log_path = os.path.join(dut.logdir, 'monitor.txt')
        with open(monitor_log_path, 'w') as log, pexpect.spawn(
            monitor_cmd, logfile=log, timeout=3, encoding='utf-8', codec_errors='ignore'
        ) as p:
            check_output(p)
    else:
        check_output(dut)
