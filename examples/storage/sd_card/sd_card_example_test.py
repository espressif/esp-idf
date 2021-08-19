from tiny_test_fw import Utility
import ttfw_idf
import re


@ttfw_idf.idf_example_test(env_tag='UT_T1_SDMODE')
def test_examples_sd_card(env, extra_data):

    dut = env.get_dut('sd_card', 'examples/storage/sd_card')
    dut.start_app()
    dut.expect('example: Initializing SD card', timeout=20)
    peripheral = dut.expect(re.compile(r'example: Using (\w+) peripheral'), timeout=5)[0]
    Utility.console_log('peripheral {} detected'.format(peripheral))
    assert peripheral in ('SDMMC', 'SPI')

    # Provide enough time for possible SD card formatting
    dut.expect('Filesystem mounted', timeout=60)

    # These lines are matched separately because of ASCII color codes in the output
    name = dut.expect(re.compile(r'Name: (\w+)'), timeout=5)[0]
    _type = dut.expect(re.compile(r'Type: (\S+)'), timeout=5)[0]
    speed = dut.expect(re.compile(r'Speed: (\S+)'), timeout=5)[0]
    size = dut.expect(re.compile(r'Size: (\S+)'), timeout=5)[0]

    Utility.console_log('Card {} {} {}MHz {} found'.format(name, _type, speed, size))

    dut.expect_all('Opening file',
                   'File written',
                   'Renaming file',
                   'Reading file',
                   "Read from file: 'Hello {}!".format(name),
                   'Card unmounted',
                   timeout=5)


if __name__ == '__main__':
    test_examples_sd_card()
