from __future__ import unicode_literals
from tiny_test_fw import Utility
import re
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_base_mac_address(env, extra_data):

    dut = env.get_dut('base_mac_address', 'examples/system/base_mac_address')
    dut.start_app()

    dut.expect('BASE_MAC: Base MAC Address read from EFUSE BLK0', timeout=30)
    hex_r = r', '.join((r'0x([0-9a-f]{1,2})',) * 6)
    mac_m = dut.expect(re.compile(r'BASE_MAC: Using "' + hex_r + r'" as base MAC address'), timeout=5)
    Utility.console_log('BASE_MAC detected: {}'.format(':'.join(mac_m)))

    def get_expected_mac_string(increment):
        '''
        Return the string representation of the MAC address mac_m with the last octet incremented.
        mac_m is an array of strings in hexa-decimal format without the '0x' prefix.
        '''
        return ', '.join(['0x{}'.format(m) for m in mac_m[:-1]] + [hex(int(mac_m[-1], 16) + increment)])

    dut.expect_all('WIFI_STA MAC: ' + get_expected_mac_string(0),
                   'SoftAP MAC: ' + get_expected_mac_string(1),
                   'BT MAC: ' + get_expected_mac_string(2),
                   'Ethernet MAC: ' + get_expected_mac_string(3),
                   timeout=10)


if __name__ == '__main__':
    test_examples_base_mac_address()
