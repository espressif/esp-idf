#!/usr/bin/env python
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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
import signal
import time
import subprocess
import pty

test_list = (
        # Add new tests here. All files should be placed in in_dir. Columns are:
        # Input file            Filter string                                               File with expected output
        ('in1.txt',             '',                                                         'in1f1.txt'),
        ('in1.txt',             '*:V',                                                      'in1f1.txt'),
        ('in1.txt',             'hello_world',                                              'in1f2.txt'),
        ('in1.txt',             '*:N',                                                      'in1f3.txt'),
        ('in2.txt',             'boot mdf_device_handle:I mesh:E vfs:I',                    'in2f1.txt'),
        ('in2.txt',             'vfs',                                                      'in2f2.txt'),
        )

in_dir = 'tests/'       # tests are in this directory
out_dir = 'outputs/'    # test results are written to this directory (kept only for debugging purposes)
socat_in = './socatfile'# temporary socat file (deleted after run)
err_out = out_dir + 'monitor_error_output'
elf_file = './dummy.elf'  # ELF file used for starting the monitor
idf_monitor = '{}/tools/idf_monitor.py'.format(os.getenv("IDF_PATH"))

class SocatRunner(object):
    """
    Runs socat in the background for creating a socket.
    """
    def __enter__(self):
        # Wait for a connection on port 2399 and then run "tail" which will send the file content to that port. Tail
        # is used because it can start even when the file doesn't exists and remains running after the file has been
        # processed. This way the idf_monitor can end the communication when it received the content. Using regular
        # "cat" would invoke exception in idf_monitor.
        # Note: "-c 1GB" option is used to force sending the whole file under the assumption that all testing files
        # will be much smaller than 1G.
        # Note: A temporary file socat_in is used in order to be able to start socat only once instead of for each test.
        socat_cmd = ['socat',
                '-U', # unidirectional pipe from file to port
                'TCP4-LISTEN:2399,reuseaddr,fork',
                'exec:"tail -c 1GB -F ' + socat_in + '"']
        print(' '.join(socat_cmd))
        self._socat_process = subprocess.Popen(socat_cmd, preexec_fn=os.setsid) # See __exit__ for os.setsid
        return self

    def __exit__(self, type, value, traceback):
        # self._socat_process.terminate() doesn't enough because each connection to the port starts a new socat and a
        # tail processes
        os.killpg(os.getpgid(self._socat_process.pid), signal.SIGTERM)
        # Note: this terminates all the processes but makes the script UNIX-only

def cleanup():
    try:
        os.remove(socat_in)
    except:
        # ignore if the file doesn't exist
        pass

def main():
    start = time.time()
    cleanup() # avoid sending old content
    if not os.path.exists(out_dir):
        os.mkdir(out_dir)
    try:
        with SocatRunner():
            # Sleep is necessary to make sure that socat is already listening. Only one sleep is used per run (this is
            # another reason while the temporary socat_in file is used instead of directly reading the test files).
            time.sleep(1)
            for t in test_list:
                print('Running test on {} with filter "{}" and expecting {}'.format(t[0], t[1], t[2]))
                with open(in_dir + t[0], "r", encoding='utf-8') as i_f, open(socat_in, "w", encoding='utf-8') as s_f:
                    print('cat {} > {}'.format(i_f.name, s_f.name))
                    for line in i_f:
                        s_f.write(line)
                    idf_exit_sequence = b'\x1d\n'
                    print('echo "<exit>" >> {}'.format(s_f.name))
                    s_f.write(idf_exit_sequence.decode())
                monitor_cmd = [idf_monitor,
                        '--port', 'socket://localhost:2399',
                        '--print_filter', t[1],
                        elf_file]
                with open(out_dir + t[2], "w", encoding='utf-8') as o_f, open(err_out, "w", encoding='utf-8') as e_f:
                    try:
                        (master_fd, slave_fd) = pty.openpty()
                        print(' '.join(monitor_cmd), end=' ')
                        print(' > {} 2> {} < {}'.format(o_f.name, e_f.name, os.ttyname(slave_fd)))
                        proc = subprocess.Popen(monitor_cmd, stdin=slave_fd, stdout=o_f, stderr=e_f,
                                close_fds=True)
                        proc.wait()
                    finally:
                        os.close(slave_fd)
                        os.close(master_fd)
                diff_cmd = ['diff', in_dir + t[2], out_dir + t[2]]
                print(' '.join(diff_cmd))
                subprocess.check_call(diff_cmd)
                print('Test has passed')
    finally:
        cleanup()

    end = time.time()
    print('Execution took {:.2f} seconds'.format(end - start))

if __name__ == "__main__":
    main()
