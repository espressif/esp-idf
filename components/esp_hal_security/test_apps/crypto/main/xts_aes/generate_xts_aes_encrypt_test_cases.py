#!/usr/bin/env python
# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
# import struct
import sys
import tempfile

import espsecure


def encrypt_and_print_aes_xts(keyfile: str, plaintext: bytes, address: int) -> None:
    ciphertext = b''

    inputfile = tempfile.NamedTemporaryFile()
    outputfile = tempfile.NamedTemporaryFile()

    with open(inputfile.name, 'wb') as f:
        f.write(plaintext)

    espsecure.main(
        [
            'encrypt-flash-data',
            '--aes-xts',
            '--keyfile',
            f'{keyfile}',
            '--output',
            f'{outputfile.name}',
            '--address',
            f'{address}',
            f'{inputfile.name}',
        ]
    )

    with open(outputfile.name, 'rb') as f:
        ciphertext = f.read()

    assert len(ciphertext) == len(plaintext)

    # fmt: off
    print(
        '{\n'
        f'    .address = {address:#08x},\n'
        '    .ciphertext = {\n'
        f'{as_c_array(ciphertext)}\n'
        '    },\n'
        '},'
    )
    # fmt: on


def as_c_array(byte_arr: bytes) -> str:
    hex_str = ''
    bytes_per_line = 16
    for idx, byte in enumerate(byte_arr):
        if idx % bytes_per_line == 0:
            hex_str += '        '
        hex_str += f'{byte:#02x}, '
        if idx % bytes_per_line == bytes_per_line - 1 and idx != (len(byte_arr) - 1):
            hex_str += '\n'

    return hex_str


def gen_aes_xts_test_case_output(keyfile: str) -> None:
    plaintext = bytes(range(1, 129))
    addresses = [0x160000, 0x160010, 0x160020, 0x160030, 0x160040, 0x160050, 0x160060, 0x160070]

    for addr in addresses:
        encrypt_and_print_aes_xts(keyfile, plaintext, addr)


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Please specify the XTS-AES keyfile!')
        sys.exit(1)

    gen_aes_xts_test_case_output(sys.argv[1])
