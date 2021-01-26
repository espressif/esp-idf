#!/usr/bin/env python
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import print_function

import os
import re

import esp_prov
import ttfw_idf

# Have esp_prov throw exception
esp_prov.config_throw_except = True


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_BT')
def test_examples_wifi_prov_mgr(env, extra_data):
    # Acquire DUT
    dut1 = env.get_dut('wifi_prov_mgr', 'examples/provisioning/wifi_prov_mgr', dut_class=ttfw_idf.ESP32DUT)

    # Get binary file
    binary_file = os.path.join(dut1.app.binary_path, 'wifi_prov_mgr.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('wifi_prov_mgr_bin_size', '{}KB'.format(bin_size // 1024))

    # Upload binary and start testing
    dut1.start_app()

    # Check if BT memory is released before provisioning starts
    dut1.expect('wifi_prov_scheme_ble: BT memory released', timeout=60)

    # Parse BLE devname
    devname = dut1.expect(re.compile(r'Provisioning started with service name : (PROV_\S\S\S\S\S\S)'), timeout=30)[0]
    print('BLE Device Alias for DUT :', devname)

    print('Starting Provisioning')
    verbose = False
    protover = 'v1.1'
    secver = 1
    pop = 'abcd1234'
    provmode = 'ble'
    ap_ssid = 'myssid'
    ap_password = 'mypassword'

    print('Getting security')
    security = esp_prov.get_security(secver, pop, verbose)
    if security is None:
        raise RuntimeError('Failed to get security')

    print('Getting transport')
    transport = esp_prov.get_transport(provmode, devname)
    if transport is None:
        raise RuntimeError('Failed to get transport')

    print('Verifying protocol version')
    if not esp_prov.version_match(transport, protover):
        raise RuntimeError('Mismatch in protocol version')

    print('Verifying scan list capability')
    if not esp_prov.has_capability(transport, 'wifi_scan'):
        raise RuntimeError('Capability not present')

    print('Starting Session')
    if not esp_prov.establish_session(transport, security):
        raise RuntimeError('Failed to start session')

    print('Sending Custom Data')
    if not esp_prov.custom_data(transport, security, 'My Custom Data'):
        raise RuntimeError('Failed to send custom data')

    print('Sending Wifi credential to DUT')
    if not esp_prov.send_wifi_config(transport, security, ap_ssid, ap_password):
        raise RuntimeError('Failed to send Wi-Fi config')

    print('Applying config')
    if not esp_prov.apply_wifi_config(transport, security):
        raise RuntimeError('Failed to send apply config')

    if not esp_prov.wait_wifi_connected(transport, security):
        raise RuntimeError('Provisioning failed')

    # Check if BTDM memory is released after provisioning finishes
    dut1.expect('wifi_prov_scheme_ble: BTDM memory released', timeout=30)


if __name__ == '__main__':
    test_examples_wifi_prov_mgr()
