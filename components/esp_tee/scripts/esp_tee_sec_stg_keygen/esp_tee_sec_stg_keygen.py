#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import os
import struct
from enum import Enum
from enum import IntFlag
from typing import Any

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ec

# === Constants ===
SEC_STG_KEY_DATA_SZ = 256
AES_KEY_LEN = 32
AES_DEFAULT_IV_LEN = 16
AES_GCM_IV_LEN = 12
ECDSA_P256_LEN = 32
ECDSA_P384_LEN = 48


# === Key Type Enum ===
class KeyType(Enum):
    AES256 = 0
    ECDSA_P256 = 1
    ECDSA_P384 = 3


# === Bitwise Flags Enum ===
class Flags(IntFlag):
    NONE = 0x00000000
    WRITE_ONCE = 0x00000001


# === Key Generators ===


def generate_aes256_key(flags: Flags, key_file: str | None = None) -> bytes:
    if key_file:
        with open(key_file, 'rb') as f:
            key_data = f.read()

        if len(key_data) < AES_KEY_LEN:
            raise ValueError('AES key file must be at least 32 bytes long')

        key = key_data[:AES_KEY_LEN]
        iv_data = key_data[AES_KEY_LEN:]

        iv_len = len(iv_data)
        if iv_len == 0:
            iv = os.urandom(AES_DEFAULT_IV_LEN)
        elif iv_len == AES_GCM_IV_LEN:
            iv = iv_data + b'\x00' * (AES_DEFAULT_IV_LEN - AES_GCM_IV_LEN)
        elif iv_len == AES_DEFAULT_IV_LEN:
            iv = iv_data
        else:
            raise ValueError('IV length must be exactly 12 or 16 bytes, or omitted to generate one')
    else:
        key = os.urandom(AES_KEY_LEN)
        iv = os.urandom(AES_DEFAULT_IV_LEN)

    packed = struct.pack('<II32s16s', KeyType.AES256.value, flags.value, key, iv)
    return packed + b'\x00' * (SEC_STG_KEY_DATA_SZ - len(packed))


def generate_ecdsa_key(
    curve: ec.EllipticCurve, key_type_enum: KeyType, key_len: int, flags: Flags, pem_file: str | None = None
) -> bytes:
    if pem_file:
        with open(pem_file, 'rb') as f:
            private_key = serialization.load_pem_private_key(f.read(), password=None, backend=default_backend())
        if not isinstance(private_key, ec.EllipticCurvePrivateKey):
            raise ValueError('Provided PEM does not contain an EC private key')
    else:
        private_key = ec.generate_private_key(curve, default_backend())

    priv = private_key.private_numbers().private_value.to_bytes(key_len, 'big')
    pub = private_key.public_key().public_numbers()
    x = pub.x.to_bytes(key_len, 'big')
    y = pub.y.to_bytes(key_len, 'big')

    packed = struct.pack(f'<II{key_len}s{key_len}s{key_len}s', key_type_enum.value, flags.value, priv, x, y)
    return packed + b'\x00' * (SEC_STG_KEY_DATA_SZ - len(packed))


def generate_key_data(key_type: KeyType, flags: Flags, input_file: str | None) -> bytes:
    if key_type == KeyType.AES256:
        return generate_aes256_key(flags, input_file)
    elif key_type == KeyType.ECDSA_P256:
        return generate_ecdsa_key(ec.SECP256R1(), key_type, ECDSA_P256_LEN, flags, input_file)
    elif key_type == KeyType.ECDSA_P384:
        return generate_ecdsa_key(ec.SECP384R1(), key_type, ECDSA_P384_LEN, flags, input_file)
    else:
        raise ValueError(f'Unsupported key type: {key_type}')


# === CLI ===


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description='Generate or import a cryptographic key structure for secure storage')
    parser.add_argument(
        '-k',
        '--key-type',
        type=str,
        choices=[e.name.lower() for e in KeyType],
        required=True,
        help='key type to be processed',
    )
    parser.add_argument(
        '-o',
        '--output',
        required=True,
        help='output binary file name',
    )
    parser.add_argument(
        '-i',
        '--input',
        help='input key file (.pem for ecdsa, .bin for aes)',
    )
    parser.add_argument(
        '--write-once',
        action='store_true',
        help='make key persistent - cannot be modified or deleted once written',
    )
    return parser.parse_args()


def main() -> None:
    args: Any = parse_args()

    key_type = KeyType[args.key_type.upper()]
    flags = Flags.NONE
    if args.write_once:
        flags |= Flags.WRITE_ONCE

    print(f'[+] Generating key of type: {key_type.name} (value: {key_type.value})')
    if args.input:
        print(f'[+] Using user-provided key file: {args.input}')
    if args.write_once:
        print('[+] WRITE_ONCE flag is set')

    key_data = generate_key_data(key_type, flags, args.input)

    with open(args.output, 'wb') as f:
        f.write(key_data)

    print(f'[✓] Key written to {args.output}')


if __name__ == '__main__':
    main()
