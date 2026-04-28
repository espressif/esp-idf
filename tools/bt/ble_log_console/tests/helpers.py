# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import struct
from collections.abc import Callable

from src.backend.models import HEADER_FMT


def build_frame_header(payload_len: int, source_code: int, frame_sn: int) -> bytes:
    """Build a 6-byte BLE Log frame header."""
    frame_meta = (source_code & 0xFF) | (frame_sn << 8)
    return struct.pack(HEADER_FMT, payload_len, frame_meta)


def build_frame(
    payload: bytes,
    source_code: int,
    frame_sn: int,
    checksum_fn: Callable[[bytes], int],
    checksum_scope_full: bool = True,
) -> bytes:
    """Build a complete BLE Log frame with header, payload, and checksum.

    Args:
        payload: Frame payload bytes (should include 4B os_ts prefix if applicable)
        source_code: BLE Log source code (0-7)
        frame_sn: 24-bit sequence number
        checksum_fn: Function(data: bytes) -> int
        checksum_scope_full: If True, checksum covers header+payload; else header only
    """
    header = build_frame_header(len(payload), source_code, frame_sn)

    if checksum_scope_full:
        checksum_data = header + payload
    else:
        checksum_data = header

    checksum_val = checksum_fn(checksum_data)
    return header + payload + struct.pack('<I', checksum_val)
