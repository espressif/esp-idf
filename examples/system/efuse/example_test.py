from __future__ import unicode_literals
import re
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_efuse(env, extra_data):

    dut = env.get_dut('efuse', 'examples/system/efuse')
    dut.start_app()

    dut.expect_all(re.compile(r'example: Coding Scheme (3/4)|(NONE)|(REPEAT)|(RS \(Reed-Solomon coding\))'),
                   'example: read efuse fields',
                   re.compile(r'example: 1. read MAC address: {}'.format(r':'.join((r'[0-9a-f]{2}',) * 6))),
                   'example: 2. read secure_version: 0',
                   'example: 3. read custom fields',
                   'example: module_version = 0',
                   'example: device_role = None',
                   'example: setting_1 = 0',
                   'example: setting_2 = 0',
                   'example: custom_secure_version = 0',
                   'example: This example does not burn any efuse in reality only virtually',
                   'example: Write operations in efuse fields are performed virtually',
                   'example: write custom efuse fields',
                   'efuse: Virtual efuses enabled: Not really burning eFuses',
                   'example: module_version = 1',
                   'example: device_role = Slave',
                   'example: setting_1 = 3',
                   'example: setting_2 = 4',
                   'example: custom_secure_version = 5',
                   'example: Done',
                   timeout=30)


if __name__ == '__main__':
    test_examples_efuse()
