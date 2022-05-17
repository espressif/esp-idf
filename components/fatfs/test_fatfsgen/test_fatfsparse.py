#!/usr/bin/env python
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import shutil
import unittest
from subprocess import STDOUT, run

from test_utils import generate_test_dir_2


class FatFSGen(unittest.TestCase):
    def setUp(self) -> None:
        os.makedirs('output_data')
        generate_test_dir_2()

    def tearDown(self) -> None:
        shutil.rmtree('output_data')
        shutil.rmtree('Espressif')

        if os.path.exists('fatfs_image.img'):
            os.remove('fatfs_image.img')

    @staticmethod
    def test_gen_parse() -> None:
        run([
            'python',
            f'{os.path.join(os.path.dirname(__file__), "..", "fatfsgen.py")}',
            'output_data/tst_str'
        ], stderr=STDOUT)

        run(['python', '../fatfsgen.py', 'output_data/tst_str'], stderr=STDOUT)
        run(['python', '../fatfsparse.py', 'fatfs_image.img'], stderr=STDOUT)

        assert set(os.listdir('Espressif')) == {'TEST', 'TESTFILE'}
        with open('Espressif/TESTFILE', 'rb') as in_:
            assert in_.read() == b'ahoj\n'

        assert set(os.listdir('Espressif/TEST')) == {'TEST', 'TESTFIL2'}
        with open('Espressif/TEST/TESTFIL2', 'rb') as in_:
            assert in_.read() == b'thisistest\n'
        assert set(os.listdir('Espressif/TEST/TEST')) == {'LASTFILE.TXT'}

        with open('Espressif/TEST/TEST/LASTFILE.TXT', 'rb') as in_:
            assert in_.read() == b'deeptest\n'


if __name__ == '__main__':
    unittest.main()
