#!/usr/bin/env python
#
# Demonstrates the use of otatool.py, a tool for performing ota partition level
# operations.
#
# SPDX-FileCopyrightText: 2018-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import re
import subprocess
import sys
from subprocess import CalledProcessError

import rich_click as click
import serial
from esp_pylib.cli_types import SerialPortType
from esp_pylib.constants import ESP_ROM_BAUD
from esp_pylib.excepthook import install_exception_reporting
from esp_pylib.logger import log
from rich.markup import escape


def get_running_partition(port=None):
    # Monitor the serial output of target device. The firmware outputs the currently
    # running partition

    baud = os.environ.get('ESPTOOL_BAUD', ESP_ROM_BAUD)

    if not port:
        error_message = 'Unable to obtain default target device port.\nSerial log:\n\n'
        try:
            # Check what esptool finds on what port the device is connected to
            output = subprocess.check_output(
                [sys.executable, '-m', 'esptool', 'chip-id']
            )  # may raise CalledProcessError
            pattern = r'Serial port ([^:\s]+)'
            pattern = re.compile(pattern.encode())

            port = re.search(pattern, output).group(1).decode('utf-8')  # may raise AttributeError
        except CalledProcessError as e:
            raise Exception(error_message + e.output.decode('utf-8', errors='replace'))
        except AttributeError:
            raise Exception(error_message + output.decode('utf-8', errors='replace'))

    serial_instance = serial.serial_for_url(port, baud, do_not_open=True)

    serial_instance.dtr = False
    serial_instance.rts = False

    serial_instance.rts = True
    serial_instance.open()
    serial_instance.rts = False

    # Read until example end and find the currently running partition string
    content = serial_instance.read_until(b'Example end')
    pattern = re.compile(b'Running partition: ([a-z0-9_]+)')
    running = re.search(pattern, content).group(1)

    return running.decode('utf-8')


@click.command(context_settings={'help_option_names': ['-h', '--help']})
@click.option('--port', type=SerialPortType(), default=None, help='Serial port device')
def main(port):
    try:
        res = get_running_partition(port)
    except Exception as e:
        log.die(escape(str(e)))

    print(res)


if __name__ == '__main__':
    install_exception_reporting()
    main()
