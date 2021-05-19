from __future__ import print_function
import binascii
from io import BytesIO
from collections import namedtuple
import os
import sys

import ttfw_idf
try:
    import espsecure
except ImportError:
    idf_path = os.getenv("IDF_PATH")
    if not idf_path or not os.path.exists(idf_path):
        raise
    sys.path.insert(0, os.path.join(idf_path, "components", "esptool_py", "esptool"))
    import espsecure


# To prepare a test runner for this example:
# 1. Generate zero flash encryption key:
#   dd if=/dev/zero of=key.bin bs=1 count=32
# 2.Burn Efuses:
#   espefuse.py --do-not-confirm -p $ESPPORT burn_efuse FLASH_CRYPT_CONFIG 0xf
#   espefuse.py --do-not-confirm -p $ESPPORT burn_efuse FLASH_CRYPT_CNT 0x1
#   espefuse.py --do-not-confirm -p $ESPPORT burn_key flash_encryption key.bin
@ttfw_idf.idf_example_test(env_tag='Example_Flash_Encryption')
def test_examples_security_flash_encryption(env, extra_data):
    dut = env.get_dut('flash_encryption', 'examples/security/flash_encryption', dut_class=ttfw_idf.ESP32DUT)
    # start test
    dut.start_app()

    # calculate the expected ciphertext
    flash_addr = dut.app.partition_table["storage"]["offset"]
    plain_hex_str = '00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f'
    plain_data = binascii.unhexlify(plain_hex_str.replace(' ', ''))

    # Emulate espsecure encrypt_flash_data command
    EncryptFlashDataArgs = namedtuple('EncryptFlashDataArgs', ['output', 'plaintext_file', 'address', 'keyfile', 'flash_crypt_conf', 'aes_xts'])
    args = EncryptFlashDataArgs(BytesIO(), BytesIO(plain_data), flash_addr, BytesIO(b'\x00' * 32), 0xF, False)
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
        'with spi_flash_read',
        expected_str
    ]
    for line in lines:
        dut.expect(line, timeout=2)


if __name__ == '__main__':
    test_examples_security_flash_encryption()
