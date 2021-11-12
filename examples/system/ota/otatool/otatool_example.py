#!/usr/bin/env python
#
# Demonstrates the use of otatool.py, a tool for performing ota partition level
# operations.
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import argparse
import os
import sys

from get_running_partition import get_running_partition


def assert_file_same(file1, file2, err):
    with open(file1, 'rb') as f1:
        with open(file2, 'rb') as f2:
            f1 = f1.read()
            f2 = f2.read()

            if len(f1) < len(f2):
                f2 = f2[:len(f1)]
            else:
                f1 = f1[:len(f2)]

            if not f1 == f2:
                raise Exception(err)


def assert_running_partition(expected, port=None):
    running = get_running_partition(port)
    if running != expected:
        raise Exception('Running partition %s does not match expected %s' % (running, expected))


def main():
    COMPONENTS_PATH = os.path.expandvars(os.path.join('$IDF_PATH', 'components'))
    OTATOOL_DIR = os.path.join(COMPONENTS_PATH, 'app_update')

    sys.path.append(OTATOOL_DIR)
    from otatool import OtatoolTarget

    parser = argparse.ArgumentParser('ESP-IDF OTA Tool Example')

    parser.add_argument('--port', '-p', help='port where the device to perform operations on is connected')
    parser.add_argument('--binary', '-b', help='path to built example binary', default=os.path.join('build', 'otatool.bin'))
    args = parser.parse_args()

    target = OtatoolTarget(args.port)

    print('Writing factory firmware to ota_0')
    target.write_ota_partition(0, args.binary)

    print('Writing factory firmware to ota_1')
    target.write_ota_partition('ota_1', args.binary)

    # Verify that the contents of the two ota slots are the same as that of the factory partition
    print('Checking written firmware to ota_0 and ota_1 match factory firmware')
    target.read_ota_partition('ota_0', 'app0.bin')
    target.read_ota_partition(1, 'app1.bin')

    assert_file_same('app0.bin', args.binary, 'Slot 0 app does not match factory app')
    assert_file_same('app1.bin', args.binary, 'Slot 1 app does not match factory app')

    # Switch to factory app
    print('Switching to factory app')
    target.erase_otadata()
    assert_running_partition('factory')

    # Switch to slot 0
    print('Switching to OTA slot 0')
    target.switch_ota_partition(0)
    assert_running_partition('ota_0')

    # Switch to slot 1 twice in a row
    print('Switching to OTA slot 1 (twice in a row)')
    target.switch_ota_partition(1)
    assert_running_partition('ota_1')
    target.switch_ota_partition('ota_1')
    assert_running_partition('ota_1')

    # Switch to slot 0 twice in a row
    print('Switching to OTA slot 0 (twice in a row)')
    target.switch_ota_partition(0)
    assert_running_partition('ota_0')
    target.switch_ota_partition('ota_0')
    assert_running_partition('ota_0')

    # Switch to factory app
    print('Switching to factory app')
    target.erase_otadata()
    assert_running_partition('factory')

    # Switch to slot 1
    print('Switching to OTA slot 1')
    target.switch_ota_partition(1)
    assert_running_partition('ota_1')

    # Example end and cleanup
    print('\nOTA tool operations executed successfully!')
    clean_files = ['app0.bin', 'app1.bin']
    for clean_file in clean_files:
        os.unlink(clean_file)


if __name__ == '__main__':
    main()
