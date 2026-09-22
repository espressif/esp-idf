# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_base_mac_address(dut: Dut) -> None:
    def get_hex_r(num_bytes: int) -> str:
        return r', '.join((r'0x([0-9a-f]{1,2})',) * num_bytes)

    hex_r = get_hex_r(6)
    dut.expect_exact('BASE_MAC: Base MAC Address read from EFUSE BLK0')
    mac_m = dut.expect(r'BASE_MAC: Using "' + hex_r + r'" as base MAC address', timeout=5).groups()

    def get_expected_mac_string(increment: int, target: str) -> str:
        """
        Return the string representation of the MAC address mac_m with the last octet incremented.
        mac_m is an array of strings in hexa-decimal format without the '0x' prefix.
        """
        # as a result of some esp32s2 chips burned with one MAC address by mistake,
        # there are some MAC address are reserved for this bug fix.
        # related mistake MAC address is 0x7cdfa1003000~0x7cdfa1005fff,
        # reserved MAC address is 0x7cdfa1020000~0x7cdfa1022fff (MAC address + 0x1d000).
        if target == 'esp32s2' and increment == 1:
            hex_string = ''.join([m.decode('utf8').rjust(2, '0') for m in mac_m])
            mac_bytes = int(hex_string, 16)

            if mac_bytes >= int('7cdfa1003000', 16) and mac_bytes <= int('7cdfa1005fff', 16):
                mac_bytes += int('1d000', 16)
                hex_string = f'{mac_bytes:x}'
                # Format the new string to match the expected output from the app (includes stripping leading zeroes)
                return ', '.join('0x{}'.format(hex_string[i : i + 2].lstrip('0')) for i in range(0, len(hex_string), 2))

        return ', '.join(
            ['0x{}'.format(m.decode('utf8')) for m in mac_m[:-1]] + [hex((int(mac_m[-1], 16) + increment) & 0xFF)]
        )

    sdkconfig = dut.app.sdkconfig

    def get_expected_local_mac_string(increment: int) -> str:
        mac = [int(value.decode('utf8'), 16) for value in mac_m]
        mac[5] = (mac[5] + increment) & 0xFF
        mac[0] |= 0x02
        if mac[0] == int(mac_m[0], 16):
            mac[0] ^= 0x04
        return ', '.join(hex(value) for value in mac)

    has_wifi = sdkconfig.get('SOC_WIFI_SUPPORTED')
    has_wifi_ap_universal_mac = sdkconfig.get('ESP_MAC_ADDR_UNIVERSE_WIFI_AP')
    has_bt_mac = sdkconfig.get('ESP_MAC_ADDR_UNIVERSE_BT')
    has_four_universal_mac_addrs = sdkconfig.get('ESP_MAC_UNIVERSAL_MAC_ADDRESSES_FOUR')
    has_eth_universal_mac = sdkconfig.get('ESP_MAC_ADDR_UNIVERSE_ETH')

    if sdkconfig.get('ESP_WIFI_ENABLED'):
        dut.expect_exact('WIFI_STA MAC: ' + get_expected_mac_string(0, dut.target), timeout=2)
        if has_wifi_ap_universal_mac:
            dut.expect_exact('SoftAP MAC: ' + get_expected_mac_string(1, dut.target))
        else:
            dut.expect_exact('SoftAP MAC: ' + get_expected_local_mac_string(0))

    if has_bt_mac:
        bt_offset = 2 if has_wifi and has_four_universal_mac_addrs else 1 if has_wifi else 0
        dut.expect_exact('BT MAC: ' + get_expected_mac_string(bt_offset, dut.target))

    if has_eth_universal_mac:
        eth_offset = 3 if has_wifi or has_bt_mac else 0
        dut.expect_exact('Ethernet MAC: ' + get_expected_mac_string(eth_offset, dut.target))
    else:
        dut.expect_exact('Ethernet MAC: ' + get_expected_local_mac_string(1))
    dut.expect_exact('New Ethernet MAC: ' + get_expected_mac_string(6, dut.target))

    if sdkconfig.get('SOC_IEEE802154_SUPPORTED'):
        mac_ext_m = dut.expect(r'MAC_EXT: ' + get_hex_r(2), timeout=5).groups()
        mac_ext = ['0x{}'.format(m.decode('utf8')) for m in mac_ext_m]
        mac = ['0x{}'.format(m.decode('utf8')) for m in mac_m]
        expected_eui64 = f'{mac[0]}, {mac[1]}, {mac[2]}, {mac_ext[0]}, {mac_ext[1]}, {mac[3]}, {mac[4]}, {mac[5]}'
        dut.expect_exact(r'IEEE802154: ' + expected_eui64, timeout=5)
