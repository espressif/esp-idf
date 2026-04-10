#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
"""
Generate test data for the Key Manager Signing Example.

This script generates all the cryptographic test data needed for the example:
1. Key Manager AES mode deployment parameters (init_key, k2_info)
2. ECDSA key material (k1_ecdsa256_encrypt)
3. DS key material and encrypted parameters (k1_ds_aes_key_encrypt, ds_iv, ds_c)

The generated data can be used to replace the hardcoded values in key_mgr_sign_main.c

When keys are generated (not loaded from files), they are automatically saved to PEM files
in the same directory as the output header file:
- ecdsa_private_key.pem: ECDSA P-256 private key
- rsa_private_key.pem: RSA private key (size depends on target)

Target-specific RSA key sizes:
- ESP32-C5: 3072-bit (SOC_RSA_MAX_BIT_LEN = 3072)
- ESP32-P4: 4096-bit (SOC_RSA_MAX_BIT_LEN = 4096)

Usage:
    python generate_test_data.py [--output-header FILENAME] [--target TARGET]

Options:
    --output-header FILENAME   Generate a C header file instead of printing to stdout
    --seed SEED                Random seed for reproducible output (default: random)
    --target TARGET            Target chip (esp32c5, esp32p4). Determines RSA key size.
                               Default: esp32c5 (3072-bit RSA)
    --ecdsa-key FILE           Path to ECDSA private key PEM file to load (P-256 curve)
    --rsa-key FILE             Path to RSA private key PEM file to load (must match target)

Examples:
    # Generate for ESP32-C5 (3072-bit RSA, default)
    python generate_test_data.py --seed 12345 --output-header main/test_data.h

    # Generate for ESP32-P4 (4096-bit RSA)
    python generate_test_data.py --target esp32p4 --seed 12345 --output-header main/test_data.h

    # Use existing ECDSA key from PEM file (only RSA key will be generated and saved)
    python generate_test_data.py --ecdsa-key my_ecdsa_key.pem --output-header main/test_data.h

    # Use existing RSA key from PEM file (only ECDSA key will be generated and saved)
    python generate_test_data.py --rsa-key my_rsa_key.pem --output-header main/test_data.h

    # Use both keys from PEM files (no keys will be generated or saved)
    python generate_test_data.py --ecdsa-key ecdsa.pem --rsa-key rsa.pem --output-header main/test_data.h
"""

import argparse
import hashlib
import hmac
import os
import struct
from collections.abc import Callable

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives.asymmetric.rsa import _modinv as modinv  # type: ignore
from cryptography.hazmat.primitives.ciphers import Cipher
from cryptography.hazmat.primitives.ciphers import algorithms
from cryptography.hazmat.primitives.ciphers import modes
from cryptography.utils import int_to_bytes

# Constants matching ESP-IDF Key Manager
KEY_MGR_K1_ENCRYPTED_SIZE = 32
KEY_MGR_K2_INFO_SIZE = 64
KEY_MGR_SW_INIT_KEY_SIZE = 32

# DS peripheral constants
ESP_DS_IV_LEN = 16

# Target-specific RSA key sizes (SOC_RSA_MAX_BIT_LEN)
# These values match the SOC capabilities defined in soc_caps.h
TARGET_RSA_MAX_BIT_LEN: dict[str, int] = {
    'esp32c5': 3072,
    'esp32p4': 4096,
}

# Default target and RSA key size
DEFAULT_TARGET = 'esp32c5'
DEFAULT_RSA_KEY_SIZE = TARGET_RSA_MAX_BIT_LEN[DEFAULT_TARGET]


def get_rsa_key_size_for_target(target: str) -> int:
    """Get the maximum RSA key size for a given target."""
    target_lower = target.lower()
    if target_lower not in TARGET_RSA_MAX_BIT_LEN:
        raise ValueError(f"Unknown target '{target}'. Supported targets: {', '.join(TARGET_RSA_MAX_BIT_LEN.keys())}")
    return TARGET_RSA_MAX_BIT_LEN[target_lower]


# ESP_DS_C_LEN calculation:
# (max_key_size * 3 + 256 (MD) + 32 (M_prime) + 32 (length) + 64 (padding)) / 8
def calculate_ds_c_len(max_key_size: int) -> int:
    return (max_key_size * 3 + 256 + 32 + 32 + 64) // 8


def bytes_to_c_array(data: bytes, name: str = '', indent: int = 4) -> str:
    """Convert bytes to C array format."""
    indent_str = ' ' * indent

    # Format with line breaks for readability
    values = [f'0x{b:02x}' for b in data]
    lines = []
    for i in range(0, len(values), 8):
        lines.append(indent_str + ', '.join(values[i : i + 8]) + ',')

    if name:
        return f'static const uint8_t {name}[] = {{\n' + '\n'.join(lines) + '\n};'
    return '{\n' + '\n'.join(lines) + '\n}'


def bytes_to_c_array_oneline(data: bytes) -> str:
    """Convert bytes to single-line C array format."""
    return '{ ' + ', '.join(f'0x{b:02x}' for b in data) + ' }'


def calculate_aes_ecb_encrypt(data: bytes, key: bytes) -> bytes:
    """Encrypt data using AES-ECB mode."""
    cipher = Cipher(algorithms.AES(key), modes.ECB(), backend=default_backend())
    encryptor = cipher.encryptor()
    result: bytes = encryptor.update(data) + encryptor.finalize()
    return result


def calculate_aes_cbc_encrypt(data: bytes, key: bytes, iv: bytes) -> bytes:
    """Encrypt data using AES-CBC mode."""
    cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())
    encryptor = cipher.encryptor()
    result: bytes = encryptor.update(data) + encryptor.finalize()
    return result


def number_as_bytes_le(number: int, pad_bits: int = 0) -> bytes:
    """Convert number to little-endian bytes with optional padding."""
    result: bytes = int_to_bytes(number)[::-1]  # Convert to little endian
    while pad_bits != 0 and len(result) < (pad_bits // 8):
        result += b'\x00'
    return result


def load_ecdsa_private_key(pem_file: str) -> bytes:
    """
    Load ECDSA private key from PEM file and return the raw private key bytes.

    Args:
        pem_file: Path to the PEM file containing an ECDSA P-256 private key

    Returns:
        32-byte raw private key value (big-endian)

    Raises:
        ValueError: If the key is not a P-256 ECDSA key
    """
    with open(pem_file, 'rb') as f:
        pem_data = f.read()

    private_key = serialization.load_pem_private_key(pem_data, password=None, backend=default_backend())

    if not isinstance(private_key, ec.EllipticCurvePrivateKey):
        raise ValueError(f'Expected ECDSA private key, got {type(private_key).__name__}')

    if not isinstance(private_key.curve, ec.SECP256R1):
        raise ValueError(f'Expected P-256 (secp256r1) curve, got {private_key.curve.name}')

    # Extract the raw private key value (32 bytes for P-256)
    private_numbers = private_key.private_numbers()
    private_value = private_numbers.private_value

    # Convert to 32-byte big-endian representation
    result: bytes = int_to_bytes(private_value).rjust(32, b'\x00')
    return result


def load_rsa_private_key(pem_file: str, expected_key_size: int) -> rsa.RSAPrivateKey:
    """
    Load RSA private key from PEM file.

    Args:
        pem_file: Path to the PEM file containing an RSA private key
        expected_key_size: Expected RSA key size in bits (e.g., 3072, 4096)

    Returns:
        RSA private key object

    Raises:
        ValueError: If the key is not an RSA key or has wrong size
    """
    with open(pem_file, 'rb') as f:
        pem_data = f.read()

    private_key = serialization.load_pem_private_key(pem_data, password=None, backend=default_backend())

    if not isinstance(private_key, rsa.RSAPrivateKey):
        raise ValueError(f'Expected RSA private key, got {type(private_key).__name__}')

    if private_key.key_size != expected_key_size:
        raise ValueError(f'Expected {expected_key_size}-bit RSA key, got {private_key.key_size}-bit')

    return private_key


def save_ecdsa_private_key(private_value: bytes, pem_file: str) -> None:
    """
    Save ECDSA private key to PEM file.

    Args:
        private_value: 32-byte raw private key value (big-endian)
        pem_file: Path to save the PEM file
    """
    # Convert raw bytes to integer
    private_int = int.from_bytes(private_value, byteorder='big')

    # Derive the public key from the private key
    private_key = ec.derive_private_key(private_int, ec.SECP256R1(), default_backend())

    # Serialize to PEM format
    pem_data = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.TraditionalOpenSSL,
        encryption_algorithm=serialization.NoEncryption(),
    )

    with open(pem_file, 'wb') as f:
        f.write(pem_data)

    print(f'Saved ECDSA key to: {pem_file}')


def save_rsa_private_key(private_key: rsa.RSAPrivateKey, pem_file: str) -> None:
    """
    Save RSA private key to PEM file.

    Args:
        private_key: RSA private key object
        pem_file: Path to save the PEM file
    """
    pem_data = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.TraditionalOpenSSL,
        encryption_algorithm=serialization.NoEncryption(),
    )

    with open(pem_file, 'wb') as f:
        f.write(pem_data)

    print(f'Saved RSA key to: {pem_file}')


def generate_key_manager_params(init_key: bytes, k2: bytes, rand_num: bytes) -> bytes:
    """
    Generate K2 info for Key Manager AES mode deployment.

    The K2 info is computed as:
    k2_info = AES_ECB(init_key, AES_ECB(rand_num, k2) || rand_num)
    """
    temp_result_inner = calculate_aes_ecb_encrypt(k2, rand_num)
    k2_info = calculate_aes_ecb_encrypt(temp_result_inner + rand_num, init_key)
    return k2_info


def generate_k1_encrypted(k1: bytes, k2: bytes) -> bytes:
    """
    Generate encrypted K1 for Key Manager.

    K1 is reversed and then encrypted with K2 using AES-ECB.
    """
    k1_reversed = k1[::-1]
    return calculate_aes_ecb_encrypt(k1_reversed, k2)


def generate_ds_aes_key(hmac_key: bytes) -> bytes:
    """
    Generate the AES key used for DS parameter encryption.

    The DS AES key is derived from HMAC key: HMAC-SHA256(hmac_key, 0xFF * 32)
    """
    return hmac.HMAC(hmac_key, b'\xff' * 32, hashlib.sha256).digest()


def generate_ds_encrypted_params(private_key: rsa.RSAPrivateKey, aes_key: bytes, iv: bytes, max_key_size: int) -> bytes:
    """
    Generate encrypted DS parameters (the 'C' blob).

    This follows the ESP-IDF DS parameter encryption format.
    """
    priv_numbers = private_key.private_numbers()
    pub_numbers = private_key.public_key().public_numbers()

    Y = priv_numbers.d  # Private exponent
    M = pub_numbers.n  # Modulus
    key_size = private_key.key_size

    # Calculate RSA helper values
    rr = 1 << (key_size * 2)
    rinv = rr % M
    mprime = -modinv(M, 1 << 32) & 0xFFFFFFFF
    length = key_size // 32 - 1

    # Calculate MD (message digest) from parameters and IV
    md_in = (
        number_as_bytes_le(Y, max_key_size)
        + number_as_bytes_le(M, max_key_size)
        + number_as_bytes_le(rinv, max_key_size)
        + struct.pack('<II', mprime, length)
        + iv
    )
    md = hashlib.sha256(md_in).digest()

    # Generate plaintext P for encryption
    p = (
        number_as_bytes_le(Y, max_key_size)
        + number_as_bytes_le(M, max_key_size)
        + number_as_bytes_le(rinv, max_key_size)
        + md
        + struct.pack('<II', mprime, length)
        + b'\x08' * 8  # Padding
    )

    # Encrypt P to get C
    c = calculate_aes_cbc_encrypt(p, aes_key, iv)
    return c


def generate_test_data(
    seed: int | None = None,
    target: str = DEFAULT_TARGET,
    ecdsa_key_file: str | None = None,
    rsa_key_file: str | None = None,
    save_ecdsa_key_file: str | None = None,
    save_rsa_key_file: str | None = None,
) -> dict:
    """
    Generate all test data for the Key Manager signing example.

    Args:
        seed: Random seed for reproducible output (ignored for keys loaded from files)
        target: Target chip name (e.g., 'esp32c5', 'esp32p4'). Determines RSA key size.
        ecdsa_key_file: Path to ECDSA private key PEM file to load (P-256 curve)
        rsa_key_file: Path to RSA private key PEM file to load (must match target's max RSA size)
        save_ecdsa_key_file: Path to save generated ECDSA key as PEM file
        save_rsa_key_file: Path to save generated RSA key as PEM file

    Returns:
        Dictionary containing all test data arrays
    """
    # Get target-specific RSA key size
    rsa_key_size = get_rsa_key_size_for_target(target)
    print(f'Target: {target} (RSA key size: {rsa_key_size}-bit)')

    urandom: Callable[[int], bytes]
    if seed is not None:
        import random

        random.seed(seed)

        # Use seeded random for reproducible output
        def seeded_urandom(n: int) -> bytes:
            return bytes(random.randint(0, 255) for _ in range(n))

        urandom = seeded_urandom
    else:
        urandom = os.urandom

    # Generate base keys for Key Manager AES mode
    init_key = urandom(KEY_MGR_SW_INIT_KEY_SIZE)
    k2 = urandom(32)
    rand_num = urandom(32)

    # Generate K2 info
    k2_info = generate_key_manager_params(init_key, k2, rand_num)

    # ECDSA key: load from PEM file or generate randomly
    ecdsa_key_generated = False
    if ecdsa_key_file:
        print(f'Loading ECDSA key from: {ecdsa_key_file}')
        k1_ecdsa = load_ecdsa_private_key(ecdsa_key_file)
    else:
        # Generate random ECDSA key (P-256, 32 bytes)
        k1_ecdsa = urandom(32)
        ecdsa_key_generated = True
    k1_ecdsa256_encrypt = generate_k1_encrypted(k1_ecdsa, k2)

    # Save ECDSA key if generated (not loaded from file)
    if save_ecdsa_key_file and ecdsa_key_generated:
        save_ecdsa_private_key(k1_ecdsa, save_ecdsa_key_file)

    # Generate DS HMAC key and derive AES key
    hmac_key_for_ds = urandom(32)
    ds_aes_key = generate_ds_aes_key(hmac_key_for_ds)

    # Generate K1 for DS (derived from HMAC key)
    k1_ds = generate_ds_aes_key(hmac_key_for_ds)
    k1_ds_encrypt = generate_k1_encrypted(k1_ds, k2)

    # RSA key for DS: load from PEM file or generate randomly
    rsa_key_generated = False
    if rsa_key_file:
        print(f'Loading RSA key from: {rsa_key_file}')
        rsa_private_key = load_rsa_private_key(rsa_key_file, rsa_key_size)
    else:
        # Generate random RSA key with target-specific size
        rsa_private_key = rsa.generate_private_key(
            public_exponent=65537, key_size=rsa_key_size, backend=default_backend()
        )
        rsa_key_generated = True

    # Save RSA key if generated (not loaded from file)
    if save_rsa_key_file and rsa_key_generated:
        save_rsa_private_key(rsa_private_key, save_rsa_key_file)

    # Generate DS IV and encrypted parameters using target's max key size
    ds_iv = urandom(ESP_DS_IV_LEN)
    ds_c = generate_ds_encrypted_params(rsa_private_key, ds_aes_key, ds_iv, rsa_key_size)

    # Generate a sample SHA-256 hash to sign
    sha256_hash = urandom(32)

    return {
        'init_key': init_key,
        'k2_info': k2_info,
        'k1_ecdsa256_encrypt': k1_ecdsa256_encrypt,
        'k1_ds_aes_key_encrypt': k1_ds_encrypt,
        'ds_iv': ds_iv,
        'ds_c': ds_c,
        'sha256_hash': sha256_hash,
        # Keep these for reference/debugging
        '_target': target,
        '_rsa_key_size': rsa_key_size,
        '_k2': k2,
        '_rand_num': rand_num,
        '_k1_ecdsa': k1_ecdsa,
        '_hmac_key_for_ds': hmac_key_for_ds,
        '_ecdsa_key_file': ecdsa_key_file,
        '_rsa_key_file': rsa_key_file,
        '_save_ecdsa_key_file': save_ecdsa_key_file if ecdsa_key_generated else None,
        '_save_rsa_key_file': save_rsa_key_file if rsa_key_generated else None,
    }


def print_test_data(data: dict) -> None:
    """Print test data in a format suitable for copying into C code."""
    target = data.get('_target', DEFAULT_TARGET)
    rsa_key_size = data.get('_rsa_key_size', DEFAULT_RSA_KEY_SIZE)

    print('/*')
    print(' * Auto-generated test data for Key Manager Signing Example')
    print(' * Generated by generate_test_data.py')
    print(f' * Target: {target} (RSA key size: {rsa_key_size}-bit)')
    print(' */')
    print()

    print('/* SHA-256 hash to sign */')
    print(bytes_to_c_array(data['sha256_hash'], 'sha256_hash'))
    print()

    print('/* Software init key for Key Manager AES mode deployment */')
    print(bytes_to_c_array(data['init_key'], 'init_key'))
    print()

    print('/* K2 info for Key Manager AES mode deployment */')
    print(bytes_to_c_array(data['k2_info'], 'k2_info'))
    print()

    print('#if CONFIG_EXAMPLE_KEY_MGR_ECDSA_SIGN')
    print('/* K1 encrypted for ECDSA P-256 key */')
    print(bytes_to_c_array(data['k1_ecdsa256_encrypt'], 'k1_ecdsa256_encrypt'))
    print('#endif /* CONFIG_EXAMPLE_KEY_MGR_ECDSA_SIGN */')
    print()

    print('#if CONFIG_EXAMPLE_KEY_MGR_RSA_DS_SIGN')
    print(f'/* K1 encrypted for DS (AES) key - RSA {rsa_key_size}-bit */')
    print(bytes_to_c_array(data['k1_ds_aes_key_encrypt'], 'k1_ds_aes_key_encrypt'))
    print()

    print(f'static const uint8_t ds_iv[{ESP_DS_IV_LEN}] = {bytes_to_c_array_oneline(data["ds_iv"])};')
    print(f'static const uint8_t ds_c[{len(data["ds_c"])}] = {bytes_to_c_array_oneline(data["ds_c"])};')
    print('#endif /* CONFIG_EXAMPLE_KEY_MGR_RSA_DS_SIGN */')


def generate_header_file(data: dict, filename: str, seed: int | None = None) -> None:
    """Generate a C header file with the test data."""
    target = data.get('_target', DEFAULT_TARGET)
    rsa_key_size = data.get('_rsa_key_size', DEFAULT_RSA_KEY_SIZE)

    with open(filename, 'w') as f:
        f.write('/*\n')
        f.write(' * Auto-generated test data for Key Manager Signing Example\n')
        f.write(' * Generated by generate_test_data.py\n')
        f.write(' *\n')
        # Add generation parameters info
        gen_info = []
        gen_info.append(f'target: {target}')
        gen_info.append(f'RSA key size: {rsa_key_size}-bit')
        if seed is not None:
            gen_info.append(f'seed: {seed}')
        if data.get('_ecdsa_key_file'):
            gen_info.append(f'ECDSA key loaded from: {data["_ecdsa_key_file"]}')
        elif data.get('_save_ecdsa_key_file'):
            gen_info.append(f'ECDSA key saved to: {data["_save_ecdsa_key_file"]}')
        if data.get('_rsa_key_file'):
            gen_info.append(f'RSA key loaded from: {data["_rsa_key_file"]}')
        elif data.get('_save_rsa_key_file'):
            gen_info.append(f'RSA key saved to: {data["_save_rsa_key_file"]}')
        if gen_info:
            f.write(' * Generation parameters:\n')
            for info in gen_info:
                f.write(f' *   {info}\n')
        f.write(' *\n')
        f.write(' * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD\n')
        f.write(' * SPDX-License-Identifier: Unlicense OR CC0-1.0\n')
        f.write(' */\n\n')
        f.write('#pragma once\n\n')
        f.write('#include <stdint.h>\n')
        f.write('#include "sdkconfig.h"\n')
        f.write('#include "esp_ds.h"\n\n')

        f.write('/* SHA-256 hash to sign */\n')
        f.write(bytes_to_c_array(data['sha256_hash'], 'sha256_hash') + '\n\n')

        f.write('/* Software init key for Key Manager AES mode deployment */\n')
        f.write(bytes_to_c_array(data['init_key'], 'init_key') + '\n\n')

        f.write('/* K2 info for Key Manager AES mode deployment */\n')
        f.write(bytes_to_c_array(data['k2_info'], 'k2_info') + '\n\n')

        f.write('#if CONFIG_EXAMPLE_KEY_MGR_ECDSA_SIGN\n')
        f.write('/* K1 encrypted for ECDSA P-256 key */\n')
        f.write(bytes_to_c_array(data['k1_ecdsa256_encrypt'], 'k1_ecdsa256_encrypt') + '\n')
        f.write('#endif /* CONFIG_EXAMPLE_KEY_MGR_ECDSA_SIGN */\n\n')

        f.write('#if CONFIG_EXAMPLE_KEY_MGR_RSA_DS_SIGN\n')
        f.write(f'/* K1 encrypted for DS (AES) key - RSA {rsa_key_size}-bit */\n')
        f.write(bytes_to_c_array(data['k1_ds_aes_key_encrypt'], 'k1_ds_aes_key_encrypt') + '\n\n')

        f.write(f'static const uint8_t ds_iv[ESP_DS_IV_LEN] = {bytes_to_c_array_oneline(data["ds_iv"])};\n')
        f.write(f'static const uint8_t ds_c[ESP_DS_C_LEN] = {bytes_to_c_array_oneline(data["ds_c"])};\n')
        f.write('#endif /* CONFIG_EXAMPLE_KEY_MGR_RSA_DS_SIGN */\n')

    print(f'Generated header file: {filename}')


def main() -> None:
    parser = argparse.ArgumentParser(
        description='Generate test data for the Key Manager Signing Example',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument(
        '--output-header', type=str, metavar='FILENAME', help='Generate a C header file with the test data'
    )
    parser.add_argument('--seed', type=int, default=None, help='Random seed for reproducible output')
    parser.add_argument(
        '--target',
        type=str,
        default=DEFAULT_TARGET,
        choices=list(TARGET_RSA_MAX_BIT_LEN.keys()),
        help=f'Target chip (determines RSA key size). Default: {DEFAULT_TARGET}',
    )
    parser.add_argument(
        '--ecdsa-key',
        type=str,
        metavar='FILE',
        help='Path to ECDSA private key PEM file to load (P-256/secp256r1 curve)',
    )
    parser.add_argument(
        '--rsa-key',
        type=str,
        metavar='FILE',
        help='Path to RSA private key PEM file to load (must match target RSA size)',
    )

    args = parser.parse_args()

    # Determine output directory for generated key files
    if args.output_header:
        output_dir = os.path.dirname(args.output_header)
        if not output_dir:
            output_dir = '.'
    else:
        output_dir = '.'

    # Set default paths for saving generated keys
    save_ecdsa_key = None if args.ecdsa_key else os.path.join(output_dir, 'ecdsa_private_key.pem')
    save_rsa_key = None if args.rsa_key else os.path.join(output_dir, 'rsa_private_key.pem')

    data = generate_test_data(
        seed=args.seed,
        target=args.target,
        ecdsa_key_file=args.ecdsa_key,
        rsa_key_file=args.rsa_key,
        save_ecdsa_key_file=save_ecdsa_key,
        save_rsa_key_file=save_rsa_key,
    )

    if args.output_header:
        generate_header_file(data, args.output_header, seed=args.seed)
    else:
        print_test_data(data)


if __name__ == '__main__':
    main()
