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


def blecent_client_task(dut):
    interface = 'hci0'
    adv_host_name = 'BleCentTestApp'
    adv_type = 'peripheral'
    adv_uuid = '1811'

    # Get BLE client module
    ble_client_obj = lib_ble_client.BLE_Bluez_Client(iface=interface)

    # Discover Bluetooth Adapter and power on
    is_adapter_set = ble_client_obj.set_adapter()
    if not is_adapter_set:
        return

    '''
    Blecent application run:
        Create GATT data
        Register GATT Application
        Create Advertising data
        Register advertisement
        Start advertising
    '''
    # Create Gatt Application
    # Register GATT Application
    ble_client_obj.register_gatt_app()

    # Register Advertisement
    # Check read/write/subscribe is received from device
    ble_client_obj.register_adv(adv_host_name, adv_type, adv_uuid)

    # Check dut responses
    dut.expect('Connection established', timeout=30)
    dut.expect('Service discovery complete; status=0', timeout=30)
    dut.expect('GATT procedure initiated: read;', timeout=30)
    dut.expect('Read complete; status=0', timeout=30)
    dut.expect('GATT procedure initiated: write;', timeout=30)
    dut.expect('Write complete; status=0', timeout=30)
    dut.expect('GATT procedure initiated: write;', timeout=30)
    dut.expect('Subscribe complete; status=0', timeout=30)
    dut.expect('received notification;', timeout=30)


class BleCentThread(threading.Thread):
    def __init__(self, dut, exceptions_queue):
        threading.Thread.__init__(self)
        self.dut = dut
        self.exceptions_queue = exceptions_queue

    def run(self):
        try:
            blecent_client_task(self.dut)
        except RuntimeError:
            self.exceptions_queue.put(traceback.format_exc(), block=False)


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_BT')
def test_example_app_ble_central(env, extra_data):
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
    dut = env.get_dut('blecent', 'examples/bluetooth/nimble/blecent', dut_class=ttfw_idf.ESP32DUT)

    # Get binary file
    binary_file = os.path.join(dut.app.binary_path, 'blecent.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('blecent_bin_size', '{}KB'.format(bin_size // 1024))

    # Upload binary and start testing
    Utility.console_log('Starting blecent example test app')
    dut.start_app()
    dut.reset()

    exceptions_queue = Queue.Queue()
    # Starting a py-client in a separate thread
    blehr_thread_obj = BleCentThread(dut, exceptions_queue)
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
        raise Exception('Blecent thread did not run successfully')


if __name__ == '__main__':
    test_example_app_ble_central()
