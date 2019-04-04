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

from __future__ import unicode_literals
import sys
import argparse
import serial
import threading
import time
from io import open

try:
    import idf_monitor
except ImportError:
    sys.path.append('..')
    import idf_monitor


ELF_FILE = './dummy.elf'  # ELF file used for starting the monitor


def monitor_serial_reader_state(serial_reader, file_to_create):
    """
    The pupose of this wrapper is to monitor the serial reader state of idf_monitor.py. file_to_create is created
    after the serial reader thread has been started. The existence of this file will indicate to
    run_test_idf_monitor.py that idf_monitor.py is ready to process inputs.
    """
    while not serial_reader.serial.is_open or not serial_reader.alive:
        time.sleep(1)
    with open(file_to_create, "w", encoding='utf-8'):
        pass


def main():
    parser = argparse.ArgumentParser('Wrapper for idf_monitor used for testing')
    parser.add_argument('--port')
    parser.add_argument('--print_filter')
    parser.add_argument('--serial_alive_file')
    args = parser.parse_args()

    serial_instance = serial.serial_for_url(args.port, 115200, do_not_open=True)
    monitor = idf_monitor.Monitor(serial_instance, ELF_FILE, args.print_filter, 'make', 'xtensa-esp32-elf-', 'CR')
    sys.stderr.write('Monitor instance has been created.\n')
    monitor_thread = threading.Thread(target=monitor_serial_reader_state,
                                      args=(monitor.serial_reader, args.serial_alive_file))
    monitor_thread.start()
    sys.stderr.write('Monitoring thread has been created.\n')
    monitor.main_loop()
    sys.stderr.write('Monitor loop exited.\n')
    monitor_thread.join(60)
    sys.stderr.write('Monitoring thread joined.\n')


if __name__ == "__main__":
    main()
