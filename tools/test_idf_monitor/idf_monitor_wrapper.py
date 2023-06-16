# SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import sys
import threading
import time

import serial

try:
    import idf_monitor
except ImportError:
    sys.path.append('..')
    import idf_monitor


def monitor_serial_reader_state(serial_reader, file_to_create):
    """
    The purpose of this wrapper is to monitor the serial reader state of idf_monitor.py. file_to_create is created
    after the serial reader thread has been started. The existence of this file will indicate to
    run_test_idf_monitor.py that idf_monitor.py is ready to process inputs.
    """
    while not serial_reader.serial.is_open or not serial_reader.alive:
        time.sleep(1)
    with open(file_to_create, 'w', encoding='utf-8'):
        pass


def main():
    parser = argparse.ArgumentParser('Wrapper for idf_monitor used for testing')
    parser.add_argument('--port')
    parser.add_argument('--print_filter')
    parser.add_argument('--serial_alive_file')
    args = parser.parse_args()

    serial_instance = serial.serial_for_url(args.port, 115200, do_not_open=True)
    monitor = idf_monitor.SerialMonitor(serial_instance, None, args.print_filter)
    sys.stderr.write('Monitor instance has been created.\n')
    monitor_thread = threading.Thread(target=monitor_serial_reader_state,
                                      args=(monitor.serial_reader, args.serial_alive_file))
    monitor_thread.start()
    sys.stderr.write('Monitoring thread has been created.\n')
    monitor.main_loop()
    sys.stderr.write('Monitor loop exited.\n')
    monitor_thread.join(60)
    sys.stderr.write('Monitoring thread joined.\n')


if __name__ == '__main__':
    main()
