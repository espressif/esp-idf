#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import os
import sys
import tempfile
import unittest

import pexpect


class IdfPyQemuTest(unittest.TestCase):
    def test_idf_qemu(self) -> None:
        build_dir = tempfile.mkdtemp()
        idf_path = os.environ['IDF_PATH']
        hello_world_dir = os.path.join(idf_path, 'examples', 'get-started', 'hello_world')
        idf_py = os.path.join(idf_path, 'tools', 'idf.py')
        args = [idf_py, '-C', hello_world_dir, '-B', build_dir,
                'qemu', '--qemu-extra-args', '-no-reboot', 'monitor']
        logfile_name = os.path.join(os.environ['IDF_PATH'], 'qemu_log.out')
        with open(logfile_name, 'w+b') as logfile, \
             pexpect.spawn(sys.executable, args=args, logfile=logfile) as child:
            child.expect('Executing action: all')
            logging.info('Waiting for the build to finish...')
            child.expect('Executing action: qemu', timeout=120)
            child.expect('Generating flash image:')
            child.expect('Generating efuse image:')
            child.expect('Executing action: monitor')
            child.expect('Hello world!')
            child.expect('Restarting in 0 seconds', timeout=20)
            child.expect('Restarting now.')


if __name__ == '__main__':
    unittest.main()
