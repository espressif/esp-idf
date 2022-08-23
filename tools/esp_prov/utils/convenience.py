# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

# Convenience functions for commonly used data type conversions

def bytes_to_long(s: bytes) -> int:
    return int.from_bytes(s, 'big')


def long_to_bytes(n: int) -> bytes:
    if n == 0:
        return b'\x00'
    return n.to_bytes((n.bit_length() + 7) // 8, 'big')


# 'deadbeef' -> b'deadbeef'
def str_to_bytes(s: str) -> bytes:
    return bytes(s, encoding='latin-1')


# 'deadbeef' -> b'\xde\xad\xbe\xef'
def hex_str_to_bytes(s: str) -> bytes:
    return bytes.fromhex(s)
