#!/usr/bin/env python
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import shutil
import sys
import unittest

from test_utils import CFG, generate_test_dir_1, generate_test_dir_2

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
import fatfsgen  # noqa E402
from fatfsgen_utils.exceptions import WriteDirectoryException  # noqa E402
from fatfsgen_utils.exceptions import LowerCaseException, NoFreeClusterException, TooLongNameException  # noqa E402


class FatFSGen(unittest.TestCase):
    def setUp(self) -> None:
        os.makedirs('output_data')
        generate_test_dir_1()
        generate_test_dir_2()

    def tearDown(self) -> None:
        shutil.rmtree('output_data')

    def test_empty_file_sn_fat12(self) -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.create_file('TESTFILE')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = fatfs.read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x2000:0x200c], b'TESTFILE   \x20')  # check entry name and type
        self.assertEqual(file_system[0x1000:0x1006], b'\xf8\xff\xff\xff\x0f\x00')  # check fat

    def test_directory_sn_fat12(self) -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.create_directory('TESTFOLD')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = fatfs.read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x2000:0x200c], b'TESTFOLD   \x10')  # check entry name and type
        self.assertEqual(file_system[0x1000:0x1006], b'\xf8\xff\xff\xff\x0f\x00')  # check fat
        self.assertEqual(file_system[0x6000:0x600c], b'.          \x10')  # reference to itself
        self.assertEqual(file_system[0x6020:0x602c], b'..         \x10')  # reference to parent

    def test_empty_file_with_extension_sn_fat12(self) -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.create_file('TESTF', extension='TXT')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = fatfs.read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x2000:0x200c], b'TESTF   TXT\x20')  # check entry name and type
        self.assertEqual(file_system[0x1000:0x1006], b'\xf8\xff\xff\xff\x0f\x00')  # check fat

    def test_write_to_file_with_extension_sn_fat12(self) -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.create_file('WRITEF', extension='TXT')
        fatfs.write_content(path_from_root=['WRITEF.TXT'], content=b'testcontent')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = fatfs.read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x2000:0x200c], b'WRITEF  TXT\x20')  # check entry name and type
        self.assertEqual(file_system[0x201a:0x2020], b'\x02\x00\x0b\x00\x00\x00')  # check size and cluster ref
        self.assertEqual(file_system[0x1000:0x1006], b'\xf8\xff\xff\xff\x0f\x00')  # check fat
        self.assertEqual(file_system[0x6000:0x600f], b'testcontent\x00\x00\x00\x00')  # check file content

    def test_write_to_file_in_folder_sn_fat12(self) -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.create_directory('TESTFOLD')
        fatfs.create_file('WRITEF', extension='TXT', path_from_root=['TESTFOLD'])
        fatfs.write_content(path_from_root=['TESTFOLD', 'WRITEF.TXT'], content=b'testcontent')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = fatfs.read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x2000:0x200c], b'TESTFOLD   \x10')
        self.assertEqual(
            file_system[0x1000:0x1010],
            b'\xf8\xff\xff\xff\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6040:0x6050], b'WRITEF  TXT\x20\x00\x00\x01\x00')
        self.assertEqual(file_system[0x605a:0x6060], b'\x03\x00\x0b\x00\x00\x00')
        self.assertEqual(file_system[0x7000:0x700b], b'testcontent')  # check file content

    def test_cluster_setting_values(self) -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.create_file('TESTFIL1')
        fatfs.create_file('TESTFIL2')
        fatfs.create_file('TESTFIL3')
        fatfs.create_file('TESTFIL4')
        fatfs.create_file('TESTFIL5')
        fatfs.fat.clusters[2].set_in_fat(1000)
        fatfs.fat.clusters[3].set_in_fat(4)
        fatfs.fat.clusters[4].set_in_fat(5)
        fatfs.write_filesystem(CFG['output_file'])
        file_system = fatfs.read_filesystem(CFG['output_file'])
        self.assertEqual(
            file_system[0x1000:0x1010],
            b'\xf8\xff\xff\xe8\x43\x00\x05\xf0\xff\xff\x0f\x00\x00\x00\x00\x00')

    def test_full_sector_file(self) -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.create_file('WRITEF', extension='TXT')
        fatfs.write_content(path_from_root=['WRITEF.TXT'], content=CFG['sector_size'] * b'a')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = fatfs.read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x1000: 0x100e], b'\xf8\xff\xff\xff\x0f\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6000: 0x7000], CFG['sector_size'] * b'a')

    def test_file_chaining(self) -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.create_file('WRITEF', extension='TXT')
        fatfs.write_content(path_from_root=['WRITEF.TXT'], content=CFG['sector_size'] * b'a' + b'a')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = fatfs.read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x1000: 0x100e], b'\xf8\xff\xff\x03\xf0\xff\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7000: 0x8000], b'a' + (CFG['sector_size'] - 1) * b'\x00')

    def test_full_sector_folder(self) -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.create_directory('TESTFOLD')

        for i in range(CFG['sector_size'] // CFG['entry_size']):
            fatfs.create_file(f'A{str(i).upper()}', path_from_root=['TESTFOLD'])
        fatfs.write_content(path_from_root=['TESTFOLD', 'A0'], content=b'first')
        fatfs.write_content(path_from_root=['TESTFOLD', 'A126'], content=b'later')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = fatfs.read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x1000: 0x10d0],
                         b'\xf8\xff\xff\x82\xf0\xff' + 192 * b'\xff' + 10 * b'\x00')
        self.assertEqual(file_system[0x85000:0x85005], b'later')
        self.assertEqual(file_system[0x86000:0x86010], b'A126        \x00\x00\x01\x00')
        self.assertEqual(file_system[0x86020:0x86030], b'A127        \x00\x00\x01\x00')

    def test_write_to_folder_in_folder_sn_fat12(self) -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.create_directory('TESTFOLD')
        fatfs.create_directory('TESTFOLL', path_from_root=['TESTFOLD'])
        self.assertRaises(WriteDirectoryException, fatfs.write_content, path_from_root=['TESTFOLD', 'TESTFOLL'],
                          content=b'testcontent')

    def test_write_non_existing_file_in_folder_sn_fat12(self) -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.create_directory('TESTFOLD')
        self.assertRaises(FileNotFoundError, fatfs.write_content, path_from_root=['TESTFOLD', 'AHOJ'],
                          content=b'testcontent')

    @staticmethod
    def create_too_many_files() -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.create_directory('TESTFOLD')
        for i in range(2 * CFG['sector_size'] // CFG['entry_size']):
            fatfs.create_file(f'A{str(i).upper()}', path_from_root=['TESTFOLD'])

    def test_too_many_files(self) -> None:
        self.assertRaises(NoFreeClusterException, self.create_too_many_files)

    def test_full_two_sectors_folder(self) -> None:
        fatfs = fatfsgen.FATFS(size=2 * 1024 * 1024)
        fatfs.create_directory('TESTFOLD')

        for i in range(2 * CFG['sector_size'] // CFG['entry_size']):
            fatfs.create_file(f'A{str(i).upper()}', path_from_root=['TESTFOLD'])
        fatfs.write_content(path_from_root=['TESTFOLD', 'A253'], content=b'later')
        fatfs.write_content(path_from_root=['TESTFOLD', 'A255'], content=b'last')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = fatfs.read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x105000:0x105010], b'later\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x108000:0x108010], b'last\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    def test_lower_case_dir_short_names(self) -> None:
        fatfs = fatfsgen.FATFS()
        self.assertRaises(LowerCaseException, fatfs.create_directory, 'testfold')

    def test_lower_case_file_short_names(self) -> None:
        fatfs = fatfsgen.FATFS()
        self.assertRaises(LowerCaseException, fatfs.create_file, 'newfile')

    def test_too_long_name_dir_short_names(self) -> None:
        fatfs = fatfsgen.FATFS()
        self.assertRaises(TooLongNameException, fatfs.create_directory, 'TOOLONGNAME')

    def test_fatfs16_detection(self) -> None:
        fatfs = fatfsgen.FATFS(size=16 * 1024 * 1024)
        self.assertEqual(fatfs.state.fatfs_type, 16)

    def test_fatfs32_detection(self) -> None:
        self.assertRaises(NotImplementedError, fatfsgen.FATFS, size=256 * 1024 * 1024)

    def test_deep_structure(self) -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.create_directory('TESTFOLD')
        fatfs.create_directory('TESTFOLL', path_from_root=['TESTFOLD'])
        fatfs.create_directory('TESTFOLO', path_from_root=['TESTFOLD', 'TESTFOLL'])
        fatfs.create_file('WRITEF', extension='TXT', path_from_root=['TESTFOLD', 'TESTFOLL', 'TESTFOLO'])
        fatfs.write_content(path_from_root=['TESTFOLD', 'TESTFOLL', 'TESTFOLO', 'WRITEF.TXT'], content=b'later')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = fatfs.read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x9000:0x9010], b'later\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    def test_same_name_deep_structure(self) -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.create_directory('TESTFOLD')
        fatfs.create_directory('TESTFOLD', path_from_root=['TESTFOLD'])
        fatfs.create_directory('TESTFOLD', path_from_root=['TESTFOLD', 'TESTFOLD'])
        fatfs.create_file('WRITEF', extension='TXT', path_from_root=['TESTFOLD', 'TESTFOLD', 'TESTFOLD'])
        fatfs.write_content(path_from_root=['TESTFOLD', 'TESTFOLD', 'TESTFOLD', 'WRITEF.TXT'], content=b'later')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = fatfs.read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x2000:0x2010], b'TESTFOLD   \x10\x00\x00\x01\x00')
        self.assertEqual(file_system[0x2010:0x2020], b'!\x00\x00\x00\x00\x00\x01\x00\x01\x00\x02\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6040:0x6050], b'TESTFOLD   \x10\x00\x00\x01\x00')
        self.assertEqual(file_system[0x6040:0x6050], b'TESTFOLD   \x10\x00\x00\x01\x00')

        self.assertEqual(file_system[0x7040:0x7050], b'TESTFOLD   \x10\x00\x00\x01\x00')
        self.assertEqual(file_system[0x8040:0x8050], b'WRITEF  TXT \x00\x00\x01\x00')
        self.assertEqual(file_system[0x9000:0x9010], b'later\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    def test_e2e_deep_folder_into_image(self) -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.generate(CFG['test_dir'])
        fatfs.write_filesystem(CFG['output_file'])
        file_system = fatfs.read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x6060:0x6070], b'TESTFIL2    \x00\x00\x01\x00')
        self.assertEqual(file_system[0x6070:0x6080], b'!\x00\x00\x00\x00\x00\x01\x00\x01\x00\x05\x00\x0b\x00\x00\x00')
        self.assertEqual(file_system[0x7040:0x7050], b'LASTFILE    \x00\x00\x01\x00')
        self.assertEqual(file_system[0x8000:0x8010], b'deeptest\n\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x9000:0x9010], b'thisistest\n\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xa000:0xa010], b'ahoj\n\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    def test_e2e_deep_folder_into_image_ext(self) -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.generate(CFG['test_dir2'])
        fatfs.write_filesystem(CFG['output_file'])
        file_system = fatfs.read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x2020:0x2030], b'TESTFILE    \x00\x00\x01\x00')
        self.assertEqual(file_system[0x6060:0x6070], b'TESTFIL2    \x00\x00\x01\x00')
        self.assertEqual(file_system[0x7000:0x7010], b'.          \x10\x00\x00\x01\x00')
        self.assertEqual(file_system[0x7040:0x7050], b'LASTFILETXT \x00\x00\x01\x00')
        self.assertEqual(file_system[0x8000:0x8010], b'deeptest\n\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x9000:0x9010], b'thisistest\n\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xa000:0xa010], b'ahoj\n\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xb000:0xb009], b'\xff\xff\xff\xff\xff\xff\xff\xff\xff')


if __name__ == '__main__':
    unittest.main()
