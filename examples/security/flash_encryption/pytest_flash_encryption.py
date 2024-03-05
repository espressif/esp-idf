# SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import binascii
from collections import namedtuple
from io import BytesIO

import espsecure
import pytest
from pytest_embedded import Dut


# To prepare a test runner for this example:
# 1. Generate zero flash encryption key:
#   dd if=/dev/zero of=key.bin bs=1 count=32
# 2.Burn Efuses:
#   espefuse.py --do-not-confirm -p $ESPPORT burn_efuse FLASH_CRYPT_CONFIG 0xf
#   espefuse.py --do-not-confirm -p $ESPPORT burn_efuse FLASH_CRYPT_CNT 0x1
#   espefuse.py --do-not-confirm -p $ESPPORT burn_key flash_encryption key.bin
def _test_flash_encryption(dut: Dut) -> None:
    # Erase the nvs_key partition
    dut.serial.erase_partition('nvs_key')
    # calculate the expected ciphertext
    flash_addr = dut.app.partition_table['storage']['offset']
    plain_hex_str = '00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f'
    plain_data = binascii.unhexlify(plain_hex_str.replace(' ', ''))

    # espsecure uses the cryptography package for encrypting
    # with aes-xts, but does not allow for a symmetric key
    # so the key for later chips are not all zeros
    if dut.target == 'esp32':
        key_bytes = b'\x00' * 32
        aes_xts = False
    else:
        key_bytes = b'\xff' + b'\x00' * 31
        aes_xts = True

    # Emulate espsecure encrypt_flash_data command
    EncryptFlashDataArgs = namedtuple('EncryptFlashDataArgs', ['output', 'plaintext_file', 'address', 'keyfile', 'flash_crypt_conf', 'aes_xts'])
    args = EncryptFlashDataArgs(BytesIO(), BytesIO(plain_data), flash_addr, BytesIO(key_bytes), 0xF, aes_xts)
    espsecure.encrypt_flash_data(args)

    expected_ciphertext = args.output.getvalue()
    hex_ciphertext = binascii.hexlify(expected_ciphertext).decode('ascii')
    expected_str = (' '.join(hex_ciphertext[i:i + 2] for i in range(0, 16, 2)) + '  ' +
                    ' '.join(hex_ciphertext[i:i + 2] for i in range(16, 32, 2)))

    lines = [
        'FLASH_CRYPT_CNT eFuse value is 1',
        'Flash encryption feature is enabled in DEVELOPMENT mode',
        'with esp_partition_write',
        plain_hex_str,
        'with esp_partition_read',
        plain_hex_str,
        'with esp_flash_read',
        expected_str,
        # The status of NVS encryption for the "nvs" partition
        'NVS partition "nvs" is encrypted.',
        # The status of NVS encryption for the "custom_nvs" partition
        'NVS partition "custom_nvs" is encrypted.'
    ]
    for line in lines:
        dut.expect(line, timeout=20)


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.flash_encryption
def test_examples_security_flash_encryption(dut: Dut) -> None:
    _test_flash_encryption(dut)


@pytest.mark.esp32c3
@pytest.mark.flash_encryption
@pytest.mark.parametrize(
    'config',
    [
        'rom_impl',
    ],
    indirect=True,
)
def test_examples_security_flash_encryption_rom_impl(dut: Dut) -> None:
    _test_flash_encryption(dut)
