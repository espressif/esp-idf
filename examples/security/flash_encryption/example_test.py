from __future__ import print_function
import os
import sys

try:
    import IDF
except ImportError:
    test_fw_path = os.getenv('TEST_FW_PATH')
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF


# To prepare a test runner for this example:
# 1. Generate zero flash encryption key:
#   dd if=/dev/zero of=key.bin bs=1 count=32
# 2.Burn Efuses:
#   espefuse.py --do-not-confirm -p $ESPPORT burn_efuse FLASH_CRYPT_CONFIG 0xf
#   espefuse.py --do-not-confirm -p $ESPPORT burn_efuse FLASH_CRYPT_CNT 0x1
#   espefuse.py --do-not-confirm -p $ESPPORT burn_key flash_encryption key.bin
@IDF.idf_example_test(env_tag='Example_Flash_Encryption')
def test_examples_security_flash_encryption(env, extra_data):
    dut = env.get_dut('flash_encryption', 'examples/security/flash_encryption')
    # start test
    dut.start_app()
    lines = [
        'FLASH_CRYPT_CNT eFuse value is 1',
        'Flash encryption feature is enabled in DEVELOPMENT mode',
        'with esp_partition_write',
        '00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f',
        'with esp_partition_read',
        '00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f',
        'with spi_flash_read',
        '29 68 2e 13 88 a0 5b 7f  cc 6b 39 f9 d7 7b 32 2f'
    ]
    for line in lines:
        dut.expect(line, timeout=2)


if __name__ == '__main__':
    test_examples_security_flash_encryption()
