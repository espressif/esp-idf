#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import struct
from enum import Enum
from enum import IntFlag

import rich_click as click
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ec
from esp_pylib.logger import log

# === Constants ===
SEC_STG_KEY_DATA_SZ = 256
AES_KEY_LEN = 32
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
    else:
        key = os.urandom(AES_KEY_LEN)

    packed = struct.pack('<II32s', KeyType.AES256.value, flags.value, key)
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


@click.command(context_settings=dict(help_option_names=['-h', '--help']))
@click.option(
    '-k',
    '--key-type',
    'key_type',
    type=click.Choice([e.name.lower() for e in KeyType], case_sensitive=False),
    required=True,
    help='key type to be processed',
)
@click.option(
    '-o',
    '--output',
    required=True,
    help='output binary file name',
)
@click.option(
    '-i',
    '--input',
    'input_file',
    default=None,
    help='input key file (.pem for ecdsa, .bin for aes)',
)
@click.option(
    '--write-once',
    is_flag=True,
    default=False,
    help='make key persistent - cannot be modified or deleted once written',
)
def main(key_type: str, output: str, input_file: str | None, write_once: bool) -> None:
    """Generate or import a cryptographic key structure for secure storage."""
    selected_type = KeyType[key_type.upper()]
    flags = Flags.NONE
    if write_once:
        flags |= Flags.WRITE_ONCE

    log.print(
        f'[+] Generating key of type: {selected_type.name} (value: {selected_type.value})',
        markup=False,
        soft_wrap=True,
    )
    if input_file:
        log.print(f'[+] Using user-provided key file: {input_file}', markup=False, soft_wrap=True)
    if write_once:
        log.print('[+] WRITE_ONCE flag is set', markup=False, soft_wrap=True)

    try:
        key_data = generate_key_data(selected_type, flags, input_file)
        with open(output, 'wb') as f:
            f.write(key_data)
    except (ValueError, OSError) as e:
        raise click.ClickException(str(e))

    log.print(f'[✓] Key written to {output}', markup=False, soft_wrap=True)


if __name__ == '__main__':
    from esp_pylib.excepthook import install_exception_reporting

    install_exception_reporting()
    main()
