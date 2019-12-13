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
import re
import os
import time

import ttfw_idf
import esp_prov
import wifi_tools

# Have esp_prov throw exception
esp_prov.config_throw_except = True


@ttfw_idf.idf_example_test(env_tag="Example_WIFI_BT")
def test_examples_provisioning_softap(env, extra_data):
    # Acquire DUT
    dut1 = env.get_dut("softap_prov", "examples/provisioning/softap_prov")

    # Get binary file
    binary_file = os.path.join(dut1.app.binary_path, "softap_prov.bin")
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance("softap_prov_bin_size", "{}KB".format(bin_size // 1024))
    ttfw_idf.check_performance("softap_prov_bin_size", bin_size // 1024)

    # Upload binary and start testing
    dut1.start_app()

    # Parse IP address of STA
    dut1.expect("Starting WiFi SoftAP provisioning", timeout=60)
    [ssid, password] = dut1.expect(re.compile(r"SoftAP Provisioning started with SSID '(\S+)', Password '(\S+)'"), timeout=30)

    iface = wifi_tools.get_wiface_name()
    if iface is None:
        raise RuntimeError("Failed to get Wi-Fi interface on host")
    print("Interface name  : " + iface)
    print("SoftAP SSID     : " + ssid)
    print("SoftAP Password : " + password)

    ctrl = wifi_tools.wpa_cli(iface, reset_on_exit=True)
    print("Connecting to DUT SoftAP...")
    ip = ctrl.connect(ssid, password)
    got_ip = dut1.expect(re.compile(r"softAP assign IP to station,IP is: (\d+.\d+.\d+.\d+)"), timeout=30)[0]
    if ip != got_ip:
        raise RuntimeError("SoftAP connected to another host! " + ip + "!=" + got_ip)
    print("Connected to DUT SoftAP")

    print("Starting Provisioning")
    verbose = False
    protover = "V0.1"
    secver = 1
    pop = "abcd1234"
    provmode = "softap"
    ap_ssid = "myssid"
    ap_password = "mypassword"
    softap_endpoint = ip.split('.')[0] + "." + ip.split('.')[1] + "." + ip.split('.')[2] + ".1:80"

    print("Getting security")
    security = esp_prov.get_security(secver, pop, verbose)
    if security is None:
        raise RuntimeError("Failed to get security")

    print("Getting transport")
    transport = esp_prov.get_transport(provmode, softap_endpoint)
    if transport is None:
        raise RuntimeError("Failed to get transport")

    print("Verifying protocol version")
    if not esp_prov.version_match(transport, protover):
        raise RuntimeError("Mismatch in protocol version")

    print("Starting Session")
    if not esp_prov.establish_session(transport, security):
        raise RuntimeError("Failed to start session")

    print("Sending Wifi credential to DUT")
    if not esp_prov.send_wifi_config(transport, security, ap_ssid, ap_password):
        raise RuntimeError("Failed to send Wi-Fi config")

    print("Applying config")
    if not esp_prov.apply_wifi_config(transport, security):
        raise RuntimeError("Failed to send apply config")

    success = False
    while True:
        time.sleep(5)
        print("Wi-Fi connection state")
        ret = esp_prov.get_wifi_config(transport, security)
        if (ret == 1):
            continue
        elif (ret == 0):
            print("Provisioning was successful")
            success = True
        break

    if not success:
        raise RuntimeError("Provisioning failed")


if __name__ == '__main__':
    test_examples_provisioning_softap()
