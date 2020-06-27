import re
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_wear_levelling(env, extra_data):

    dut = env.get_dut('wear_levelling', 'examples/storage/wear_levelling')
    dut.start_app()
    dut.expect_all('example: Mounting FAT filesystem',
                   'example: Opening file',
                   'example: File written',
                   'example: Reading file',
                   re.compile(r'example: Read from file: \'written using ESP-IDF \S+\''),
                   'example: Unmounting FAT filesystem',
                   'example: Done',
                   timeout=20)


if __name__ == '__main__':
    test_examples_wear_levelling()
