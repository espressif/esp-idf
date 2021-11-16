# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_fatfsgen(env, _):  # type: ignore

    dut = env.get_dut('fatfsgen', 'examples/storage/fatfsgen')
    dut.start_app()
    dut.expect_all('example: Mounting FAT filesystem',
                   'example: Reading file',
                   'example: Read from file: \'this is test\'',
                   'example: Unmounting FAT filesystem',
                   'example: Done',
                   timeout=20)


if __name__ == '__main__':
    test_examples_fatfsgen()
