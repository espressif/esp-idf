#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2020 Espressif Systems (Shanghai) CO LTD
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
import filecmp
import os
import pexpect
import shutil
import sys
import tempfile
import time
import unittest

current_dir = os.path.dirname(os.path.realpath(__file__))
mkdfu_path = os.path.join(current_dir, '..', 'mkdfu.py')


class TestHelloWorldExample(unittest.TestCase):
    def common_test(self, add_args):
        with tempfile.NamedTemporaryFile(delete=False) as f:
            self.addCleanup(os.unlink, f.name)
        cmd = ' '.join([sys.executable, mkdfu_path, 'write',
                        '-o', f.name,
                        add_args])
        p = pexpect.spawn(cmd, timeout=10)
        self.addCleanup(p.terminate, force=True)

        p.expect_exact(['Adding 1/bootloader.bin at 0x1000',
                        'Adding 1/partition-table.bin at 0x8000',
                        'Adding 1/hello-world.bin at 0x10000',
                        '"{}" has been written. You may proceed with DFU flashing.'.format(f.name)])

        # Need to wait for the process to end because the output file is closed when mkdfu exits.
        # Do non-blocking wait instead of the blocking p.wait():
        for _ in range(10):
            if not p.isalive():
                break
            time.sleep(0.5)
        else:
            p.terminate()

        self.assertTrue(filecmp.cmp(f.name, os.path.join(current_dir, '1','dfu.bin')), 'Output files are different')

    def test_with_json(self):
        self.common_test(' '.join(['--json', os.path.join(current_dir, '1', 'flasher_args.json')]))

    def test_without_json(self):

        self.common_test(' '.join(['0x1000', os.path.join(current_dir, '1', '1.bin'),
                                   '0x8000', os.path.join(current_dir, '1', '2.bin'),
                                   '0x10000', os.path.join(current_dir, '1', '3.bin')
                                   ]))

    def test_filenames(self):
        temp_dir = tempfile.mkdtemp(prefix='very_long_directory_name' * 8)
        self.addCleanup(shutil.rmtree, temp_dir, ignore_errors=True)

        with tempfile.NamedTemporaryFile(dir=temp_dir, delete=False) as f:
            output = f.name

        with tempfile.NamedTemporaryFile(prefix='ľščťžýáíéěř\u0420\u043e\u0441\u0441\u0438\u044f',
                                         dir=temp_dir,
                                         delete=False) as f:
            bootloader = f.name

        shutil.copyfile(os.path.join(current_dir, '1', '1.bin'), bootloader)

        cmd = ' '.join([sys.executable, mkdfu_path, 'write',
                        '-o', output,
                        ' '.join(['0x1000', bootloader,
                                  '0x8000', os.path.join(current_dir, '1', '2.bin'),
                                  '0x10000', os.path.join(current_dir, '1', '3.bin')
                                  ])
                        ])
        p = pexpect.spawn(cmd, timeout=10, encoding='utf-8')
        self.addCleanup(p.terminate, force=True)

        p.expect_exact(['Adding {} at 0x1000'.format(bootloader),
                        'Adding 1/2.bin at 0x8000',
                        'Adding 1/3.bin at 0x10000',
                        '"{}" has been written. You may proceed with DFU flashing.'.format(output)])


if __name__ == '__main__':
    unittest.main()
