import re

import ttfw_idf
from tiny_test_fw import Utility


@ttfw_idf.idf_example_test(env_tag='UT_T1_SPIMODE', target=['esp32', 'esp32s2', 'esp32c3'])
def test_examples_sd_card(env, extra_data):  # type: (ttfw_idf.Env.Env, None ) -> None

    dut = env.get_dut('sd_card', 'examples/storage/sd_card/sdspi')
    dut.start_app()
    dut.expect('example: Initializing SD card', timeout=20)
    dut.expect('example: Using SPI peripheral', timeout=20)

    # Provide enough time for possible SD card formatting
    dut.expect('Filesystem mounted', timeout=60)

    # These lines are matched separately because of ASCII color codes in the output
    name = dut.expect(re.compile(r'Name: (\w+)'), timeout=20)[0]
    _type = dut.expect(re.compile(r'Type: (\S+)'), timeout=20)[0]
    speed = dut.expect(re.compile(r'Speed: (\S+)'), timeout=20)[0]
    size = dut.expect(re.compile(r'Size: (\S+)'), timeout=20)[0]

    Utility.console_log('Card {} {} {}MHz {} found'.format(name, _type, speed, size))

    dut.expect_all('Opening file /sdcard/hello.txt',
                   'File written',
                   'Renaming file /sdcard/hello.txt to /sdcard/foo.txt',
                   'Reading file /sdcard/foo.txt',
                   "Read from file: 'Hello {}!'".format(name),
                   'Card unmounted',
                   timeout=20)


if __name__ == '__main__':
    test_examples_sd_card()
