# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
from time import sleep

import pytest
from pytest_embedded import Dut
from serial import Serial
from serial.tools.list_ports import comports


@pytest.mark.esp32s2
@pytest.mark.usb_device
def test_usb_composite_device_serial_example(dut: Dut) -> None:
    dut.expect_exact('Hello World!')
    dut.expect_exact('USB Composite initialization')
    dut.expect_exact('USB Composite initialization DONE')
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
