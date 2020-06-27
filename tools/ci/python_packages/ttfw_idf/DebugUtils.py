# Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
from io import open
from tiny_test_fw import Utility
import pexpect


class CustomProcess(object):
    def __init__(self, cmd, logfile, verbose=True):
        self.verbose = verbose
        self.f = open(logfile, 'w')
        if self.verbose:
            Utility.console_log('Starting {} > {}'.format(cmd, self.f.name))
        self.pexpect_proc = pexpect.spawn(cmd, timeout=60, logfile=self.f, encoding='utf-8')

    def __enter__(self):
        return self

    def close(self):
        self.pexpect_proc.terminate(force=True)

    def __exit__(self, type, value, traceback):
        self.close()
        self.f.close()


class OCDProcess(CustomProcess):
    def __init__(self, logfile_path, extra_args='', verbose=True):
        # TODO Use configuration file implied by the test environment (board)
        cmd = 'openocd {} -f board/esp32-wrover-kit-3.3v.cfg'.format(extra_args)
        super(OCDProcess, self).__init__(cmd, logfile_path, verbose)
        patterns = ['Info : Listening on port 3333 for gdb connections']

        try:
            while True:
                i = self.pexpect_proc.expect_exact(patterns, timeout=30)
                # TIMEOUT or EOF exceptions will be thrown upon other errors
                if i == 0:
                    if self.verbose:
                        Utility.console_log('openocd is listening for gdb connections')
                    break  # success
        except Exception:
            if self.verbose:
                Utility.console_log('openocd initialization has failed', 'R')
            raise

    def close(self):
        try:
            self.pexpect_proc.sendcontrol('c')
            self.pexpect_proc.expect_exact('shutdown command invoked')
        except Exception:
            if self.verbose:
                Utility.console_log('openocd needs to be killed', 'O')
            super(OCDProcess, self).close()


class GDBProcess(CustomProcess):
    def __init__(self, logfile_path, elffile_path, target, extra_args='', verbose=True):
        cmd = 'xtensa-{}-elf-gdb {} {}'.format(target, extra_args, elffile_path)
        super(GDBProcess, self).__init__(cmd, logfile_path, verbose)

    def close(self):
        try:
            self.pexpect_proc.sendline('q')
            self.pexpect_proc.expect_exact('Quit anyway? (y or n)')
            self.pexpect_proc.sendline('y')
            self.pexpect_proc.expect_exact('Ending remote debugging.')
        except Exception:
            if self.verbose:
                Utility.console_log('gdb needs to be killed', 'O')
            super(GDBProcess, self).close()


class TelnetProcess(CustomProcess):
    def __init__(self, logfile_path, host='localhost', port=4444, verbose=True):
        cmd = 'telnet {} {}'.format(host, port)
        super(TelnetProcess, self).__init__(cmd, logfile_path, verbose)

    def close(self):
        try:
            self.pexpect_proc.sendline('exit')
            self.pexpect_proc.expect_exact('Connection closed by foreign host.')
        except Exception:
            if self.verbose:
                Utility.console_log('telnet needs to be killed', 'O')
            super(TelnetProcess, self).close()
