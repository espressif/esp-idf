# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""UART read loop with raw binary file writing.

See Spec Sections 6, 12.
"""

import serial
import serial.tools.list_ports

UART_READ_TIMEOUT = 0.1
UART_BLOCK_SIZE = 50 * 1024


def list_serial_ports() -> list[str]:
    ports = serial.tools.list_ports.comports()
    return [port.device for port in ports]


def validate_uart_port(port: str) -> str | None:
    """Validate port exists and is accessible. Returns error message or None if valid."""
    available = list_serial_ports()
    if port not in available:
        return f"UART port '{port}' not found. Available: {available}"
    return None


def open_serial(port: str, baudrate: int) -> serial.Serial:
    try:
        return serial.Serial(port, baudrate=baudrate, timeout=UART_READ_TIMEOUT, exclusive=True)
    except (ValueError, serial.SerialException):
        return serial.Serial(port, baudrate=baudrate, timeout=UART_READ_TIMEOUT)
