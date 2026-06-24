# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import binascii
import os
import subprocess
from typing import Any

import pytest
from cryptography import exceptions
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives.asymmetric import utils
from cryptography.hazmat.primitives.asymmetric.ec import SECP256R1
from cryptography.hazmat.primitives.serialization import load_pem_private_key
from ecdsa import NIST256p
from ecdsa.ellipticcurve import Point
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


def load_ecdsa_key(filename: str) -> SECP256R1:
    with open(filename, 'rb') as key_file:
        return load_pem_private_key(key_file.read(), password=None, backend=default_backend())


def test_xts_aes_encryption(negotiated_key: bytes, plaintext_data: bytes, encrypted_data: bytes) -> None:
    with open('test/negotiated_key.bin', 'wb+') as key_file:
        key_file.write(negotiated_key)

    with open('test/plaintext.bin', 'wb+') as plaintext_file:
        plaintext_file.write(plaintext_data)

    command = [
        'espsecure',
        'encrypt-flash-data',
        '--aes-xts',
        '--keyfile',
        'test/negotiated_key.bin',
        '--address',
        '0x120000',
        '--output',
        'test/enc-data.bin',
        'test/plaintext.bin',
    ]
    result = subprocess.run(command, capture_output=True, text=True)
    assert result.returncode == 0, f'Command failed with error: {result.stderr}'

    with open('test/enc-data.bin', 'rb') as enc_file:
        calculated_enc_data = enc_file.read()

    assert calculated_enc_data == encrypted_data, 'Calculated data does not match encrypted data obtained from firmware'


def calculate_key_manager_ecdh0_negotiated_key(k2_G_hex: str, k1_ecdsa_key: str) -> Any:
    k2_G_bytes_le = binascii.unhexlify(k2_G_hex)

    k2_G_bytes_x_be = bytes(reversed(k2_G_bytes_le[:32]))
    k2_G_bytes_y_be = bytes(reversed(k2_G_bytes_le[32:]))

    k2_G_bytes_be = k2_G_bytes_x_be + k2_G_bytes_y_be

    curve = NIST256p.curve
    k2_G = Point.from_bytes(curve, k2_G_bytes_be)

    # Load the ECDSA private key (k1)
    k1_key = load_ecdsa_key(k1_ecdsa_key)
    k1_int = k1_key.private_numbers().private_value

    # Convert the integer to bytes in big endian format
    k1_bytes_big_endian = k1_int.to_bytes((k1_int.bit_length() + 7) // 8, byteorder='big')

    # Reverse the bytes to get little endian format
    k1_bytes_little_endian = k1_bytes_big_endian[::-1]

    k1_int = int.from_bytes(k1_bytes_little_endian, byteorder='little')

    # Calculate k1*k2*G
    k1_k2_G = k1_int * k2_G

    # Extract the x-coordinate of the result and save it as the shared secret
    negotiated_key = k1_k2_G.to_bytes()[:32]
    return negotiated_key


def test_ecdsa_key(
    negotiated_key: bytes, digest: bytes, signature_r_le: bytes, signature_s_le: bytes, pubx: bytes, puby: bytes
) -> None:
    r = int.from_bytes(signature_r_le, 'little')
    s = int.from_bytes(signature_s_le, 'little')
    signature = utils.encode_dss_signature(r, s)
    pubx_int = int.from_bytes(pubx, 'little')
    puby_int = int.from_bytes(puby, 'little')
    private_number = int.from_bytes(negotiated_key, byteorder='big')
    ecdsa_private_key = ec.derive_private_key(private_number, ec.SECP256R1())
    # Get the public key
    public_key = ecdsa_private_key.public_key()
    # Extract the pubx and puby values
    calc_pubx, calc_puby = public_key.public_numbers().x, public_key.public_numbers().y

    assert calc_pubx == pubx_int, 'Public key calculated should match with public key obtained'
    assert calc_puby == puby_int, 'Public key calculated should match with public key obtained'

    try:
        public_key.verify(signature, digest, ec.ECDSA(utils.Prehashed(hashes.SHA256())))
        print('Valid signature')
    except exceptions.InvalidSignature:
        print('Invalid signature')
        raise


def test_crypto_long_aes_operations(dut: Dut) -> None:
    # if the env variable IDF_FPGA_ENV is set, we would need a longer timeout
    # as tests for efuses burning security peripherals would be run
    timeout = 600 if os.environ.get('IDF_ENV_FPGA') else 60

    dut.expect('Tests finished', timeout=timeout)


@pytest.mark.generic
@pytest.mark.parametrize('config', ['long_aes_operations'], indirect=True)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_crypto_long_aes_operations_generic(dut: Dut) -> None:
    test_crypto_long_aes_operations(dut)


@pytest.mark.generic
@pytest.mark.esp32p4_rev1
@pytest.mark.parametrize('config', ['long_aes_operations_esp32p4_rev1'], indirect=True)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_crypto_long_aes_operations_esp32p4_rev1(dut: Dut) -> None:
    test_crypto_long_aes_operations(dut)
