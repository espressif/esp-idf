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
import imp
import re
import os
import sys
import time

# This environment variable is expected on the host machine
test_fw_path = os.getenv("TEST_FW_PATH")
if test_fw_path and test_fw_path not in sys.path:
    sys.path.insert(0, test_fw_path)

# When running on local machine execute the following before running this script
# > export TEST_FW_PATH='~/esp/esp-idf/tools/tiny-test-fw'
# > make print_flash_cmd | tail -n 1 > build/download.config
# > make app bootloader

import TinyFW
import IDF

# Import esp_prov tool
idf_path = os.environ['IDF_PATH']
esp_prov   = imp.load_source("esp_prov",   idf_path + "/tools/esp_prov/esp_prov.py")
wifi_tools = imp.load_source("wifi_tools", idf_path + "/examples/provisioning/softap_prov/utils/wifi_tools.py")

@IDF.idf_example_test(env_tag="Example_WIFI_BT")
def test_examples_provisioning_softap(env, extra_data):
    # Acquire DUT
    dut1 = env.get_dut("softap_prov", "examples/provisioning/softap_prov")

    # Get binary file
    binary_file = os.path.join(dut1.app.binary_path, "softap_prov.bin")
    bin_size = os.path.getsize(binary_file)
    IDF.log_performance("softap_prov_bin_size", "{}KB".format(bin_size//1024))
    IDF.check_performance("softap_prov_bin_size", bin_size//1024)

    # Upload binary and start testing
    dut1.start_app()

    # Parse IP address of STA
    dut1.expect("Starting WiFi SoftAP provisioning")
    dut1.expect("SoftAP started")
    [ssid, password] = dut1.expect(re.compile(r"(?:[\s\S]*)SoftAP Provisioning started with SSID '(\S+)', Password '(\S+)'"))

    iface = wifi_tools.get_wiface_name()
    if iface == None:
        raise RuntimeError("Failed to get Wi-Fi interface on host")
    print("Interface name  : " + iface)
    print("SoftAP SSID     : " + ssid)
    print("SoftAP Password : " + password)

    ctrl = wifi_tools.wpa_cli(iface, reset_on_exit = True)
    print("Connecting to DUT SoftAP...")
    ip = ctrl.connect(ssid, password)
    print("Connected to DUT SoftAP")

    print("Starting Provisioning")
    verbose = False
    protover = "V0.1"
    secver = 1
    pop = "abcd1234"
    provmode = "softap"
    ap_ssid = "myssid"
    ap_password = "mypassword"
    softap_endpoint = ip.split('.')[0] + "." + ip.split('.')[1]+ "." + ip.split('.')[2] + ".1:80"

    print("Getting security")
    security = esp_prov.get_security(secver, pop, verbose)
    if security == None:
        raise RuntimeError("Failed to get security")

    print("Getting transport")
    transport = esp_prov.get_transport(provmode, softap_endpoint, None)
    if transport == None:
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
