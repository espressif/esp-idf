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

from IDF.IDFDUT import ESP32DUT


@IDF.idf_example_test(env_tag='Example_ExtFlash')
def test_examples_storage_ext_flash_fatfs(env, extra_data):
    dut = env.get_dut('ext_flash_fatfs', 'examples/storage/ext_flash_fatfs', dut_class=ESP32DUT)
    dut.start_app()

    dut.expect('Initialized external Flash')
    dut.expect('partition \'nvs\'')
    dut.expect('partition \'storage\'')
    dut.expect('File written')
    dut.expect('Read from file: \'Written using ESP-IDF')


if __name__ == '__main__':
    test_examples_storage_ext_flash_fatfs()
