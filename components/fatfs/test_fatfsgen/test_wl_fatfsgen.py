#!/usr/bin/env python
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import shutil
import sys
import unittest

from test_utils import CFG, generate_test_dir_1, generate_test_dir_2

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
import wl_fatfsgen  # noqa E402  # pylint: disable=C0413
from fatfs_utils.exceptions import WLNotInitialized  # noqa E402  # pylint: disable=C0413


class WLFatFSGen(unittest.TestCase):
    def setUp(self) -> None:
        os.makedirs('output_data')
        generate_test_dir_1()
        generate_test_dir_2()

    def tearDown(self) -> None:
        shutil.rmtree('output_data')

    def test_empty_file_sn_fat12(self) -> None:
        fatfs = wl_fatfsgen.WLFATFS()
        fatfs.plain_fatfs.create_file('TESTFILE')
        fatfs.init_wl()
        fatfs.wl_write_filesystem(CFG['output_file'])
        with open(CFG['output_file'], 'rb') as fs_file:
            file_system = fs_file.read()

        self.assertEqual(file_system[0x3000:0x300c], b'TESTFILE   \x20')  # check entry name and type
        self.assertEqual(file_system[0x2000:0x2006], b'\xf8\xff\xff\xff\x0f\x00')  # check fat

    def test_directory_sn_fat12(self) -> None:
        fatfs = wl_fatfsgen.WLFATFS(device_id=3750448905)
        fatfs.plain_fatfs.create_directory('TESTFOLD')
        fatfs.init_wl()

        fatfs.wl_write_filesystem(CFG['output_file'])
        with open(CFG['output_file'], 'rb') as fs_file:
            file_system = fs_file.read()

        # boot sector
        self.assertEqual(file_system[0x1000:0x1010], b'\xeb\xfe\x90MSDOS5.0\x00\x10\x01\x01\x00')
        self.assertEqual(file_system[0x1010:0x1020], b'\x01\x00\x02\xfa\x00\xf8\x01\x00?\x00\xff\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x102b:0x1034], b'Espressif')

        self.assertEqual(file_system[0x3000:0x300c], b'TESTFOLD   \x10')  # check entry name and type
        self.assertEqual(file_system[0x2000:0x2006], b'\xf8\xff\xff\xff\x0f\x00')  # check fat
        self.assertEqual(file_system[0x7000:0x700c], b'.          \x10')  # reference to itself
        self.assertEqual(file_system[0x7020:0x702c], b'..         \x10')  # reference to parent

        # check state1
        self.assertEqual(file_system[0xfb000:0xfb00f], b'\x00\x00\x00\x00\xfb\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xfb010:0xfb020], b'\x10\x00\x00\x00\x00\x10\x00\x00\x02\x00\x00\x00\tO\x8b\xdf')
        self.assertEqual(file_system[0xfb020:0xfb02f], b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xfb031:0xfb040], b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xd4\xa1\x94i')

        # check state2
        self.assertEqual(file_system[0xfd000:0xfd00f], b'\x00\x00\x00\x00\xfb\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xfd010:0xfd020], b'\x10\x00\x00\x00\x00\x10\x00\x00\x02\x00\x00\x00\tO\x8b\xdf')
        self.assertEqual(file_system[0xfd020:0xfd02f], b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xfd031:0xfd040], b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xd4\xa1\x94i')

        # check config
        self.assertEqual(file_system[0xff001:0xff010], b'\x00\x00\x00\x00\x00\x10\x00\x00\x10\x00\x00\x00\x10\x00\x00')
        self.assertEqual(file_system[0xff010:0xff01f], b'\x10\x00\x00\x00\x10\x00\x00\x00\x02\x00\x00\x00 \x00\x00')
        self.assertEqual(file_system[0xff020:0xff030], b'\xe0b\xb5O\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xff030:0xff03f], b'\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff')

    def test_directory_sn_fat122mb(self) -> None:
        fatfs = wl_fatfsgen.WLFATFS(device_id=3750448905, size=2 * 1024 * 1024)
        fatfs.plain_fatfs.create_directory('TESTFOLD')
        fatfs.init_wl()

        fatfs.wl_write_filesystem(CFG['output_file'])
        with open(CFG['output_file'], 'rb') as fs_file:
            file_system = fs_file.read()

        # check state1
        self.assertEqual(file_system[0x1f9000:0x1f900e], b'\x00\x00\x00\x00\xf9\x01\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x1f9010:0x1f9020],
                         b'\x10\x00\x00\x00\x00\x10\x00\x00\x02\x00\x00\x00\tO\x8b\xdf')
        self.assertEqual(file_system[0x1f9020:0x1f902e], b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x1f9030:0x1f9040], b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00j5\xbdp')

        # check state2
        self.assertEqual(file_system[0x1fc000:0x1fc00e], b'\x00\x00\x00\x00\xf9\x01\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x1fc010:0x1fc020],
                         b'\x10\x00\x00\x00\x00\x10\x00\x00\x02\x00\x00\x00\tO\x8b\xdf')
        self.assertEqual(file_system[0x1fc020:0x1fc02e], b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x1fc030:0x1fc040], b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00j5\xbdp')

        # check config
        self.assertEqual(file_system[0x1ff000:0x1ff00f], b'\x00\x00\x00\x00\x00\x00 \x00\x00\x10\x00\x00\x00\x10\x00')
        self.assertEqual(file_system[0x1ff010:0x1ff01f], b'\x10\x00\x00\x00\x10\x00\x00\x00\x02\x00\x00\x00 \x00\x00')
        self.assertEqual(file_system[0x1ff020:0x1ff030], b')\x892j\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x1ff030:0x1ff03e], b'\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff')

    def test_write_not_initialized_wlfatfs(self) -> None:
        fatfs = wl_fatfsgen.WLFATFS()
        fatfs.plain_fatfs.create_directory('TESTFOLD')
        self.assertRaises(WLNotInitialized, fatfs.wl_write_filesystem, CFG['output_file'])

    def test_e2e_deep_folder_into_image_ext(self) -> None:
        fatfs = wl_fatfsgen.WLFATFS()
        fatfs.plain_fatfs.generate(CFG['test_dir2'])
        fatfs.init_wl()
        fatfs.wl_write_filesystem(CFG['output_file'])
        with open(CFG['output_file'], 'rb') as fs_file:
            file_system = bytearray(fs_file.read())

        self.assertEqual(file_system[0x3020:0x3030], b'TESTFILE    \x00\x00\x00\x00')
        self.assertEqual(file_system[0x7060:0x7070], b'TESTFIL2    \x00\x00\x00\x00')
        self.assertEqual(file_system[0x8000:0x8010], b'.          \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x8040:0x8050], b'LASTFILETXT \x00\x00\x00\x00')
        self.assertEqual(file_system[0x9000:0x9010], b'deeptest\n\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xa000:0xa010], b'thisistest\n\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xb000:0xb010], b'ahoj\n\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xc000:0xc009], b'\xff\xff\xff\xff\xff\xff\xff\xff\xff')

    def test_e2e_deep_folder_into_image(self) -> None:
        fatfs = wl_fatfsgen.WLFATFS()
        fatfs.plain_fatfs.generate(CFG['test_dir'])
        fatfs.init_wl()
        fatfs.wl_write_filesystem(CFG['output_file'])
        with open(CFG['output_file'], 'rb') as fs_file:
            file_system = bytearray(fs_file.read())

        self.assertEqual(file_system[0x7060:0x7070], b'TESTFIL2    \x00\x00\x00\x00')
        self.assertEqual(file_system[0x7070:0x7080], b'!\x00!\x00\x00\x00\x00\x00!\x00\x05\x00\x0b\x00\x00\x00')
        self.assertEqual(file_system[0x8040:0x8050], b'LASTFILE    \x00\x00\x00\x00')
        self.assertEqual(file_system[0x9000:0x9010], b'deeptest\n\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xa000:0xa010], b'thisistest\n\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xb000:0xb010], b'ahoj\n\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')


if __name__ == '__main__':
    unittest.main()
