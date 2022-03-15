# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_base_mac_address(dut: Dut) -> None:
    dut.expect_exact('BASE_MAC: Base MAC Address read from EFUSE BLK0')
    hex_r = r', '.join((r'0x([0-9a-f]{1,2})',) * 6)
    mac_m = dut.expect(r'BASE_MAC: Using "' + hex_r + r'" as base MAC address', timeout=5).groups()

    def get_expected_mac_string(increment: int) -> str:
        '''
        Return the string representation of the MAC address mac_m with the last octet incremented.
        mac_m is an array of strings in hexa-decimal format without the '0x' prefix.
        '''
        return ', '.join(['0x{}'.format(m.decode('utf8')) for m in mac_m[:-1]] + [hex(int(mac_m[-1], 16) + increment)])

    dut.expect_exact('WIFI_STA MAC: ' + get_expected_mac_string(0), timeout=2)
    dut.expect_exact('SoftAP MAC: ' + get_expected_mac_string(1))

    if dut.target != 'esp32s2':
        dut.expect_exact('BT MAC: ' + get_expected_mac_string(2))
        dut.expect_exact('Ethernet MAC: ' + get_expected_mac_string(3))
