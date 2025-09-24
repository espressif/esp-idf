# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
# ruff: noqa: UP007
"""
Format String Parser
====================
Parses C-style format strings and handles argument formatting for log compression.
"""

import struct
from typing import Union


def parse_format_string(format_str: str) -> list[Union[str, tuple[int, int, str, str, str, str, str]]]:
    """
    Parse a format string into tokens.

    Args:
        format_str: C-style format string

    Returns:
        List of tokens (strings or format spec tuples)
    """
    tokens: list[Union[str, tuple[int, int, str, str, str, str, str]]] = []
    i = 0
    n = len(format_str)

    while i < n:
        if format_str[i] == '%':
            start = i
            i += 1

            # Handle escaped %
            if i < n and format_str[i] == '%':
                tokens.append('%')
                i += 1
                continue

            # Parse flags
            flags = ''
            while i < n and format_str[i] in '-+ #0':
                flags += format_str[i]
                i += 1

            # Parse width
            width = ''
            while i < n and format_str[i].isdigit():
                width += format_str[i]
                i += 1

            # Parse precision
            precision = ''
            if i < n and format_str[i] == '.':
                i += 1
                while i < n and format_str[i].isdigit():
                    precision += format_str[i]
                    i += 1

            # Parse length modifier
            length = ''
            if i < n and format_str[i] in 'zhl':
                length += format_str[i]
                i += 1
                # Handle double length (e.g., ll)
                if i < n and format_str[i] == 'l' and length == 'l':
                    length += 'l'
                    i += 1

            if i < n and format_str[i] in 'diuoxXfcsplL':
                conv_char = format_str[i]
                i += 1
                full_spec = format_str[start:i]
                tokens.append((start, i, full_spec, flags, width, length, conv_char))
            else:
                tokens.append(format_str[start:i])
        else:
            # Regular text
            start = i
            while i < n and format_str[i] != '%':
                i += 1
            tokens.append(format_str[start:i])

    return tokens


def format_integer(value: int, conv_char: str, flags: str, width: str, length_mod: str) -> str:
    """
    Format an integer value according to format specifiers.

    Args:
        value: Integer value
        conv_char: Conversion character (d, i, u, o, x, X)
        flags: Format flags
        width: Minimum width
        length_mod: Length modifier

    Returns:
        Formatted string
    """
    # Determine base
    base = 10
    uppercase = False
    if conv_char in 'xX':
        base = 16
        uppercase = conv_char == 'X'
    elif conv_char == 'o':
        base = 8

    # Generate number string
    if base == 16:
        num_str = format(value, 'X' if uppercase else 'x')
    elif base == 8:
        num_str = format(value, 'o')
    else:  # decimal
        num_str = str(value)

    # Add prefix
    prefix = ''
    if '#' in flags and value != 0:
        if base == 16:
            prefix = '0X' if uppercase else '0x'
        elif base == 8:
            prefix = '0'

    # Apply width and padding
    total_len = len(prefix) + len(num_str)
    width_val = int(width) if width else 0

    if width_val > total_len:
        padding = width_val - total_len
        if '0' in flags and '-' not in flags:
            num_str = num_str.zfill(padding + len(num_str))
        else:
            pad_char = ' ' * padding
            if '-' in flags:
                num_str = prefix + num_str + pad_char
                prefix = ''
            else:
                num_str = pad_char + prefix + num_str
                prefix = ''

    return prefix + num_str


def parse_compressed_arguments(byte_sequence: bytes, format_str: str) -> str:
    """
    Parse compressed log arguments into formatted string.

    Args:
        byte_sequence: Compressed argument bytes
        format_str: Original format string

    Returns:
        Formatted log string

    Raises:
        ValueError: If the input is invalid
    """
    if len(byte_sequence) < 2:
        raise ValueError('Insufficient bytes for header')

    # Parse header
    header = (byte_sequence[0] << 8) | byte_sequence[1]
    type_flag = (header >> 15) & 0x01
    arg_count = header & 0x7FFF

    if type_flag != 1:
        raise ValueError(f'Unsupported type flag: {type_flag}')

    # Parse size list
    size_bytes_needed = (arg_count + 1) // 2
    if len(byte_sequence) < 2 + size_bytes_needed:
        raise ValueError('Insufficient bytes for size list')

    size_bytes = byte_sequence[2 : 2 + size_bytes_needed]
    arg_sizes = []

    for i in range(arg_count):
        byte_index = i // 2
        if i % 2 == 0:
            size = (size_bytes[byte_index] >> 4) & 0x0F
        else:
            size = size_bytes[byte_index] & 0x0F
        arg_sizes.append(size)

    # Parse arguments
    args = []
    pos = 2 + size_bytes_needed

    for size in arg_sizes:
        if pos + size > len(byte_sequence):
            raise ValueError('Insufficient bytes for arguments')

        args.append(byte_sequence[pos : pos + size])
        pos += size

    # Parse format string
    tokens = parse_format_string(format_str)
    output = []
    arg_index = 0

    for token in tokens:
        if isinstance(token, tuple):
            start, end, flags, width, precision, length_mod, conv_char = token

            if conv_char == '%':
                output.append('%')
            else:
                if arg_index >= len(args):
                    raise ValueError('Not enough arguments for format string')

                arg_bytes = args[arg_index]
                arg_index += 1

                # Character type
                if conv_char == 'c':
                    # Pad to 4 bytes for unpacking
                    padded = arg_bytes.ljust(4, b'\x00')
                    char_code = struct.unpack('>I', padded)[0]
                    output.append(chr(char_code))

                # Pointer type
                elif conv_char == 'p':
                    ptr_value = int.from_bytes(arg_bytes, 'big', signed=False)
                    output.append(hex(ptr_value))

                # Floating point types
                elif conv_char in 'fFeEgGaA':
                    if len(arg_bytes) == 4:
                        float_value = struct.unpack('>f', arg_bytes)[0]
                    elif len(arg_bytes) == 8:
                        float_value = struct.unpack('>d', arg_bytes)[0]
                    else:
                        raise ValueError(f'Unsupported float size: {len(arg_bytes)} bytes')
                    output.append(str(float_value))

                # Integer types
                elif conv_char in 'diuoxX':
                    signed = conv_char in 'di'

                    # Determine expected size
                    if length_mod == 'll':
                        expected_size = 8
                    elif length_mod in ('l', 'z', 'j', 't') or conv_char == 'p':
                        expected_size = 4
                    else:
                        expected_size = len(arg_bytes)

                    # Pad to expected size
                    if len(arg_bytes) < expected_size:
                        if signed and arg_bytes and (arg_bytes[0] & 0x80):
                            # Sign extension for negative numbers
                            pad = b'\xff' * (expected_size - len(arg_bytes))
                        else:
                            pad = b'\x00' * (expected_size - len(arg_bytes))
                        arg_bytes = pad + arg_bytes
                    elif len(arg_bytes) > expected_size:
                        arg_bytes = arg_bytes[:expected_size]

                    # Convert to integer
                    int_value = int.from_bytes(arg_bytes, 'big', signed=signed)
                    output.append(format_integer(int_value, conv_char, flags, width, length_mod))
                else:
                    raise ValueError(f'Unsupported conversion: {conv_char}')
        else:
            output.append(token)

    return ''.join(output)
