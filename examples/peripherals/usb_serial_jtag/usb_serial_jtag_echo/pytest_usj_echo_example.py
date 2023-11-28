# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

from time import sleep

import pytest
import serial
import serial.tools.list_ports
from pytest_embedded import Dut


@pytest.mark.esp32c6     # usb_serial_jtag is very similar, test C6 is enough.
@pytest.mark.usj_device
def test_usb_device_serial_example(dut: Dut) -> None:
    dut.expect_exact('USB_SERIAL_JTAG init done')
    sleep(2)

    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        if (p.device == '/dev/ttyACM0'):      # Get the usb_serial_jtag port
            with serial.Serial(p.device) as s:
                s.write(b'hi, espressif\n')
                sleep(1)
                dut.expect_exact('hi, espressif')
                res = s.readline()
                assert b'hi, espressif' in res
                s.write(b'See you again!\n')
                sleep(1)
                dut.expect_exact('See you again!')
                res = s.readline()
                assert b'See you again!' in res
                s.write(b'Echo a very long buffer. Assume this buffer is very large and you can see whole buffer\n')
                sleep(1)
                dut.expect_exact('Echo a very long')  # 16 bytes a line because we use `ESP_LOG_BUFFER_HEXDUMP` in code.
                dut.expect_exact(' buffer. Assume ')
                dut.expect_exact('this buffer is v')
                dut.expect_exact('ery large and yo')
                dut.expect_exact('u can see whole ')
                dut.expect_exact('buffer')
                res = s.readline()
                assert b'Echo a very long buffer. Assume this buffer is very large and you can see whole buffer' in res
                s.write(b'64 bytes buffer:-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-\n')
                sleep(1)
                res = s.readline()
                assert b'64 bytes buffer:-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-' in res

            return

    raise Exception('usb_serial_jtag port not found')
