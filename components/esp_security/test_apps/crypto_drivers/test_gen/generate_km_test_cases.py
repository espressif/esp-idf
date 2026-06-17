# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import hashlib
import hmac
import os
from typing import Any

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.ciphers import Cipher
from cryptography.hazmat.primitives.ciphers import algorithms
from cryptography.hazmat.primitives.ciphers import modes


def key_to_c_format(key: bytes) -> str:
    return ', '.join([f'0x{byte:02x}' for byte in key])


def calculate_aes_cipher(data: bytes, key: bytes) -> Any:
    cipher = Cipher(algorithms.AES(key), modes.ECB(), backend=default_backend())
    encryptor = cipher.encryptor()
    return encryptor.update(data) + encryptor.finalize()


# Helper functions
def generate_random_key(size: int = 32) -> bytes:
    return os.urandom(size)


def save_key_to_file(key: bytes, filename: str) -> None:
    with open(filename, 'wb') as file:
        file.write(key)


def key_from_file_or_generate(filename: str, size: int = 32) -> bytes:
    if not os.path.exists(filename):
        key = generate_random_key(size)
        save_key_to_file(key, filename)

    with open(filename, 'rb') as file:
        return file.read()


init_key = key_from_file_or_generate('init_key.bin', 32)
k2 = key_from_file_or_generate('k2.bin', 32)
rand_num = key_from_file_or_generate('rand_num.bin', 32)

temp_result_inner = calculate_aes_cipher(k2, rand_num)
temp_result_outer = calculate_aes_cipher(temp_result_inner + rand_num, init_key)
k2_info = temp_result_outer

print(f'init_key = {key_to_c_format(init_key)}')
print(f'k2_info = {key_to_c_format(k2_info)}')

# XTS-AES key
k1_xts_aes = key_from_file_or_generate('k1_xts_aes.bin', 32)
k1_xts_aes_reversed = k1_xts_aes[::-1]
k1_xts_aes_encrypted_reversed = calculate_aes_cipher(k1_xts_aes_reversed, k2)
print(f'k1_xts_aes_encrypt = {key_to_c_format(k1_xts_aes_encrypted_reversed)}')

# HMAC Key
# Key from hmac_test_cases.h
# k1_hmac_reversed = bytes([
#     1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
#     17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32
# ])
k1_hmac_reversed = bytes([])
k1_hmac_encrypted_reversed = calculate_aes_cipher(k1_hmac_reversed, k2)
print(f'k1_hmac_encrypt = {key_to_c_format(k1_hmac_encrypted_reversed)}')

# Digital Signature
# Key DS_KEY_IDX from digital_signature_test_cases_<bits>.h
# hmac_key_for_ds = bytes([
#     0x3d, 0x71, 0xd5, 0xc7, 0xe0, 0x15, 0xe6, 0x30, 0x66, 0xe8, 0xee, 0x5d, 0x14, 0x1e, 0xe1, 0xa1,
#     0x03, 0xd9, 0x7d, 0x48, 0x16, 0xde, 0xbc, 0xba, 0xaa, 0xf4, 0x3e, 0xd5, 0xb2, 0xbc, 0x46, 0xbc
# ])
hmac_key_for_ds = bytes([])
k1_ds = hmac.HMAC(hmac_key_for_ds, b'\xff' * 32, hashlib.sha256).digest()
k1_ds_reversed = k1_ds[::-1]
k1_ds_encrypted_reversed = calculate_aes_cipher(k1_ds_reversed, k2)
print(f'k1_ds_encrypt = {key_to_c_format(k1_ds_encrypted_reversed)}')
