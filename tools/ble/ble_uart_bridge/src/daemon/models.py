# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from typing import Any

from pydantic import BaseModel
from pydantic import Field

MAX_OP_LENGTH = 64
MAX_REQUEST_DATA_BYTES = 4096


class BLEUARTRequestPayload(BaseModel):
    op: str = Field('raw', min_length=1, max_length=MAX_OP_LENGTH, description='Operation name to send to BLE device')
    data: Any = Field(..., description='Request payload to send to BLE device')
    timeout: float = Field(10.0, gt=0, description='Response timeout in seconds')


class BLEUARTNotifyPayload(BaseModel):
    op: str = Field('raw', min_length=1, max_length=MAX_OP_LENGTH, description='Operation name to send to BLE device')
    data: Any = Field(..., description='Notification payload to send to BLE device')
