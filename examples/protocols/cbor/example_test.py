from __future__ import unicode_literals

import re
import textwrap

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_Protocols')
def test_examples_cbor(env, extra_data):

    dut = env.get_dut('cbor', 'examples/protocols/cbor')
    dut.start_app()
    dut.expect(re.compile(r'example: encoded buffer size \d+'))
    dut.expect('example: convert CBOR to JSON')
    parsed_info = dut.expect(re.compile(r'\[\{"chip":"(\w+)","unicore":(\w+),"ip":\[(\d+),(\d+),(\d+),(\d+)\]\},'
                                        r'3.1400001049041748'
                                        r',"simple\(99\)","2019-07-10 09:00:00\+0000","undefined"\]'))
    dut.expect('example: decode CBOR manually')

    dut.expect(re.compile(textwrap.dedent(r'''
                                           Array\[\s+
                                             Map{{\s+
                                               chip\s+
                                               {}\s+
                                               unicore\s+
                                               {}\s+
                                               ip\s+
                                               Array\[\s+
                                                 {}\s+
                                                 {}\s+
                                                 {}\s+
                                                 {}\s+
                                               \]\s+
                                             }}\s+
                                             3.14\s+
                                             simple\(99\)\s+
                                             2019-07-10 09:00:00\+0000\s+
                                             undefined\s+
                                           \]'''.format(*parsed_info)).replace('{', r'\{').replace('}', r'\}')))


if __name__ == '__main__':
    test_examples_cbor()
