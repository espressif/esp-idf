# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Checksum implementations matching BLE Log firmware (ble_log_util.c).

Two algorithms:
- sum_checksum: byte-by-byte sum
- xor_checksum: 32-bit word XOR matching firmware ble_log_fast_checksum()

The firmware's ror32 alignment compensation makes the XOR checksum
alignment-independent — simple word-by-word XOR produces the same result
regardless of the original buffer alignment.
"""

import struct


def sum_checksum(data: bytes) -> int:
    return sum(data) & 0xFFFFFFFF


def xor_checksum(data: bytes) -> int:
    """Compute XOR checksum matching firmware ble_log_fast_checksum().

    XORs consecutive 4-byte little-endian words. Partial last word is
    zero-padded. Alignment-independent due to firmware's ror32 compensation.
    """
    length = len(data)
    if length == 0:
        return 0

    checksum = 0
    for i in range(0, length, 4):
        remaining = length - i
        if remaining >= 4:
            (word,) = struct.unpack_from('<I', data, i)
        else:
            chunk = data[i:] + b'\x00' * (4 - remaining)
            (word,) = struct.unpack('<I', chunk)

        checksum ^= word

    return checksum & 0xFFFFFFFF
