import re
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_spiffs(env, extra_data):

    dut = env.get_dut('spiffs', 'examples/storage/spiffs')
    dut.start_app()
    dut.expect_all('example: Initializing SPIFFS',
                   re.compile(r'example: Partition size: total: \d+, used: \d+'),
                   'example: Opening file',
                   'example: File written',
                   'example: Renaming file',
                   'example: Reading file',
                   'example: Read from file: \'Hello World!\'',
                   'example: SPIFFS unmounted',
                   timeout=20)


if __name__ == '__main__':
    test_examples_spiffs()
