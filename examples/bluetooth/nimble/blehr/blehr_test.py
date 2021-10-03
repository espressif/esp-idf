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
import re
import subprocess
import threading
import traceback

try:
    import Queue
except ImportError:
    import queue as Queue

import ttfw_idf
from ble import lib_ble_client
from tiny_test_fw import Utility

# When running on local machine execute the following before running this script
# > make app bootloader
# > make print_flash_cmd | tail -n 1 > build/download.config


def blehr_client_task(hr_obj, dut, dut_addr):
    interface = 'hci0'
    ble_devname = 'blehr_sensor_1.0'
    hr_srv_uuid = '180d'
    hr_char_uuid = '2a37'

    # Get BLE client module
    ble_client_obj = lib_ble_client.BLE_Bluez_Client(iface=interface)

    # Discover Bluetooth Adapter and power on
    is_adapter_set = ble_client_obj.set_adapter()
    if not is_adapter_set:
        return

    # Connect BLE Device
    is_connected = ble_client_obj.connect(
        devname=ble_devname,
        devaddr=dut_addr)
    if not is_connected:
        return

    # Get services of the connected device
    services = ble_client_obj.get_services()
    if not services:
        ble_client_obj.disconnect()
        return

    # Get characteristics of the connected device
    ble_client_obj.get_chars()

    '''
    Blehr application run:
        Start Notifications
        Retrieve updated value
        Stop Notifications
    '''
    # Get service if exists
    service = ble_client_obj.get_service_if_exists(hr_srv_uuid)
    if service:
        # Get characteristic if exists
        char = ble_client_obj.get_char_if_exists(hr_char_uuid)
        if not char:
            ble_client_obj.disconnect()
            return
    else:
        ble_client_obj.disconnect()
        return

    # Start Notify
    # Read updated value
    # Stop Notify
    notify = ble_client_obj.start_notify(char)
    if not notify:
        ble_client_obj.disconnect()
        return

    # Check dut responses
    dut.expect('subscribe event; cur_notify=1', timeout=30)
    dut.expect('subscribe event; cur_notify=0', timeout=30)

    # Call disconnect to perform cleanup operations before exiting application
    ble_client_obj.disconnect()

    # Check dut responses
    dut.expect('disconnect;', timeout=30)


class BleHRThread(threading.Thread):
    def __init__(self, dut, dut_addr, exceptions_queue):
        threading.Thread.__init__(self)
        self.dut = dut
        self.dut_addr = dut_addr
        self.exceptions_queue = exceptions_queue

    def run(self):
        try:
            blehr_client_task(self, self.dut, self.dut_addr)
        except RuntimeError:
            self.exceptions_queue.put(traceback.format_exc(), block=False)


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_BT')
def test_example_app_ble_hr(env, extra_data):
    """
        Steps:
            1. Discover Bluetooth Adapter and Power On
            2. Connect BLE Device
            3. Start Notifications
            4. Updated value is retrieved
            5. Stop Notifications
    """
    # Remove cached bluetooth devices of any previous connections
    subprocess.check_output(['rm', '-rf', '/var/lib/bluetooth/*'])
    subprocess.check_output(['hciconfig', 'hci0', 'reset'])

    # Acquire DUT
    dut = env.get_dut('blehr', 'examples/bluetooth/nimble/blehr', dut_class=ttfw_idf.ESP32DUT)

    # Get binary file
    binary_file = os.path.join(dut.app.binary_path, 'blehr.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('blehr_bin_size', '{}KB'.format(bin_size // 1024))

    # Upload binary and start testing
    Utility.console_log('Starting blehr simple example test app')
    dut.start_app()
    dut.reset()

    # Get device address from dut
    dut_addr = dut.expect(re.compile(r'Device Address: ([a-fA-F0-9:]+)'), timeout=30)[0]
    exceptions_queue = Queue.Queue()
    # Starting a py-client in a separate thread
    blehr_thread_obj = BleHRThread(dut, dut_addr, exceptions_queue)
    blehr_thread_obj.start()
    blehr_thread_obj.join()

    exception_msg = None
    while True:
        try:
            exception_msg = exceptions_queue.get(block=False)
        except Queue.Empty:
            break
        else:
            Utility.console_log('\n' + exception_msg)

    if exception_msg:
        raise Exception('Blehr thread did not run successfully')


if __name__ == '__main__':
    test_example_app_ble_hr()
