from __future__ import unicode_literals
import re
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='8Mpsram')
def test_examples_himem(env, extra_data):

    dut = env.get_dut('himem', 'examples/system/himem')
    dut.start_app()

    mem = dut.expect(re.compile(r'esp_himem: Initialized. Using last \d+ 32KB address blocks for bank '
                                r'switching on (\d+) KB of physical memory.'), timeout=30)[0]

    dut.expect_all(re.compile(r'Himem has {}KiB of memory, \d+KiB of which is free. '
                              r'Testing the free memory...'.format(mem)),
                   'Done!',
                   timeout=10)


if __name__ == '__main__':
    test_examples_himem()
