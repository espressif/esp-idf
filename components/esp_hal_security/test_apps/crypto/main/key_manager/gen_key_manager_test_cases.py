# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import hashlib
import hmac
import os
import random
import struct
from typing import Any

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives.ciphers import Cipher
from cryptography.hazmat.primitives.ciphers import algorithms
from cryptography.hazmat.primitives.ciphers import modes
from cryptography.utils import int_to_bytes
from ecdsa.curves import NIST256p

# Constants
TEST_COUNT = 5
STORAGE_PARTITION_OFFSET = 0x160000


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


def key_to_c_format(key: bytes) -> str:
    return ', '.join([f'0x{byte:02x}' for byte in key])


def calculate_aes_cipher(data: bytes, key: bytes) -> Any:
    cipher = Cipher(algorithms.AES(key), modes.ECB(), backend=default_backend())
    encryptor = cipher.encryptor()
    return encryptor.update(data) + encryptor.finalize()


def _flash_encryption_operation_aes_xts(
    input_data: bytes, flash_address: int, key: bytes, do_decrypt: bool = False
) -> bytes:
    backend = default_backend()

    indata = input_data

    pad_left = flash_address % 0x80
    indata = (b'\x00' * pad_left) + indata

    pad_right = (0x80 - (len(indata) % 0x80)) % 0x80
    indata += b'\x00' * pad_right

    inblocks = [indata[i : i + 0x80] for i in range(0, len(indata), 0x80)]

    output = b''
    for inblock in inblocks:
        tweak = struct.pack('<I', flash_address & ~0x7F) + (b'\x00' * 12)
        flash_address += 0x80
        cipher = Cipher(algorithms.AES(key), modes.XTS(tweak), backend=backend)
        encryptor = cipher.encryptor() if not do_decrypt else cipher.decryptor()

        outblock = encryptor.update(inblock[::-1])
        output += outblock[::-1]

    return output[pad_left : len(output) - pad_right]


def generate_xts_test_data(key: bytes, base_flash_address: int = STORAGE_PARTITION_OFFSET) -> list:
    xts_test_data = []
    plaintext_data = bytes(range(1, 129))
    data_size = 16
    flash_address = base_flash_address
    for i in range(TEST_COUNT):
        data_size = (data_size * 2) % 256
        if data_size < 16:
            data_size = 16
        input_data = plaintext_data[:data_size]
        flash_address = base_flash_address + (i * 0x100)

        ciphertext = _flash_encryption_operation_aes_xts(input_data, flash_address, key)
        xts_test_data.append((data_size, flash_address, ciphertext[:data_size]))
    return xts_test_data


def generate_ecdsa_key_and_pub_key(key: bytes, key_size: int) -> tuple:
    private_number = int.from_bytes(key, byteorder='big')

    if key_size == 192:
        curve = ec.SECP192R1()
    elif key_size == 256:
        curve = ec.SECP256R1()
    elif key_size == 384:
        curve = ec.SECP384R1()
    else:
        raise ValueError(f'Unsupported key size: {key_size}')

    private_key = ec.derive_private_key(private_number, curve)
    pem = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.TraditionalOpenSSL,
        encryption_algorithm=serialization.NoEncryption(),
    )

    with open(f'ecdsa_{key_size}_key.pem', 'wb') as pem_file:
        pem_file.write(pem)

    public_key = private_key.public_key()
    pub_numbers = public_key.public_numbers()
    pubx = pub_numbers.x.to_bytes(key_size // 8, byteorder='little')
    puby = pub_numbers.y.to_bytes(key_size // 8, byteorder='little')

    return pubx, puby


def perform_ecc_point_multiplication(k1_int: int) -> Any:
    generator = NIST256p.generator.to_affine()
    k1_G = k1_int * generator
    return k1_G


def generate_k1_G(k1_bytes: bytes) -> tuple:
    k1_G = []
    k1_int = int.from_bytes(k1_bytes, byteorder='big')
    k1_G_point = perform_ecc_point_multiplication(k1_int)
    k1_G = k1_G_point.to_bytes()[:64]

    k1_G = k1_G[::-1]
    k1_G_x = k1_G[:32]
    k1_G_y = k1_G[32:]
    k1_G = k1_G_y + k1_G_x

    return k1_G, k1_G


def generate_hmac_test_data(key: bytes) -> tuple:
    hmac_message = (
        b'Deleniti voluptas explicabo et assumenda. Sed et aliquid minus quis. '
        b'Praesentium cupiditate quia nemo est. Laboriosam pariatur ut distinctio tenetur. '
        b'Sunt architecto iure aspernatur soluta ut recusandae. '
        b'Ut quibusdam occaecati ut qui sit dignissimos eaque..'
    )
    hmac_result = hmac.HMAC(key, hmac_message, hashlib.sha256).digest()
    return hmac_message, hmac_result


def number_as_bytes(number, pad_bits=0):  # type: (int, int) -> bytes
    """
    Given a number, format as a little endian array of bytes
    """
    result = int_to_bytes(number)[::-1]  # type: bytes
    while pad_bits != 0 and len(result) < (pad_bits // 8):
        result += b'\x00'
    return result


def number_as_bignum_words(number):  # type: (int) -> str
    """
    Given a number, format result as a C array of words
    (little-endian, same as ESP32 RSA peripheral or mbedTLS)
    """
    result = []
    while number != 0:
        result.append('0x%08x' % (number & 0xFFFFFFFF))
        number >>= 32
    return '{ ' + ', '.join(result) + ' }'


def generate_ds_encrypted_input_params(aes_key: bytes, max_key_size: int, iv: bytes) -> tuple:
    private_key = rsa.generate_private_key(public_exponent=65537, key_size=max_key_size, backend=default_backend())
    priv_numbers = private_key.private_numbers()
    pub_numbers = private_key.public_key().public_numbers()
    Y = priv_numbers.d
    M = pub_numbers.n

    rr = 1 << (max_key_size * 2)
    rinv = rr % pub_numbers.n
    mprime = -rsa._modinv(M, 1 << 32)
    mprime &= 0xFFFFFFFF
    length = max_key_size // 32 - 1

    # calculate MD from preceding values and IV
    # Y_max_key_size || M_max_key_size || Rb_max_key_size || M_prime32 || LENGTH32 || IV128
    md_in = (
        number_as_bytes(Y, max_key_size)
        + number_as_bytes(M, max_key_size)
        + number_as_bytes(rinv, max_key_size)
        + struct.pack('<II', mprime, length)
        + iv
    )

    md = hashlib.sha256(md_in).digest()

    # generate expected C value from P bitstring
    #
    # Y_max_key_size || M_max_key_size || Rb_max_key_size || M_prime32 || LENGTH32 ||  0x08*8
    # E.g. for C3: Y3072 || M3072 || Rb3072 || M_prime32 || LENGTH32 || MD256 || 0x08*8
    p = (
        number_as_bytes(Y, max_key_size)
        + number_as_bytes(M, max_key_size)
        + number_as_bytes(rinv, max_key_size)
        + md
        + struct.pack('<II', mprime, length)
        + b'\x08' * 8
    )

    # expected_len = max_len_Y + max_len_M + max_len_rinv + md (32 bytes)
    #               + (mprime + length packed (8bytes)) + padding (8 bytes)
    expected_len = (max_key_size / 8) * 3 + 32 + 8 + 8
    assert len(p) == expected_len

    cipher = Cipher(algorithms.AES(aes_key), modes.CBC(iv), backend=default_backend())
    encryptor = cipher.encryptor()
    ds_encrypted_input_params = encryptor.update(p) + encryptor.finalize()

    mask = (1 << max_key_size) - 1  # truncate messages if needed

    ds_message = random.randrange(0, 1 << max_key_size)
    ds_result = number_as_bytes(pow(ds_message & mask, Y, M))

    return number_as_bytes(ds_message), ds_encrypted_input_params, ds_result


def write_to_c_header(
    init_key: bytes,
    k1: bytes,
    k2_info: bytes,
    k1_encrypted_24: list,
    k1_encrypted_24_reversed: list,
    k1_encrypted_32: list,
    k1_encrypted_32_reversed: list,
    k1_encrypted_48: list,
    k1_encrypted_48_reversed: list,
    test_data_xts_aes_128: list,
    k1_encrypted_64: list,
    k1_encrypted_64_reversed: list,
    xts_test_data_xts_aes_256: list,
    ecdsa_p192_pubx: bytes,
    ecdsa_p192_puby: bytes,
    ecdsa_p256_pubx: bytes,
    ecdsa_p256_puby: bytes,
    ecdsa_p384_pubx: bytes,
    ecdsa_p384_puby: bytes,
    k1_G_0: bytes,
    k1_G_1: bytes,
    hmac_message: bytes,
    hmac_result: bytes,
    ds_message_4096: bytes,
    ds_encrypted_input_params_4096: bytes,
    ds_result_4096: bytes,
    ds_message_3072: bytes,
    ds_encrypted_input_params_3072: bytes,
    ds_result_3072: bytes,
    ds_iv: bytes,
) -> None:
    with open('key_manager_test_cases.h', 'w', encoding='utf-8') as file:
        header_content = f"""#include <stdint.h>
#include "soc/soc_caps.h"

#define TEST_COUNT 5

typedef struct test_xts_data {{
    uint16_t data_size;
    uint32_t data_offset;
    uint8_t ciphertext[128];
}} test_xts_data_t;

typedef struct test_ecdsa_data {{
    uint8_t ecdsa_p192_pubx[24];
    uint8_t ecdsa_p192_puby[24];
    uint8_t ecdsa_p256_pubx[32];
    uint8_t ecdsa_p256_puby[32];
    uint8_t ecdsa_p384_pubx[48];
    uint8_t ecdsa_p384_puby[48];
}} test_ecdsa_data_t;

typedef struct test_hmac_data {{
    uint8_t message[{len(hmac_message)}];
    uint8_t hmac_result[32];
}} test_hmac_data_t;

typedef struct test_ds_data {{
#if SOC_RSA_MAX_BIT_LEN == 4096
    uint8_t ds_message[4096 / 8];
    uint8_t ds_encrypted_input_params[1584];
    uint8_t ds_result[4096 / 8];
#elif SOC_RSA_MAX_BIT_LEN == 3072
    uint8_t ds_message[3072 / 8];
    uint8_t ds_encrypted_input_params[1200];
    uint8_t ds_result[3072 / 8];
#endif
    size_t ds_key_size;
    uint8_t ds_encrypted_input_params_iv[16];
}} test_ds_data_t;

typedef struct test_data {{
    uint8_t init_key[32];
    uint8_t k2_info[64];
    // [0] for XTS-AES-128 / ECDSA-P192 / HMAC / DS, [1] for XTS-AES-256 / ECDSA-P256
    // [2] for ECDSA-P384-H, [3] for ECDSA-P384-L
    uint8_t k1_encrypted[4][32];
    uint8_t plaintext_data[128];
    union {{
        test_xts_data_t xts_test_data[TEST_COUNT];
        test_ecdsa_data_t ecdsa_test_data;
        test_hmac_data_t hmac_test_data;
        test_ds_data_t ds_test_data;
    }};
}} test_data_aes_mode_t;

typedef struct test_data_ecdh0 {{
    uint8_t plaintext_data[128];
    uint8_t k1[2][32];
    uint8_t k1_G[2][64];
}} test_data_ecdh0_mode_t;

// For 32-byte k1 key
test_data_aes_mode_t test_data_xts_aes_128 = {{
    .init_key = {{ {key_to_c_format(init_key)} }},
    .k2_info = {{ {key_to_c_format(k2_info)} }},
    .k1_encrypted = {{ {{ {key_to_c_format(k1_encrypted_32_reversed[0])} }}, {{  }} }},
    .plaintext_data = {{ {key_to_c_format(bytes(range(1, 129)))} }},
    .xts_test_data = {{
"""

        for data_size, flash_address, ciphertext in test_data_xts_aes_128:
            header_content += (
                f'\t\t{{.data_size = {data_size}, '
                f'.data_offset = 0x{flash_address:x}, '
                f'.ciphertext = {{{key_to_c_format(ciphertext)}}}}},\n'
            )
        header_content += '\t}\n};\n\n'

        # For 64-byte k1 key
        header_content += '// For 64-byte k1 key\n'
        header_content += 'test_data_aes_mode_t test_data_xts_aes_256 = {\n'
        header_content += f'\t.init_key = {{{key_to_c_format(init_key)}}},\n'
        header_content += f'\t.k2_info = {{{key_to_c_format(k2_info)}}},\n'
        header_content += (
            f'\t.k1_encrypted = {{{{{key_to_c_format(k1_encrypted_64_reversed[0])}}}, '
            f'{{{key_to_c_format(k1_encrypted_64_reversed[1])}}}}},\n'
        )
        header_content += f'\t.plaintext_data = {{{key_to_c_format(bytes(range(1, 129)))}}},\n'
        header_content += '    .xts_test_data = {\n'

        for data_size, flash_address, ciphertext in xts_test_data_xts_aes_256:
            header_content += (
                f'\t\t{{.data_size = {data_size}, '
                f'.data_offset = 0x{flash_address:x}, '
                f'.ciphertext = {{{key_to_c_format(ciphertext)}}}}},\n'
            )
        header_content += '\t}\n};\n'

        header_content += f"""
test_data_aes_mode_t test_data_ecdsa = {{
    .init_key = {{ {key_to_c_format(init_key)} }},
    .k2_info = {{ {key_to_c_format(k2_info)} }},
    .k1_encrypted = {{
        {{ {key_to_c_format(k1_encrypted_24_reversed[0])} }},
        {{ {key_to_c_format(k1_encrypted_32_reversed[0])} }},
        {{ {key_to_c_format(k1_encrypted_48_reversed[0])} }},
        {{ {key_to_c_format(k1_encrypted_48_reversed[1])} }},
    }},
    .ecdsa_test_data = {{
        .ecdsa_p192_pubx = {{ {key_to_c_format(ecdsa_p192_pubx)} }},
        .ecdsa_p192_puby = {{ {key_to_c_format(ecdsa_p192_puby)} }},
        .ecdsa_p256_pubx = {{ {key_to_c_format(ecdsa_p256_pubx)} }},
        .ecdsa_p256_puby = {{ {key_to_c_format(ecdsa_p256_puby)} }},
        .ecdsa_p384_pubx = {{ {key_to_c_format(ecdsa_p384_pubx)} }},
        .ecdsa_p384_puby = {{ {key_to_c_format(ecdsa_p384_puby)} }},
    }}
}};
"""

        header_content += f"""
test_data_ecdh0_mode_t test_data_ecdh0 = {{
    .plaintext_data = {{ {key_to_c_format(bytes(range(1, 129)))} }},
    .k1 = {{
        {{ {key_to_c_format(k1)} }},
        {{ {key_to_c_format(k1)} }},
    }},
    .k1_G = {{
        {{ {key_to_c_format(k1_G_0)} }},
        {{ {key_to_c_format(k1_G_1)} }},
    }}
}};
"""

        header_content += f"""
test_data_aes_mode_t test_data_hmac = {{
    .init_key = {{ {key_to_c_format(init_key)} }},
    .k2_info = {{ {key_to_c_format(k2_info)} }},
    .k1_encrypted = {{ {{ {key_to_c_format(k1_encrypted_32[0])} }}, {{  }} }},
    .hmac_test_data = {{
        .message = {{ {key_to_c_format(hmac_message)} }},
        .hmac_result = {{ {key_to_c_format(hmac_result)} }}
    }}
}};
"""

        header_content += f"""
test_data_aes_mode_t test_data_ds = {{
    .init_key = {{ {key_to_c_format(init_key)} }},
    .k2_info = {{ {key_to_c_format(k2_info)} }},
    .k1_encrypted = {{ {{ {key_to_c_format(k1_encrypted_32_reversed[0])} }}, {{  }} }},
    .ds_test_data = {{
#if SOC_DS_SIGNATURE_MAX_BIT_LEN == 4096
        .ds_message = {{ {key_to_c_format(ds_message_4096)} }},
        .ds_encrypted_input_params = {{ {key_to_c_format(ds_encrypted_input_params_4096)} }},
        .ds_key_size = 4096,
        .ds_result = {{ {key_to_c_format(ds_result_4096)} }},
#elif SOC_DS_SIGNATURE_MAX_BIT_LEN == 3072
        .ds_message = {{ {key_to_c_format(ds_message_3072)} }},
        .ds_encrypted_input_params = {{ {key_to_c_format(ds_encrypted_input_params_3072)} }},
        .ds_key_size = 3072,
        .ds_result = {{ {key_to_c_format(ds_result_3072)} }},
#endif
        .ds_encrypted_input_params_iv = {{ {key_to_c_format(ds_iv)} }},
    }},
}};
"""

        file.write(header_content)


def generate_tests_cases() -> None:
    # Main script logic follows as per your provided structure
    init_key = key_from_file_or_generate('init_key.bin', 32)
    k2 = key_from_file_or_generate('k2.bin', 32)
    rand_num = key_from_file_or_generate('rand_num.bin', 32)

    temp_result_inner = calculate_aes_cipher(k2, rand_num)
    temp_result_outer = calculate_aes_cipher(temp_result_inner + rand_num, init_key)
    k2_info = temp_result_outer

    k1 = key_from_file_or_generate('k1_64.bin', 64)

    k1_24 = k1[:24]
    k1_32 = k1[:32]
    k1_48 = k1[:48]
    k1_64 = k1[:]

    k1_24_reversed = k1_24[::-1]
    k1_32_reversed = k1_32[::-1]

    k1_48_1 = k1_48[:16]
    k1_48_1_reversed = k1_48_1[::-1]
    k1_48_2 = k1_48[16:]
    k1_48_2_reversed = k1_48_2[::-1]

    k1_64_1 = k1_64[:32]
    k1_64_1_reversed = k1_64_1[::-1]
    k1_64_2 = k1_64[32:]
    k1_64_2_reversed = k1_64_2[::-1]

    k1_encrypted_24 = [calculate_aes_cipher(b'\x00' * 8 + k1_24, k2)]
    k1_encrypted_32 = [calculate_aes_cipher(k1_32, k2)]
    k1_encrypted_48 = [calculate_aes_cipher(b'\x00' * 16 + k1_48_1, k2), calculate_aes_cipher(k1_48_2, k2)]
    k1_encrypted_64 = [calculate_aes_cipher(k1_64_1, k2), calculate_aes_cipher(k1_64_2, k2)]

    k1_encrypted_24_reversed = [calculate_aes_cipher(k1_24_reversed + b'\x00' * 8, k2)]
    k1_encrypted_32_reversed = [calculate_aes_cipher(k1_32_reversed, k2)]
    k1_encrypted_48_reversed = [
        calculate_aes_cipher(k1_48_1_reversed + b'\x00' * 16, k2),
        calculate_aes_cipher(k1_48_2_reversed, k2),
    ]
    k1_encrypted_64_reversed = [calculate_aes_cipher(k1_64_1_reversed, k2), calculate_aes_cipher(k1_64_2_reversed, k2)]

    test_data_xts_aes_128 = generate_xts_test_data(k1_32)
    xts_test_data_xts_aes_256 = generate_xts_test_data(k1_64)

    ecdsa_p192_pubx, ecdsa_p192_puby = generate_ecdsa_key_and_pub_key(k1_24, 192)
    ecdsa_p256_pubx, ecdsa_p256_puby = generate_ecdsa_key_and_pub_key(k1_32, 256)
    ecdsa_p384_pubx, ecdsa_p384_puby = generate_ecdsa_key_and_pub_key(k1_48, 384)

    k1_G_0, k1_G_1 = generate_k1_G(k1_32)

    hmac_message, hmac_result = generate_hmac_test_data(k1_32)

    ds_iv = os.urandom(16)

    ds_message_4096, ds_encrypted_input_params_4096, ds_result_4096 = generate_ds_encrypted_input_params(
        k1_32, 4096, ds_iv
    )

    ds_message_3072, ds_encrypted_input_params_3072, ds_result_3072 = generate_ds_encrypted_input_params(
        k1_32, 3072, ds_iv
    )

    write_to_c_header(
        init_key,
        k1_32,
        k2_info,
        k1_encrypted_24,
        k1_encrypted_24_reversed,
        k1_encrypted_32,
        k1_encrypted_32_reversed,
        k1_encrypted_48,
        k1_encrypted_48_reversed,
        test_data_xts_aes_128,
        k1_encrypted_64,
        k1_encrypted_64_reversed,
        xts_test_data_xts_aes_256,
        ecdsa_p192_pubx,
        ecdsa_p192_puby,
        ecdsa_p256_pubx,
        ecdsa_p256_puby,
        ecdsa_p384_pubx,
        ecdsa_p384_puby,
        k1_G_0,
        k1_G_1,
        hmac_message,
        hmac_result,
        ds_message_4096,
        ds_encrypted_input_params_4096,
        ds_result_4096,
        ds_message_3072,
        ds_encrypted_input_params_3072,
        ds_result_3072,
        ds_iv,
    )


if __name__ == '__main__':
    generate_tests_cases()
