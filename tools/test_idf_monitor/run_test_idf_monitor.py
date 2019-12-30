#!/usr/bin/env python
#
# Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
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
from __future__ import unicode_literals
from builtins import object
from io import open
import os
import sys
import time
import subprocess
import socket
import pty
import filecmp
import threading
import errno
import tempfile

test_list = (
    # Add new tests here. All files should be placed in IN_DIR. Columns are:
    # Input file            Filter string                                               File with expected output   Timeout
    ('in1.txt',             '',                                                         'in1f1.txt',                60),
    ('in1.txt',             '*:V',                                                      'in1f1.txt',                60),
    ('in1.txt',             'hello_world',                                              'in1f2.txt',                60),
    ('in1.txt',             '*:N',                                                      'in1f3.txt',                60),
    ('in2.txt',             'boot mdf_device_handle:I mesh:E vfs:I',                    'in2f1.txt',               420),
    ('in2.txt',             'vfs',                                                      'in2f2.txt',               420),
)

IN_DIR = 'tests/'       # tests are in this directory
OUT_DIR = 'outputs/'    # test results are written to this directory (kept only for debugging purposes)
ERR_OUT = 'monitor_error_output.'
IDF_MONITOR_WAPPER = 'idf_monitor_wrapper.py'
SERIAL_ALIVE_FILE = '/tmp/serial_alive'  # the existence of this file signalize that idf_monitor is ready to receive

# connection related to communicating with idf_monitor through sockets
HOST = 'localhost'
# blocking socket operations are used with timeout:
SOCKET_TIMEOUT = 30
# the test is restarted after failure (idf_monitor has to be killed):
RETRIES_PER_TEST = 2


def monitor_timeout(process):
    if process.poll() is None:
        # idf_monitor_wrapper is still running
        try:
            process.kill()
            print('\tidf_monitor_wrapper was killed because it did not finish in time.')
        except OSError as e:
            if e.errno == errno.ESRCH:
                # ignores a possible race condition which can occur when the process exits between poll() and kill()
                pass
            else:
                raise


class TestRunner(object):
    def __enter__(self):
        self.serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.serversocket.bind((HOST, 0))
        self.port = self.serversocket.getsockname()[1]
        self.serversocket.listen(5)
        return self

    def __exit__(self, type, value, traceback):
        self.serversocket.shutdown(socket.SHUT_RDWR)
        self.serversocket.close()
        print('Socket was closed successfully')

    def accept_connection(self):
        """ returns a socket for sending the input for idf_monitor which must be closed before calling this again. """
        (clientsocket, address) = self.serversocket.accept()
        # exception will be thrown here if the idf_monitor didn't connect in time
        return clientsocket


def test_iteration(runner, test):
    try:
        # Make sure that the file doesn't exist. It will be recreated by idf_monitor_wrapper.py
        os.remove(SERIAL_ALIVE_FILE)
    except OSError:
        pass
    print('\nRunning test on {} with filter "{}" and expecting {}'.format(test[0], test[1], test[2]))
    try:
        with open(OUT_DIR + test[2], "w", encoding='utf-8') as o_f, \
                tempfile.NamedTemporaryFile(dir=OUT_DIR, prefix=ERR_OUT, mode="w", delete=False) as e_f:
            monitor_cmd = [sys.executable, IDF_MONITOR_WAPPER,
                           '--port', 'socket://{}:{}?logging=debug'.format(HOST, runner.port),
                           '--print_filter', test[1],
                           '--serial_alive_file', SERIAL_ALIVE_FILE]
            (master_fd, slave_fd) = pty.openpty()
            print('\t', ' '.join(monitor_cmd), sep='')
            print('\tstdout="{}" stderr="{}" stdin="{}"'.format(o_f.name, e_f.name, os.ttyname(slave_fd)))
            print('\tMonitor timeout: {} seconds'.format(test[3]))
            start = time.time()
            # the server socket is alive so idf_monitor can start now
            proc = subprocess.Popen(monitor_cmd, stdin=slave_fd, stdout=o_f, stderr=e_f, close_fds=True, bufsize=0)
            # - idf_monitor's stdin needs to be connected to some pseudo-tty in docker image even when it is not
            #   used at all
            # - setting bufsize is needed because the default value is different on Python 2 and 3
            # - the default close_fds is also different on Python 2 and 3
        monitor_watchdog = threading.Timer(test[3], monitor_timeout, [proc])
        monitor_watchdog.start()
        client = runner.accept_connection()
        # The connection is ready but idf_monitor cannot yet receive data (the serial reader thread is not running).
        # This seems to happen on Ubuntu 16.04 LTS and is not related to the version of Python or pyserial.
        # Updating to Ubuntu 18.04 LTS also helps but here, a workaround is used: A wrapper is used for IDF monitor
        # which checks the serial reader thread and creates a file when it is running.
        while not os.path.isfile(SERIAL_ALIVE_FILE) and proc.poll() is None:
            print('\tSerial reader is not ready. Do a sleep...')
            time.sleep(1)
        # Only now can we send the inputs:
        with open(IN_DIR + test[0], 'rb') as f:
            print('\tSending {} to the socket'.format(f.name))
            for chunk in iter(lambda: f.read(1024), b''):
                client.sendall(chunk)
        idf_exit_sequence = b'\x1d\n'
        print('\tSending <exit> to the socket')
        client.sendall(idf_exit_sequence)
        close_end_time = start + 0.75 * test[3]  # time when the process is close to be killed
        while True:
            ret = proc.poll()
            if ret is not None:
                break
            if time.time() > close_end_time:
                # The process isn't finished yet so we are starting to send additional exit sequences because maybe
                # the other end didn't received it.
                print('\tSending additional <exit> to the socket')
                client.sendall(idf_exit_sequence)
            time.sleep(1)
        end = time.time()
        print('\tidf_monitor exited after {:.2f} seconds'.format(end - start))
        if ret < 0:
            raise RuntimeError('idf_monitor was terminated by signal {}'.format(-ret))
        # idf_monitor needs to end before the socket is closed in order to exit without an exception.
    finally:
        if monitor_watchdog:
            monitor_watchdog.cancel()
        os.close(slave_fd)
        os.close(master_fd)
        if client:
            client.close()
        print('\tThe client was closed successfully')
    f1 = IN_DIR + test[2]
    f2 = OUT_DIR + test[2]
    print('\tdiff {} {}'.format(f1, f2))
    if filecmp.cmp(f1, f2, shallow=False):
        print('\tTest has passed')
    else:
        raise RuntimeError("The contents of the files are different. Please examine the artifacts.")


def main():
    gstart = time.time()
    if not os.path.exists(OUT_DIR):
        os.mkdir(OUT_DIR)

    socket.setdefaulttimeout(SOCKET_TIMEOUT)

    for test in test_list:
        for i in range(RETRIES_PER_TEST):
            with TestRunner() as runner:
                # Each test (and each retry) is run with a different port (and server socket). This is done for
                # the CI run where retry with a different socket is necessary to pass the test. According to the
                # experiments, retry with the same port (and server socket) is not sufficient.
                try:
                    test_iteration(runner, test)
                    # no more retries if test_iteration exited without an exception
                    break
                except Exception as e:
                    if i < RETRIES_PER_TEST - 1:
                        print('Test has failed with exception:', e)
                        print('Another attempt will be made.')
                    else:
                        raise

    gend = time.time()
    print('Execution took {:.2f} seconds\n'.format(gend - gstart))


if __name__ == "__main__":
    main()
