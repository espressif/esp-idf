#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0


import collections
import filecmp
import json
import os
import shutil
import sys
import tempfile
import unittest

import pexpect

current_dir = os.path.dirname(os.path.realpath(__file__))
mkdfu_path = os.path.join(current_dir, '..', 'mkdfu.py')


class TestMkDFU(unittest.TestCase):
    def common_test(self, json_input=None, file_args=[], output_to_compare=None, part_size=None):
        """
        - json_input - input JSON file compatible with mkdfu.py - used when not None
        - file_args - list of (address, path_to_file) tuples
        - output_to_compare - path to the file containing the expected output - tested when not None
        - part_size - partition size - used when not None
        """
        with tempfile.NamedTemporaryFile(delete=False) as f_out:
            self.addCleanup(os.unlink, f_out.name)
        args = [mkdfu_path, 'write', '-o', f_out.name, '--pid', '2', '--flash-size', '4MB']
        if part_size:
            args += ['--part-size', str(part_size)]
        if json_input:
            args += ['--json', json_input]
        for addr, f_path in file_args:
            args += [str(addr), f_path]
        env = os.environ.copy()
        p = pexpect.spawn(sys.executable, args, timeout=10, encoding='utf-8', env=env, dimensions=(40, 1000))
        self.addCleanup(p.terminate, force=True)

        p.expect(pexpect.EOF, timeout=30)
        output = p.before or ''

        self.assertIn('Adding flash chip parameters file with flash_size = 4MB', output)

        for addr, f_path in sorted(file_args, key=lambda e: e[0]):
            self.assertIn(f'Adding {f_path} at {addr:#x}', output)

        self.assertIn(f'"{f_out.name}" has been written. You may proceed with DFU flashing.', output)

        if output_to_compare:
            self.assertTrue(
                filecmp.cmp(f_out.name, os.path.join(current_dir, output_to_compare)), 'Output files are different'
            )


class TestHelloWorldExample(TestMkDFU):
    """
    tests with images prepared in the "1" subdirectory
    """

    def test_with_json(self):
        with tempfile.NamedTemporaryFile(mode='w', dir=os.path.join(current_dir, '1'), delete=False) as f:
            self.addCleanup(os.unlink, f.name)

            bins = [('0x1000', '1.bin'), ('0x8000', '2.bin'), ('0x10000', '3.bin')]
            json.dump({'flash_files': collections.OrderedDict(bins)}, f)

        self.common_test(json_input=f.name, output_to_compare='1/dfu.bin')

    def test_without_json(self):
        self.common_test(
            file_args=[(0x1000, '1/1.bin'), (0x8000, '1/2.bin'), (0x10000, '1/3.bin')], output_to_compare='1/dfu.bin'
        )

    def test_filenames(self):
        temp_dir = tempfile.mkdtemp(prefix='very_long_directory_name' * 8)
        self.addCleanup(shutil.rmtree, temp_dir, ignore_errors=True)

        with tempfile.NamedTemporaryFile(
            prefix='ľščťžýáíéěř\u0420\u043e\u0441\u0441\u0438\u044f', dir=temp_dir, delete=False
        ) as f:
            bootloader = f.name

        shutil.copyfile(os.path.join(current_dir, '1', '1.bin'), bootloader)

        self.common_test(
            file_args=[
                (0x1000, bootloader),
                (0x8000, os.path.join(current_dir, '1', '2.bin')),
                (0x10000, os.path.join(current_dir, '1', '3.bin')),
            ]
        )


class TestSplit(TestMkDFU):
    """
    tests with images prepared in the "2" subdirectory

    "2/dfu.bin" was prepared with:
        mkdfu.py write --part-size 5 --pid 2 --flash-size 4MB -o 2/dfu.bin 0 bin
    where the content of "bin" is b"\xce" * 10
    """

    def test_split(self):
        temp_dir = tempfile.mkdtemp(dir=current_dir)
        self.addCleanup(shutil.rmtree, temp_dir, ignore_errors=True)

        with open(os.path.join(temp_dir, 'bin'), 'wb') as f:
            self.addCleanup(os.unlink, f.name)
            f.write(b'\xce' * 10)

        self.common_test(file_args=[(0, f.name)], part_size=5, output_to_compare='2/dfu.bin')


if __name__ == '__main__':
    unittest.main()
