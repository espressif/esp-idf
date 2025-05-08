# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
from time import sleep

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from serial import Serial
from serial.tools.list_ports import comports


@pytest.mark.temp_skip_ci(targets=['esp32s3'], reason='lack of runners with usb_device tag')
@pytest.mark.usb_device
@idf_parametrize('target', ['esp32s2', 'esp32s3', 'esp32p4'], indirect=['target'])
def test_usb_device_serial_example(dut: Dut) -> None:
    dut.expect_exact('USB initialization DONE')
    sleep(2)  # Some time for the OS to enumerate our USB device

    # Find device with Espressif TinyUSB VID/PID
    ports = comports()
    for port, _, hwid in ports:
        if '303A:4001' in hwid:
            with Serial(port) as s:
                s.write('text\r\n'.encode())  # Write dummy text to COM port
                dut.expect_exact('Data from channel 0:')  # Check ESP log
                dut.expect_exact('|text..|')
                res = s.readline()  # Check COM echo
                assert b'text\r\n' in res
            return

    raise Exception('TinyUSB COM port not found')
