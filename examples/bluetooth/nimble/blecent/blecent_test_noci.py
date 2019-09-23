#!/usr/bin/env python
#
# Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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
import sys
import re
import uuid
import subprocess

try:
    # This environment variable is expected on the host machine
    test_fw_path = os.getenv("TEST_FW_PATH")
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF
except ImportError as e:
    print(e)
    print("\nCheck your IDF_PATH\nOR")
    print("Try `export TEST_FW_PATH=$IDF_PATH/tools/tiny-test-fw` for resolving the issue\nOR")
    print("Try `pip install -r $IDF_PATH/tools/tiny-test-fw/requirements.txt` for resolving the issue")
    import IDF

try:
    import lib_ble_client
except ImportError:
    lib_ble_client_path = os.getenv("IDF_PATH") + "/tools/ble"
    if lib_ble_client_path and lib_ble_client_path not in sys.path:
        sys.path.insert(0, lib_ble_client_path)
    import lib_ble_client


import Utility

# When running on local machine execute the following before running this script
# > make app bootloader
# > make print_flash_cmd | tail -n 1 > build/download.config
# > export TEST_FW_PATH=~/esp/esp-idf/tools/tiny-test-fw


@IDF.idf_example_test(env_tag="Example_WIFI_BT")
def test_example_app_ble_central(env, extra_data):
    """
        Steps:
            1. Discover Bluetooth Adapter and Power On
    """

    interface = 'hci0'
    adv_host_name = "BleCentTestApp"
    adv_iface_index = 0
    adv_type = 'peripheral'
    adv_uuid = '1811'

    # Acquire DUT
    dut = env.get_dut("blecent", "examples/bluetooth/nimble/blecent")

    # Get binary file
    binary_file = os.path.join(dut.app.binary_path, "blecent.bin")
    bin_size = os.path.getsize(binary_file)
    IDF.log_performance("blecent_bin_size", "{}KB".format(bin_size // 1024))

    # Upload binary and start testing
    Utility.console_log("Starting blecent example test app")
    dut.start_app()

    subprocess.check_output(['rm','-rf','/var/lib/bluetooth/*'])
    device_addr = ':'.join(re.findall('..', '%012x' % uuid.getnode()))

    # Get BLE client module
    ble_client_obj = lib_ble_client.BLE_Bluez_Client(interface)
    if not ble_client_obj:
        raise RuntimeError("Get DBus-Bluez object failed !!")

    # Discover Bluetooth Adapter and power on
    is_adapter_set = ble_client_obj.set_adapter()
    if not is_adapter_set:
        raise RuntimeError("Adapter Power On failed !!")

    # Write device address to dut
    dut.expect("BLE Host Task Started", timeout=60)
    dut.write(device_addr + "\n")

    '''
    Blecent application run:
        Create GATT data
        Register GATT Application
        Create Advertising data
        Register advertisement
        Start advertising
    '''
    ble_client_obj.start_advertising(adv_host_name, adv_iface_index, adv_type, adv_uuid)

    # Call disconnect to perform cleanup operations before exiting application
    ble_client_obj.disconnect()

    # Check dut responses
    dut.expect("Connection established", timeout=30)

    dut.expect("Service discovery complete; status=0", timeout=30)
    print("Service discovery passed\n\tService Discovery Status: 0")

    dut.expect("GATT procedure initiated: read;", timeout=30)
    dut.expect("Read complete; status=0", timeout=30)
    print("Read passed\n\tSupportedNewAlertCategoryCharacteristic\n\tRead Status: 0")

    dut.expect("GATT procedure initiated: write;", timeout=30)
    dut.expect("Write complete; status=0", timeout=30)
    print("Write passed\n\tAlertNotificationControlPointCharacteristic\n\tWrite Status: 0")

    dut.expect("GATT procedure initiated: write;", timeout=30)
    dut.expect("Subscribe complete; status=0", timeout=30)
    print("Subscribe passed\n\tClientCharacteristicConfigurationDescriptor\n\tSubscribe Status: 0")


if __name__ == '__main__':
    test_example_app_ble_central()
