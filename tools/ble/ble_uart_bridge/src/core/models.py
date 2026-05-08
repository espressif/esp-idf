# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import annotations

from dataclasses import dataclass
from enum import Enum

from .constants import NUS_RX_CHAR_UUID
from .constants import NUS_SERVICE_UUID
from .constants import NUS_TX_CHAR_UUID


class ConnectionState(str, Enum):
    DISCONNECTED = 'DISCONNECTED'
    CONNECTING = 'CONNECTING'
    CONNECTED = 'CONNECTED'


@dataclass(frozen=True, slots=True)
class DeviceInfo:
    device_id: str
    name: str | None
    rssi: int | None


@dataclass(frozen=True, slots=True)
class BLEUARTProfile:
    service_uuid: str = NUS_SERVICE_UUID
    rx_char_uuid: str = NUS_RX_CHAR_UUID
    tx_char_uuid: str = NUS_TX_CHAR_UUID
