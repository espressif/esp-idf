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
import serial
import subprocess
import re
import argparse

from subprocess import CalledProcessError


def get_running_partition(port=None):
    # Monitor the serial output of target device. The firmware outputs the currently
    # running partition

    IDF_PATH = os.path.expandvars("$IDF_PATH")
    sys.path.append(os.path.join(IDF_PATH, 'components', 'esptool_py', 'esptool'))
    import esptool

    ESPTOOL_PY = os.path.join(IDF_PATH, "components", "esptool_py", "esptool", "esptool.py")

    baud = os.environ.get("ESPTOOL_BAUD", esptool.ESPLoader.ESP_ROM_BAUD)

    if not port:
        error_message = "Unable to obtain default target device port.\nSerial log:\n\n"
        try:
            # Check what esptool.py finds on what port the device is connected to
            output = subprocess.check_output([sys.executable, ESPTOOL_PY, "chip_id"])  # may raise CalledProcessError
            pattern = r"Serial port ([\S]+)"
            pattern = re.compile(pattern.encode())

            port = re.search(pattern, output).group(1)  # may raise AttributeError
        except CalledProcessError as e:
            raise Exception(error_message + e.output)
        except AttributeError:
            raise Exception(error_message + output)

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

    return running.decode("utf-8")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", default=None)
    args = parser.parse_args()

    try:
        res = get_running_partition(args.port)
    except Exception as e:
        print(e.message)
        sys.exit(1)

    print(res)


if __name__ == "__main__":
    main()
