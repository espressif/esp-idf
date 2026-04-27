# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import annotations


class BUBError(Exception):
    """Base exception for the BLE UART Bridge."""


class DeviceNotFoundError(BUBError):
    """Raised when the requested BLE UART device cannot be found."""


class ConnectionTimeout(BUBError):
    """Raised when the client cannot connect to a BLE UART device."""


class NotConnectedError(BUBError):
    """Raised when a read/write operation requires an open connection."""


class ReadTimeout(BUBError):
    """Raised when no data arrives before a read timeout expires."""


class WriteError(BUBError):
    """Raised when a BLE write operation fails."""


class ServiceDiscoveryError(BUBError):
    """Raised when required service/characteristics are missing."""


class BufferOverflowError(BUBError):
    """Raised when the receive buffer exceeds its configured limit."""
