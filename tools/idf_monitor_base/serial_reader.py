# Copyright 2015-2021 Espressif Systems (Shanghai) CO LTD
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

import queue
import sys
import time

import serial

from .constants import CHECK_ALIVE_FLAG_TIMEOUT, MINIMAL_EN_LOW_DELAY, RECONNECT_DELAY, TAG_SERIAL
from .output_helpers import red_print, yellow_print
from .stoppable_thread import StoppableThread


class SerialReader(StoppableThread):
    """ Read serial data from the serial port and push to the
    event queue, until stopped.
    """
    def __init__(self, serial_instance, event_queue):
        #  type: (serial.Serial, queue.Queue) -> None
        super(SerialReader, self).__init__()
        self.baud = serial_instance.baudrate
        self.serial = serial_instance
        self.event_queue = event_queue
        self.gdb_exit = False
        if not hasattr(self.serial, 'cancel_read'):
            # enable timeout for checking alive flag,
            # if cancel_read not available
            self.serial.timeout = CHECK_ALIVE_FLAG_TIMEOUT

    def run(self):
        #  type: () -> None
        if not self.serial.is_open:
            self.serial.baudrate = self.baud
            # We can come to this thread at startup or from external application line GDB.
            # If we come from GDB we would like to continue to run without reset.

            high = False
            low = True

            self.serial.dtr = low      # Non reset state
            self.serial.rts = high     # IO0=HIGH
            self.serial.dtr = self.serial.dtr   # usbser.sys workaround
            # Current state not reset the target!
            self.serial.open()
            if not self.gdb_exit:
                self.serial.dtr = high     # Set dtr to reset state (affected by rts)
                self.serial.rts = low      # Set rts/dtr to the reset state
                self.serial.dtr = self.serial.dtr   # usbser.sys workaround

                # Add a delay to meet the requirements of minimal EN low time (2ms for ESP32-C3)
                time.sleep(MINIMAL_EN_LOW_DELAY)
            self.gdb_exit = False
            self.serial.rts = high             # Set rts/dtr to the working state
            self.serial.dtr = self.serial.dtr   # usbser.sys workaround
        try:
            while self.alive:
                try:
                    data = self.serial.read(self.serial.in_waiting or 1)
                except (serial.serialutil.SerialException, IOError) as e:
                    data = b''
                    # self.serial.open() was successful before, therefore, this is an issue related to
                    # the disappearance of the device
                    red_print(e)
                    yellow_print('Waiting for the device to reconnect', newline='')
                    self.serial.close()
                    while self.alive:  # so that exiting monitor works while waiting
                        try:
                            time.sleep(RECONNECT_DELAY)
                            self.serial.open()
                            break  # device connected
                        except serial.serialutil.SerialException:
                            yellow_print('.', newline='')
                            sys.stderr.flush()
                    yellow_print('')  # go to new line
                if data:
                    self.event_queue.put((TAG_SERIAL, data), False)
        finally:
            self.serial.close()

    def _cancel(self):
        #  type: () -> None
        if hasattr(self.serial, 'cancel_read'):
            try:
                self.serial.cancel_read()
            except Exception:  # noqa
                pass
