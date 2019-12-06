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
import Queue
import traceback
import threading
import subprocess

try:
    # This environment variable is expected on the host machine
    test_fw_path = os.getenv("TEST_FW_PATH")
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF
    from IDF.IDFDUT import ESP32DUT
except ImportError as e:
    print(e)
    print("Try `export TEST_FW_PATH=$IDF_PATH/tools/tiny-test-fw` for resolving the issue")
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


def bleprph_client_task(prph_obj, dut, dut_addr):
    interface = 'hci0'
    ble_devname = 'nimble-bleprph'
    srv_uuid = '2f12'

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
        # Call disconnect to perform cleanup operations before exiting application
        ble_client_obj.disconnect()
        raise RuntimeError("Connection to device " + ble_devname + " failed !!")

    # Check dut responses
    dut.expect("GAP procedure initiated: advertise;", timeout=30)

    # Read Services
    services_ret = ble_client_obj.get_services(srv_uuid)
    if services_ret:
        Utility.console_log("\nServices\n")
        Utility.console_log(str(services_ret))
    else:
        ble_client_obj.disconnect()
        raise RuntimeError("Failure: Read Services failed")

    # Read Characteristics
    chars_ret = {}
    chars_ret = ble_client_obj.read_chars()
    if chars_ret:
        Utility.console_log("\nCharacteristics retrieved")
        for path, props in chars_ret.items():
            Utility.console_log("\n\tCharacteristic: " + str(path))
            Utility.console_log("\tCharacteristic UUID: " + str(props[2]))
            Utility.console_log("\tValue: " + str(props[0]))
            Utility.console_log("\tProperties: : " + str(props[1]))
    else:
        ble_client_obj.disconnect()
        raise RuntimeError("Failure: Read Characteristics failed")

    '''
    Write Characteristics
    - write 'A' to characteristic with write permission
    '''
    chars_ret_on_write = {}
    chars_ret_on_write = ble_client_obj.write_chars('A')
    if chars_ret_on_write:
        Utility.console_log("\nCharacteristics after write operation")
        for path, props in chars_ret_on_write.items():
            Utility.console_log("\n\tCharacteristic:" + str(path))
            Utility.console_log("\tCharacteristic UUID: " + str(props[2]))
            Utility.console_log("\tValue:" + str(props[0]))
            Utility.console_log("\tProperties: : " + str(props[1]))
    else:
        ble_client_obj.disconnect()
        raise RuntimeError("Failure: Write Characteristics failed")

    # Call disconnect to perform cleanup operations before exiting application
    ble_client_obj.disconnect()


class BlePrphThread(threading.Thread):
    def __init__(self, dut, dut_addr, exceptions_queue):
        threading.Thread.__init__(self)
        self.dut = dut
        self.dut_addr = dut_addr
        self.exceptions_queue = exceptions_queue

    def run(self):
        try:
            bleprph_client_task(self, self.dut, self.dut_addr)
        except Exception:
            self.exceptions_queue.put(traceback.format_exc(), block=False)


@IDF.idf_example_test(env_tag="Example_WIFI_BT")
def test_example_app_ble_peripheral(env, extra_data):
    """
        Steps:
            1. Discover Bluetooth Adapter and Power On
            2. Connect BLE Device
            3. Read Services
            4. Read Characteristics
            5. Write Characteristics
    """
    subprocess.check_output(['rm','-rf','/var/lib/bluetooth/*'])
    subprocess.check_output(['hciconfig','hci0','reset'])

    # Acquire DUT
    dut = env.get_dut("bleprph", "examples/bluetooth/nimble/bleprph", dut_class=ESP32DUT)

    # Get binary file
    binary_file = os.path.join(dut.app.binary_path, "bleprph.bin")
    bin_size = os.path.getsize(binary_file)
    IDF.log_performance("bleprph_bin_size", "{}KB".format(bin_size // 1024))
    IDF.check_performance("bleprph_bin_size", bin_size // 1024)

    # Upload binary and start testing
    Utility.console_log("Starting bleprph simple example test app")
    dut.start_app()
    dut.reset()

    # Get device address from dut
    dut_addr = dut.expect(re.compile(r"Device Address: ([a-fA-F0-9:]+)"), timeout=30)[0]

    exceptions_queue = Queue.Queue()
    # Starting a py-client in a separate thread
    bleprph_thread_obj = BlePrphThread(dut, dut_addr, exceptions_queue)
    bleprph_thread_obj.start()
    bleprph_thread_obj.join()

    exception_msg = None
    while True:
        try:
            exception_msg = exceptions_queue.get(block=False)
        except Queue.Empty:
            break
        else:
            Utility.console_log("\n" + exception_msg)

    if exception_msg:
        raise Exception("Thread did not run successfully")

    # Check dut responses
    dut.expect("connection established; status=0", timeout=30)
    dut.expect("disconnect;", timeout=30)


if __name__ == '__main__':
    test_example_app_ble_peripheral()
