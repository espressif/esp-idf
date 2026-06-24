# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from unittest.mock import patch

from src.backend.uart_transport import validate_uart_port


class TestValidateUartPort:
    @patch('src.backend.uart_transport.list_serial_ports', return_value=['/dev/ttyUSB0', '/dev/ttyUSB1'])
    def test_valid_port_returns_none(self, _mock: object) -> None:
        assert validate_uart_port('/dev/ttyUSB0') is None

    @patch('src.backend.uart_transport.list_serial_ports', return_value=['/dev/ttyUSB0'])
    def test_invalid_port_returns_error(self, _mock: object) -> None:
        result = validate_uart_port('/dev/ttyUSB99')
        assert result is not None
        assert '/dev/ttyUSB99' in result

    @patch('src.backend.uart_transport.list_serial_ports', return_value=['COM3', 'COM4'])
    def test_windows_com_port_valid(self, _mock: object) -> None:
        """COM ports don't exist as filesystem paths — must not use Path.exists()."""
        assert validate_uart_port('COM3') is None

    @patch('src.backend.uart_transport.list_serial_ports', return_value=[])
    def test_empty_port_list(self, _mock: object) -> None:
        result = validate_uart_port('/dev/ttyUSB0')
        assert result is not None
