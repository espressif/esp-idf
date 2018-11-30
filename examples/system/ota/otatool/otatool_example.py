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
import os
import sys
import subprocess
import argparse
import serial
import re

IDF_PATH = os.path.expandvars("$IDF_PATH")

OTATOOL_PY = os.path.join(IDF_PATH, "components", "app_update", "otatool.py")
ESPTOOL_PY = os.path.join(IDF_PATH, "components", "esptool_py", "esptool", "esptool.py")

INVOKE_ARGS = [sys.executable, OTATOOL_PY, "-q"]


def sized_file_compare(file1, file2):
    with open(file1, "rb") as f1:
        with open(file2, "rb") as f2:
            f1 = f1.read()
            f2 = f2.read()

            if len(f1) < len(f2):
                f2 = f2[:len(f1)]
            else:
                f1 = f1[:len(f2)]

            return f1 == f2


def check(condition, message):
    if not condition:
        print("Error: " + message)
        sys.exit(1)


def flash_example_firmware_to_ota_partitions(args):
    # Invokes the command
    #
    # otatool.py -q write_ota_partition --slot <part_slot>        or
    # otatool.py -q write_ota_partition --name <part_name>
    #
    # to write the contents of a file to the specified ota partition (either using name or the slot number)
    print("Writing factory firmware to ota_0")
    invoke_args = INVOKE_ARGS + ["write_ota_partition", "--slot", "0", "--input", args.binary]
    subprocess.check_call(invoke_args)

    print("Writing factory firmware to ota_1")
    invoke_args = INVOKE_ARGS + ["write_ota_partition", "--name", "ota_1", "--input", args.binary]
    subprocess.check_call(invoke_args)

    # Verify that the contents of the two ota slots are the same as that of the factory partition
    print("Checking written firmware to ota_0 and ota_1 match factory firmware")

    # Invokes the command
    #
    # otatool.py -q read_ota_partition --slot <part_slot>        or
    # otatool.py -q read_ota_partition --name <part_name>
    #
    # to read the contents of a specified ota partition (either using name or the slot number) and write to a file
    invoke_args = INVOKE_ARGS + ["read_ota_partition", "--slot", "0", "--output", "app_0.bin"]
    subprocess.check_call(invoke_args)

    invoke_args = INVOKE_ARGS + ["read_ota_partition", "--name", "ota_1", "--output", "app_1.bin"]
    subprocess.check_call(invoke_args)

    ota_same = sized_file_compare("app_0.bin", args.binary)
    check(ota_same, "Slot 0 app does not match factory app")

    ota_same = sized_file_compare("app_1.bin", args.binary)
    check(ota_same, "Slot 1 app does not match factory app")


def check_running_ota_partition(expected, port=None):
    # Monitor the serial output of target device. The firmware outputs the currently
    # running partition. It should match the partition the otatool switched to.

    if expected == 0 or expected == "ota_0":
        expected = b"ota_0"
    elif expected == 1 or expected == "ota_1":
        expected = b"ota_1"
    else:
        expected = b"factory"

    sys.path.append(os.path.join(IDF_PATH, 'components', 'esptool_py', 'esptool'))
    import esptool

    baud = os.environ.get("ESPTOOL_BAUD", esptool.ESPLoader.ESP_ROM_BAUD)

    if not port:
        # Check what esptool.py finds on what port the device is connected to
        output = subprocess.check_output([sys.executable, ESPTOOL_PY, "chip_id"])
        pattern = r"Serial port ([\S]+)"
        pattern = re.compile(pattern.encode())
        port = re.search(pattern, output).group(1)

    serial_instance = serial.serial_for_url(port.decode("utf-8"), baud, do_not_open=True)

    serial_instance.dtr = False
    serial_instance.rts = False

    serial_instance.rts = True
    serial_instance.open()
    serial_instance.rts = False

    # Read until example end and find the currently running partition string
    content = serial_instance.read_until(b"Example end")
    pattern = re.compile(b"Running partition: ([a-z0-9_]+)")
    running = re.search(pattern, content).group(1)

    check(expected == running, "Running partition %s does not match expected %s" % (running, expected))


def switch_partition(part, port):
    if isinstance(part, int):
        spec = "slot"
    else:
        spec = "name"

    print("Switching to ota partition %s %s" % (spec, str(part)))

    if str(part) == "factory":
        # Invokes the command
        #
        # otatool.py -q erase_otadata
        #
        # to erase the otadata partition, effectively setting boot firmware to
        # factory
        subprocess.check_call(INVOKE_ARGS + ["erase_otadata"])
    else:
        # Invokes the command
        #
        # otatool.py -q switch_otadata --slot <part_slot>        or
        # otatool.py -q switch_otadata --name <part_name>
        #
        # to switch to the indicated ota partition (either using name or the slot number)
        subprocess.check_call(INVOKE_ARGS + ["switch_otadata", "--" + spec, str(part)])

    check_running_ota_partition(part, port)


def main():
    global INVOKE_ARGS

    parser = argparse.ArgumentParser("ESP-IDF OTA Tool Example")

    parser.add_argument("--port", "-p", help="port where the device to perform operations on is connected")
    parser.add_argument("--binary", "-b", help="path to built example binary", default=os.path.join("build", "otatool.bin"))
    args = parser.parse_args()

    if args.port:
        INVOKE_ARGS += ["--port", args.port]

    # Flash the factory firmware to all ota partitions
    flash_example_firmware_to_ota_partitions(args)

    # Perform switching ota partitions
    switch_partition("factory", args.port)
    switch_partition("factory", args.port)  # check switching to factory partition twice in a row

    switch_partition(0, args.port)

    switch_partition("ota_1", args.port)
    switch_partition(1, args.port)  # check switching to ota_1 partition twice in a row

    switch_partition("ota_0", args.port)
    switch_partition(0, args.port)  # check switching to ota_0 partition twice in a row

    switch_partition("factory", args.port)

    switch_partition(1, args.port)  # check switching to ota_1 partition from factory

    print("\nOTA tool operations executed successfully!")


if __name__ == '__main__':
    main()
