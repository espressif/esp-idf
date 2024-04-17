# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
from time import sleep

import pytest
from pytest_embedded import Dut
from serial import Serial
from serial.tools.list_ports import comports


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.temp_skip_ci(targets=['esp32s3', 'esp32p4'], reason='lack of runners with usb_device tag')
@pytest.mark.usb_device
def test_usb_device_console_example(dut: Dut) -> None:
    dut.expect_exact('USB initialization DONE')
    dut.expect_exact('example: log -> UART')
    dut.expect_exact('example: print -> stdout')
    dut.expect_exact('example: print -> stderr')

    # Find device with Espressif TinyUSB VID/PID
    sleep(2)  # Some time for the OS to enumerate our USB device
    ports = comports()
    for port, _, hwid in ports:
        if '303A:4001' in hwid:
            with Serial(port) as s:
                # Assert TinyUSB output: Read 3 lines and check their content
                serial_output = list()
                serial_output.append(s.readline())
                serial_output.append(s.readline())
                serial_output.append(s.readline())
                assert any(b'example: log -> USB' in out for out in serial_output)
                assert any(b'example: print -> stdout' in out for out in serial_output)
                assert any(b'example: print -> stderr' in out for out in serial_output)
            return

    raise Exception('TinyUSB COM port not found')
