from __future__ import print_function

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_ExtFlash')
def test_examples_storage_ext_flash_fatfs(env, extra_data):
    dut = env.get_dut('ext_flash_fatfs', 'examples/storage/ext_flash_fatfs')
    dut.start_app()

    dut.expect('Initialized external Flash')
    dut.expect('partition \'nvs\'')
    dut.expect('partition \'storage\'')
    dut.expect('File written')
    dut.expect('Read from file: \'Written using ESP-IDF')


if __name__ == '__main__':
    test_examples_storage_ext_flash_fatfs()
