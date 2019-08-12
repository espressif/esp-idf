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
from threading import Thread
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
    print("Try `pip install -r $IDF_PATH/tools/tiny-test-fw/requirements.txt` for resolving the issue\n")
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


def blehr_client_task(dut_addr, dut):
    interface = 'hci0'
    ble_devname = 'blehr_sensor_1.0'
    hr_srv_uuid = '180d'
    hr_char_uuid = '2a37'

    # Get BLE client module
    ble_client_obj = lib_ble_client.BLE_Bluez_Client(interface, devname=ble_devname, devaddr=dut_addr)
    if not ble_client_obj:
        raise RuntimeError("Failed to get DBus-Bluez object")

    # Discover Bluetooth Adapter and power on
    is_adapter_set = ble_client_obj.set_adapter()
    if not is_adapter_set:
        raise RuntimeError("Adapter Power On failed !!")

    # Connect BLE Device
    is_connected = ble_client_obj.connect()
    if not is_connected:
        Utility.console_log("Connection to device ", ble_devname, "failed !!")
        # Call disconnect to perform cleanup operations before exiting application
        ble_client_obj.disconnect()
        return

    # Read Services
    services_ret = ble_client_obj.get_services()
    if services_ret:
        print("\nServices\n")
        print(services_ret)
    else:
        print("Failure: Read Services failed")
        ble_client_obj.disconnect()
        return

    '''
    Blehr application run:
        Start Notifications
        Retrieve updated value
        Stop Notifications
    '''
    blehr_ret = ble_client_obj.hr_update_simulation(hr_srv_uuid, hr_char_uuid)
    if blehr_ret:
        print("Success: blehr example test passed")
    else:
        print("Failure: blehr example test failed")

    # Call disconnect to perform cleanup operations before exiting application
    ble_client_obj.disconnect()


@IDF.idf_example_test(env_tag="Example_WIFI_BT")
def test_example_app_ble_hr(env, extra_data):
    """
        Steps:
            1. Discover Bluetooth Adapter and Power On
            2. Connect BLE Device
            3. Start Notifications
            4. Updated value is retrieved
            5. Stop Notifications
    """
    try:
        # Acquire DUT
        dut = env.get_dut("blehr", "examples/bluetooth/nimble/blehr")

        # Get binary file
        binary_file = os.path.join(dut.app.binary_path, "blehr.bin")
        bin_size = os.path.getsize(binary_file)
        IDF.log_performance("blehr_bin_size", "{}KB".format(bin_size // 1024))
        IDF.check_performance("blehr_bin_size", bin_size // 1024)

        # Upload binary and start testing
        Utility.console_log("Starting blehr simple example test app")
        dut.start_app()

        subprocess.check_output(['rm','-rf','/var/lib/bluetooth/*'])

        # Get device address from dut
        dut_addr = dut.expect(re.compile(r"Device Address: ([a-fA-F0-9:]+)"), timeout=30)[0]

        # Starting a py-client in a separate thread
        thread1 = Thread(target=blehr_client_task, args=(dut_addr,dut,))
        thread1.start()
        thread1.join()

        # Check dut responses
        dut.expect("subscribe event; cur_notify=1", timeout=30)
        dut.expect("GATT procedure initiated: notify;", timeout=30)
        dut.expect("subscribe event; cur_notify=0", timeout=30)
        dut.expect("disconnect;", timeout=30)

    except Exception as e:
        sys.exit(e)


if __name__ == '__main__':
    test_example_app_ble_hr()
