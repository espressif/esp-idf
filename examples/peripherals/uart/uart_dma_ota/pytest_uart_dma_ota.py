# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os

import pytest
import serial
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

PACKET_SIZE = 10 * 1024  # 10 KB for upper computer
FLASH_PORT = '/dev/serial_ports/ttyUSB-esp32'


def send_file_via_uart(port: str, baud_rate: int, file_path: str, packet_size: int) -> None:
    try:
        with serial.Serial(port, baud_rate, timeout=1) as ser:
            print(f'Opened {port} at {baud_rate} baud')

            file_size = os.path.getsize(file_path)
            print(f'File size: {file_size} bytes')

            with open(file_path, 'rb') as file:
                bytes_sent = 0
                while bytes_sent < file_size:
                    chunk = file.read(packet_size)
                    if not chunk:
                        break
                    ser.write(chunk)
                    bytes_sent += len(chunk)

            print('File sent successfully!')

    except Exception as e:
        print(f'Error: {e}')


@pytest.mark.usb_serial_jtag
@pytest.mark.parametrize(
    'port, flash_port, config',
    [
        pytest.param('/dev/serial_ports/ttyACM-esp32', FLASH_PORT, 'defaults'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c6', 'esp32c3', 'esp32s3', 'esp32h2'], indirect=['target'])
def test_uart_dma_ota(dut: Dut) -> None:
    dut.expect_exact('uhci-example: OTA process started')
    # We OTA the same binary to another partition and switch to there.
    binary_path = os.path.join(dut.app.binary_path, 'uart_dma_ota.bin')
    assert os.path.exists(binary_path), f'OTA binary not found at {binary_path}'

    buad_rate = dut.app.sdkconfig.get('UART_BAUD_RATE')
    send_file_via_uart(FLASH_PORT, buad_rate, binary_path, PACKET_SIZE)

    dut.expect('OTA update successful. Rebooting', timeout=10)
    dut.expect('ESP-ROM:', timeout=10)
