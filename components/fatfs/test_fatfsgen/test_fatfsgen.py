#!/usr/bin/env python
# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import shutil
import sys
import unittest
from subprocess import STDOUT, check_output

from test_utils import CFG, fill_sector, generate_test_dir_1, generate_test_dir_2

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
import fatfsgen  # noqa E402  # pylint: disable=C0413
from fatfs_utils.boot_sector import BootSector  # noqa E402  # pylint: disable=C0413
from fatfs_utils.cluster import Cluster  # noqa E402  # pylint: disable=C0413
from fatfs_utils.entry import Entry  # noqa E402  # pylint: disable=C0413
from fatfs_utils.exceptions import InconsistentFATAttributes  # noqa E402  # pylint: disable=C0413
from fatfs_utils.exceptions import NotInitialized  # noqa E402  # pylint: disable=C0413
from fatfs_utils.exceptions import TooLongNameException  # noqa E402  # pylint: disable=C0413
from fatfs_utils.exceptions import WriteDirectoryException  # noqa E402  # pylint: disable=C0413
from fatfs_utils.exceptions import LowerCaseException, NoFreeClusterException  # noqa E402  # pylint: disable=C0413
from fatfs_utils.utils import right_strip_string  # noqa E402  # pylint: disable=C0413
from fatfs_utils.utils import FAT12, read_filesystem  # noqa E402  # pylint: disable=C0413


class FatFSGen(unittest.TestCase):
    def setUp(self) -> None:
        os.makedirs('output_data')
        os.makedirs('test_dir')
        generate_test_dir_1()
        generate_test_dir_2()

    def tearDown(self) -> None:
        shutil.rmtree('output_data')
        shutil.rmtree('test_dir')

        if os.path.exists('fatfs_image.img'):
            os.remove('fatfs_image.img')

    def test_empty_file_sn_fat12(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.create_file('TESTFILE')

        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x3000:0x300c], b'TESTFILE   \x20')  # check entry name and type
        self.assertEqual(file_system[0x1000:0x1006], b'\xf8\xff\xff\xff\x0f\x00')  # check fat

    def test_empty_file_sn_fat12_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1)
        fatfs.create_file('TESTFILE')

        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x2000:0x200c], b'TESTFILE   \x20')  # check entry name and type
        self.assertEqual(file_system[0x1000:0x1006], b'\xf8\xff\xff\xff\x0f\x00')  # check fat

    def test_directory_sn_fat12(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.create_directory('TESTFOLD')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x3000:0x300c], b'TESTFOLD   \x10')  # check entry name and type
        self.assertEqual(file_system[0x1000:0x1006], b'\xf8\xff\xff\xff\x0f\x00')  # check fat
        self.assertEqual(file_system[0x7000:0x700c], b'.          \x10')  # reference to itself
        self.assertEqual(file_system[0x7020:0x702c], b'..         \x10')  # reference to parent

    def test_directory_sn_fat12_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1)
        fatfs.create_directory('TESTFOLD')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x2000:0x200c], b'TESTFOLD   \x10')  # check entry name and type
        self.assertEqual(file_system[0x1000:0x1006], b'\xf8\xff\xff\xff\x0f\x00')  # check fat
        self.assertEqual(file_system[0x6000:0x600c], b'.          \x10')  # reference to itself
        self.assertEqual(file_system[0x6020:0x602c], b'..         \x10')  # reference to parent

    def test_empty_file_with_extension_sn_fat12(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.create_file('TESTF', extension='TXT')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x3000:0x300c], b'TESTF   TXT\x20')  # check entry name and type
        self.assertEqual(file_system[0x1000:0x1006], b'\xf8\xff\xff\xff\x0f\x00')  # check fat

    def test_empty_file_with_extension_sn_fat12_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1)
        fatfs.create_file('TESTF', extension='TXT')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x2000:0x200c], b'TESTF   TXT\x20')  # check entry name and type
        self.assertEqual(file_system[0x1000:0x1006], b'\xf8\xff\xff\xff\x0f\x00')  # check fat

    def test_write_to_file_with_extension_sn_fat12(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.create_file('WRITEF', extension='TXT')
        fatfs.write_content(path_from_root=['WRITEF.TXT'], content=b'testcontent')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x3000:0x300c], b'WRITEF  TXT\x20')  # check entry name and type
        self.assertEqual(file_system[0x301a:0x3020], b'\x02\x00\x0b\x00\x00\x00')  # check size and cluster ref
        self.assertEqual(file_system[0x1000:0x1006], b'\xf8\xff\xff\xff\x0f\x00')  # check fat
        self.assertEqual(file_system[0x7000:0x700f], b'testcontent\x00\x00\x00\x00')  # check file content

    def test_write_to_file_with_extension_sn_fat12_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1)
        fatfs.create_file('WRITEF', extension='TXT')
        fatfs.write_content(path_from_root=['WRITEF.TXT'], content=b'testcontent')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x2000:0x200c], b'WRITEF  TXT\x20')  # check entry name and type
        self.assertEqual(file_system[0x201a:0x2020], b'\x02\x00\x0b\x00\x00\x00')  # check size and cluster ref
        self.assertEqual(file_system[0x1000:0x1006], b'\xf8\xff\xff\xff\x0f\x00')  # check fat
        self.assertEqual(file_system[0x6000:0x600f], b'testcontent\x00\x00\x00\x00')  # check file content

    def test_write_to_file_in_folder_sn_fat12(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.create_directory('TESTFOLD')
        fatfs.create_file('WRITEF', extension='TXT', path_from_root=['TESTFOLD'])
        fatfs.write_content(path_from_root=['TESTFOLD', 'WRITEF.TXT'], content=b'testcontent')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x3000:0x300c], b'TESTFOLD   \x10')
        self.assertEqual(
            file_system[0x1000:0x1010],
            b'\xf8\xff\xff\xff\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7040:0x7050], b'WRITEF  TXT\x20\x00\x00\x00\x00')
        self.assertEqual(file_system[0x705a:0x7060], b'\x03\x00\x0b\x00\x00\x00')
        self.assertEqual(file_system[0x8000:0x800b], b'testcontent')  # check file content

    def test_write_to_file_in_folder_sn_fat12_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1)
        fatfs.create_directory('TESTFOLD')
        fatfs.create_file('WRITEF', extension='TXT', path_from_root=['TESTFOLD'])
        fatfs.write_content(path_from_root=['TESTFOLD', 'WRITEF.TXT'], content=b'testcontent')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x2000:0x200c], b'TESTFOLD   \x10')
        self.assertEqual(
            file_system[0x1000:0x1010],
            b'\xf8\xff\xff\xff\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6040:0x6050], b'WRITEF  TXT\x20\x00\x00\x00\x00')
        self.assertEqual(file_system[0x605a:0x6060], b'\x03\x00\x0b\x00\x00\x00')
        self.assertEqual(file_system[0x7000:0x700b], b'testcontent')  # check file content

    def test_cluster_setting_values(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.create_file('TESTFIL1')
        fatfs.create_file('TESTFIL2')
        fatfs.create_file('TESTFIL3')
        fatfs.create_file('TESTFIL4')
        fatfs.create_file('TESTFIL5')
        fatfs.fat.clusters[2].set_in_fat(1000)
        fatfs.fat.clusters[3].set_in_fat(4)
        fatfs.fat.clusters[4].set_in_fat(5)
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(
            file_system[0x1000:0x1010],
            b'\xf8\xff\xff\xe8\x43\x00\x05\xf0\xff\xff\x0f\x00\x00\x00\x00\x00')

    def test_full_sector_file(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.create_file('WRITEF', extension='TXT')
        fatfs.write_content(path_from_root=['WRITEF.TXT'], content=CFG['sector_size'] * b'a')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x1000: 0x100e], b'\xf8\xff\xff\xff\x0f\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7000: 0x8000], CFG['sector_size'] * b'a')

    def test_full_sector_file_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1)
        fatfs.create_file('WRITEF', extension='TXT')
        fatfs.write_content(path_from_root=['WRITEF.TXT'], content=CFG['sector_size'] * b'a')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x1000: 0x100e], b'\xf8\xff\xff\xff\x0f\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6000: 0x7000], CFG['sector_size'] * b'a')

    def test_file_chaining(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.create_file('WRITEF', extension='TXT')
        fatfs.write_content(path_from_root=['WRITEF.TXT'], content=CFG['sector_size'] * b'a' + b'a')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x1000: 0x100e], b'\xf8\xff\xff\x03\xf0\xff\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x8000: 0x9000], b'a' + (CFG['sector_size'] - 1) * b'\x00')

    def test_file_chaining_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1)
        fatfs.create_file('WRITEF', extension='TXT')
        fatfs.write_content(path_from_root=['WRITEF.TXT'], content=CFG['sector_size'] * b'a' + b'a')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x1000: 0x100e], b'\xf8\xff\xff\x03\xf0\xff\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7000: 0x8000], b'a' + (CFG['sector_size'] - 1) * b'\x00')

    def test_full_sector_folder(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.create_directory('TESTFOLD')

        fill_sector(fatfs)
        fatfs.write_content(path_from_root=['TESTFOLD', 'A0'], content=b'first')
        fatfs.write_content(path_from_root=['TESTFOLD', 'A126'], content=b'later')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x1000: 0x10d0],
                         b'\xf8\xff\xff\x82\xf0\xff' + 192 * b'\xff' + 10 * b'\x00')
        self.assertEqual(file_system[0x86000:0x86005], b'later')
        self.assertEqual(file_system[0x87000:0x87010], b'A126        \x00\x00\x00\x00')
        self.assertEqual(file_system[0x87020:0x87030], b'A127        \x00\x00\x00\x00')

    def test_full_sector_folder_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1)
        fatfs.create_directory('TESTFOLD')

        fill_sector(fatfs)
        fatfs.write_content(path_from_root=['TESTFOLD', 'A0'], content=b'first')
        fatfs.write_content(path_from_root=['TESTFOLD', 'A126'], content=b'later')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x1000: 0x10d0],
                         b'\xf8\xff\xff\x82\xf0\xff' + 192 * b'\xff' + 10 * b'\x00')
        self.assertEqual(file_system[0x85000:0x85005], b'later')
        self.assertEqual(file_system[0x86000:0x86010], b'A126        \x00\x00\x00\x00')
        self.assertEqual(file_system[0x86020:0x86030], b'A127        \x00\x00\x00\x00')

    def test_write_to_folder_in_folder_sn_fat12(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.create_directory('TESTFOLD')
        fatfs.create_directory('TESTFOLL', path_from_root=['TESTFOLD'])
        self.assertRaises(WriteDirectoryException, fatfs.write_content, path_from_root=['TESTFOLD', 'TESTFOLL'],
                          content=b'testcontent')

    def test_write_non_existing_file_in_folder_sn_fat12(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.create_directory('TESTFOLD')
        self.assertRaises(FileNotFoundError, fatfs.write_content, path_from_root=['TESTFOLD', 'AHOJ'],
                          content=b'testcontent')

    @staticmethod
    def create_too_many_files() -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.create_directory('TESTFOLD')
        for i in range(2 * CFG['sector_size'] // CFG['entry_size']):
            fatfs.create_file(f'A{str(i).upper()}', path_from_root=['TESTFOLD'])

    def test_too_many_files(self) -> None:
        self.assertRaises(NoFreeClusterException, self.create_too_many_files)

    def test_full_two_sectors_folder(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2, size=2 * 1024 * 1024)
        fatfs.create_directory('TESTFOLD')

        for i in range(2 * CFG['sector_size'] // CFG['entry_size']):
            fatfs.create_file(f'A{str(i).upper()}', path_from_root=['TESTFOLD'])
        fatfs.write_content(path_from_root=['TESTFOLD', 'A253'], content=b'later')
        fatfs.write_content(path_from_root=['TESTFOLD', 'A255'], content=b'last')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x106000:0x106010], b'later\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x109000:0x109010], b'last\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    def test_full_two_sectors_folder_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1, size=2 * 1024 * 1024)
        fatfs.create_directory('TESTFOLD')

        for i in range(2 * CFG['sector_size'] // CFG['entry_size']):
            fatfs.create_file(f'A{str(i).upper()}', path_from_root=['TESTFOLD'])
        fatfs.write_content(path_from_root=['TESTFOLD', 'A253'], content=b'later')
        fatfs.write_content(path_from_root=['TESTFOLD', 'A255'], content=b'last')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x105000:0x105010], b'later\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x108000:0x108010], b'last\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    def test_lower_case_dir_short_names(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        self.assertRaises(LowerCaseException, fatfs.create_directory, 'testfold')

    def test_lower_case_file_short_names(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        self.assertRaises(LowerCaseException, fatfs.create_file, 'newfile')

    def test_too_long_name_dir_short_names(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        self.assertRaises(TooLongNameException, fatfs.create_directory, 'TOOLONGNAME')

    def test_fatfs16_detection(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2, size=16 * 1024 * 1024)
        self.assertEqual(fatfs.state.boot_sector_state.fatfs_type, 16)

    def test_fatfs32_detection(self) -> None:
        self.assertRaises(NotImplementedError, fatfsgen.FATFS, size=256 * 1024 * 1024)

    def test_deep_structure(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.create_directory('TESTFOLD')
        fatfs.create_directory('TESTFOLL', path_from_root=['TESTFOLD'])
        fatfs.create_directory('TESTFOLO', path_from_root=['TESTFOLD', 'TESTFOLL'])
        fatfs.create_file('WRITEF', extension='TXT', path_from_root=['TESTFOLD', 'TESTFOLL', 'TESTFOLO'])
        fatfs.write_content(path_from_root=['TESTFOLD', 'TESTFOLL', 'TESTFOLO', 'WRITEF.TXT'], content=b'later')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0xa000:0xa010], b'later\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    def test_deep_structure_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1)
        fatfs.create_directory('TESTFOLD')
        fatfs.create_directory('TESTFOLL', path_from_root=['TESTFOLD'])
        fatfs.create_directory('TESTFOLO', path_from_root=['TESTFOLD', 'TESTFOLL'])
        fatfs.create_file('WRITEF', extension='TXT', path_from_root=['TESTFOLD', 'TESTFOLL', 'TESTFOLO'])
        fatfs.write_content(path_from_root=['TESTFOLD', 'TESTFOLL', 'TESTFOLO', 'WRITEF.TXT'], content=b'later')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x9000:0x9010], b'later\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    def test_same_name_deep_structure(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.create_directory('TESTFOLD')
        fatfs.create_directory('TESTFOLD', path_from_root=['TESTFOLD'])
        fatfs.create_directory('TESTFOLD', path_from_root=['TESTFOLD', 'TESTFOLD'])
        fatfs.create_file('WRITEF', extension='TXT', path_from_root=['TESTFOLD', 'TESTFOLD', 'TESTFOLD'])
        fatfs.write_content(path_from_root=['TESTFOLD', 'TESTFOLD', 'TESTFOLD', 'WRITEF.TXT'], content=b'later')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x3000:0x3010], b'TESTFOLD   \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x3010:0x3020], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7040:0x7050], b'TESTFOLD   \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7040:0x7050], b'TESTFOLD   \x10\x00\x00\x00\x00')

        self.assertEqual(file_system[0x8040:0x8050], b'TESTFOLD   \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x9040:0x9050], b'WRITEF  TXT \x00\x00\x00\x00')
        self.assertEqual(file_system[0xa000:0xa010], b'later\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    def test_same_name_deep_structure_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1)
        fatfs.create_directory('TESTFOLD')
        fatfs.create_directory('TESTFOLD', path_from_root=['TESTFOLD'])
        fatfs.create_directory('TESTFOLD', path_from_root=['TESTFOLD', 'TESTFOLD'])
        fatfs.create_file('WRITEF', extension='TXT', path_from_root=['TESTFOLD', 'TESTFOLD', 'TESTFOLD'])
        fatfs.write_content(path_from_root=['TESTFOLD', 'TESTFOLD', 'TESTFOLD', 'WRITEF.TXT'], content=b'later')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x2000:0x2010], b'TESTFOLD   \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x2010:0x2020], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6040:0x6050], b'TESTFOLD   \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6040:0x6050], b'TESTFOLD   \x10\x00\x00\x00\x00')

        self.assertEqual(file_system[0x7040:0x7050], b'TESTFOLD   \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x8040:0x8050], b'WRITEF  TXT \x00\x00\x00\x00')
        self.assertEqual(file_system[0x9000:0x9010], b'later\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    def test_e2e_deep_folder_into_image(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.generate(CFG['test_dir'])
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x7060:0x7070], b'TESTFIL2    \x00\x00\x00\x00')
        self.assertEqual(file_system[0x7070:0x7080], b'!\x00!\x00\x00\x00\x00\x00!\x00\x05\x00\x0b\x00\x00\x00')
        self.assertEqual(file_system[0x8040:0x8050], b'LASTFILE    \x00\x00\x00\x00')
        self.assertEqual(file_system[0x9000:0x9010], b'deeptest\n\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xa000:0xa010], b'thisistest\n\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xb000:0xb010], b'ahoj\n\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    def test_e2e_deep_folder_into_image_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1)
        fatfs.generate(CFG['test_dir'])
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x6060:0x6070], b'TESTFIL2    \x00\x00\x00\x00')
        self.assertEqual(file_system[0x6070:0x6080], b'!\x00!\x00\x00\x00\x00\x00!\x00\x05\x00\x0b\x00\x00\x00')
        self.assertEqual(file_system[0x7040:0x7050], b'LASTFILE    \x00\x00\x00\x00')
        self.assertEqual(file_system[0x8000:0x8010], b'deeptest\n\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x9000:0x9010], b'thisistest\n\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xa000:0xa010], b'ahoj\n\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    def test_e2e_deep_folder_into_image_ext(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.generate(CFG['test_dir2'])
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x3020:0x3030], b'TESTFILE    \x00\x00\x00\x00')
        self.assertEqual(file_system[0x7060:0x7070], b'TESTFIL2    \x00\x00\x00\x00')
        self.assertEqual(file_system[0x8000:0x8010], b'.          \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x8040:0x8050], b'LASTFILETXT \x00\x00\x00\x00')
        self.assertEqual(file_system[0x9000:0x9010], b'deeptest\n\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xa000:0xa010], b'thisistest\n\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xb000:0xb010], b'ahoj\n\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xc000:0xc009], b'\xff\xff\xff\xff\xff\xff\xff\xff\xff')

    def test_e2e_deep_folder_into_image_ext_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1)
        fatfs.generate(CFG['test_dir2'])
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x2020:0x2030], b'TESTFILE    \x00\x00\x00\x00')
        self.assertEqual(file_system[0x6060:0x6070], b'TESTFIL2    \x00\x00\x00\x00')
        self.assertEqual(file_system[0x7000:0x7010], b'.          \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7040:0x7050], b'LASTFILETXT \x00\x00\x00\x00')
        self.assertEqual(file_system[0x8000:0x8010], b'deeptest\n\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x9000:0x9010], b'thisistest\n\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xa000:0xa010], b'ahoj\n\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xb000:0xb009], b'\xff\xff\xff\xff\xff\xff\xff\xff\xff')

    def test_empty_fat16(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2, size=17 * 1024 * 1024)
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x1000:0x1007], b'\xf8\xff\xff\xff\x00\x00\x00')

    def test_simple_fat16(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2, size=17 * 1024 * 1024)
        fatfs.create_directory('TESTFOLD')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x1000:0x1007], b'\xf8\xff\xff\xff\xff\xff\x00')

    def test_chaining_fat16(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2, size=17 * 1024 * 1024)
        fatfs.create_file('WRITEF', extension='TXT')
        fatfs.write_content(path_from_root=['WRITEF.TXT'], content=CFG['sector_size'] * b'a' + b'a')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x1000: 0x100e], b'\xf8\xff\xff\xff\x03\x00\xff\xff\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0xc000: 0xd000], b'a' + (CFG['sector_size'] - 1) * b'\x00')

    def test_chaining_fat16_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1, size=17 * 1024 * 1024)
        fatfs.create_file('WRITEF', extension='TXT')
        fatfs.write_content(path_from_root=['WRITEF.TXT'], content=CFG['sector_size'] * b'a' + b'a')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x1000: 0x100e], b'\xf8\xff\xff\xff\x03\x00\xff\xff\x00\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x9000: 0xa000], b'a' + (CFG['sector_size'] - 1) * b'\x00')

    def test_full_sector_folder_fat16(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2, size=17 * 1024 * 1024)
        fatfs.create_directory('TESTFOLD')

        fill_sector(fatfs)
        fatfs.write_content(path_from_root=['TESTFOLD', 'A0'], content=b'first')
        fatfs.write_content(path_from_root=['TESTFOLD', 'A126'], content=b'later')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x1000: 0x1110],
                         b'\xf8\xff\xff\xff\x82\x00' + 258 * b'\xff' + 8 * b'\x00')
        self.assertEqual(file_system[0x8a000:0x8a005], b'later')
        self.assertEqual(file_system[0x8b000:0x8b010], b'A126        \x00\x00\x00\x00')
        self.assertEqual(file_system[0x8b020:0x8b030], b'A127        \x00\x00\x00\x00')

    def test_full_sector_folder_fat16_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1, size=17 * 1024 * 1024)
        fatfs.create_directory('TESTFOLD')

        fill_sector(fatfs)
        fatfs.write_content(path_from_root=['TESTFOLD', 'A0'], content=b'first')
        fatfs.write_content(path_from_root=['TESTFOLD', 'A126'], content=b'later')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x1000: 0x1110],
                         b'\xf8\xff\xff\xff\x82\x00' + 258 * b'\xff' + 8 * b'\x00')
        self.assertEqual(file_system[0x87000:0x87005], b'later')
        self.assertEqual(file_system[0x88000:0x88010], b'A126        \x00\x00\x00\x00')
        self.assertEqual(file_system[0x88020:0x88030], b'A127        \x00\x00\x00\x00')

    def test_empty_lfn_short_name(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2, long_names_enabled=True)
        fatfs.create_file('HELLO', extension='TXT')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x3000: 0x3019], b'HELLO   TXT \x18\x00\x00\x00!\x00!\x00\x00\x00\x00\x00!')

    def test_empty_lfn_short_name_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1, long_names_enabled=True)
        fatfs.create_file('HELLO', extension='TXT')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x2000: 0x2019], b'HELLO   TXT \x18\x00\x00\x00!\x00!\x00\x00\x00\x00\x00!')

    def test_lfn_short_name(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2, long_names_enabled=True)
        fatfs.create_file('HELLO', extension='TXT')
        fatfs.write_content(path_from_root=['HELLO.TXT'], content=b'this is a test')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x3000: 0x3010], b'HELLO   TXT \x18\x00\x00\x00')
        self.assertEqual(file_system[0x3010: 0x3020], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x0e\x00\x00\x00')
        self.assertEqual(file_system[0x7000: 0x7010], b'this is a test\x00\x00')

    def test_lfn_short_name_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1, long_names_enabled=True)
        fatfs.create_file('HELLO', extension='TXT')
        fatfs.write_content(path_from_root=['HELLO.TXT'], content=b'this is a test')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x2000: 0x2010], b'HELLO   TXT \x18\x00\x00\x00')
        self.assertEqual(file_system[0x2010: 0x2020], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x0e\x00\x00\x00')
        self.assertEqual(file_system[0x6000: 0x6010], b'this is a test\x00\x00')

    def test_lfn_empty_name(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2, long_names_enabled=True)
        fatfs.create_file('HELLOHELLOHELLO', extension='TXT')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x3000: 0x3010], b'Bl\x00o\x00.\x00t\x00x\x00\x0f\x00\xadt\x00')
        self.assertEqual(file_system[0x3012: 0x3020], b'\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\xff\xff\xff\xff')
        self.assertEqual(file_system[0x3020: 0x3030], b'\x01h\x00e\x00l\x00l\x00o\x00\x0f\x00\xadh\x00')
        self.assertEqual(file_system[0x3030: 0x3040], b'e\x00l\x00l\x00o\x00h\x00\x00\x00e\x00l\x00')
        self.assertEqual(file_system[0x3040: 0x3050], b'HELLOH~\x01TXT \x00\x00\x00\x00')
        self.assertEqual(file_system[0x3050: 0x3060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')

    def test_lfn_empty_name_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1, long_names_enabled=True)
        fatfs.create_file('HELLOHELLOHELLO', extension='TXT')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x2000: 0x2010], b'Bl\x00o\x00.\x00t\x00x\x00\x0f\x00\xadt\x00')
        self.assertEqual(file_system[0x2012: 0x2020], b'\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\xff\xff\xff\xff')
        self.assertEqual(file_system[0x2020: 0x2030], b'\x01h\x00e\x00l\x00l\x00o\x00\x0f\x00\xadh\x00')
        self.assertEqual(file_system[0x2030: 0x2040], b'e\x00l\x00l\x00o\x00h\x00\x00\x00e\x00l\x00')
        self.assertEqual(file_system[0x2040: 0x2050], b'HELLOH~\x01TXT \x00\x00\x00\x00')
        self.assertEqual(file_system[0x2050: 0x2060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')

    def test_lfn_plain_name(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2, long_names_enabled=True)
        fatfs.create_file('HELLOHELLOHELLO', extension='TXT')
        fatfs.write_content(path_from_root=['HELLOHELLOHELLO.TXT'], content=b'this is a test')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x3000: 0x3010], b'Bl\x00o\x00.\x00t\x00x\x00\x0f\x00\xadt\x00')
        self.assertEqual(file_system[0x3012: 0x3020], b'\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\xff\xff\xff\xff')
        self.assertEqual(file_system[0x3020: 0x3030], b'\x01h\x00e\x00l\x00l\x00o\x00\x0f\x00\xadh\x00')
        self.assertEqual(file_system[0x3030: 0x3040], b'e\x00l\x00l\x00o\x00h\x00\x00\x00e\x00l\x00')
        self.assertEqual(file_system[0x3040: 0x3050], b'HELLOH~\x01TXT \x00\x00\x00\x00')
        self.assertEqual(file_system[0x3050: 0x3060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x0e\x00\x00\x00')
        self.assertEqual(file_system[0x7000: 0x7010], b'this is a test\x00\x00')

    def test_lfn_plain_name_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1, long_names_enabled=True)
        fatfs.create_file('HELLOHELLOHELLO', extension='TXT')
        fatfs.write_content(path_from_root=['HELLOHELLOHELLO.TXT'], content=b'this is a test')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x2000: 0x2010], b'Bl\x00o\x00.\x00t\x00x\x00\x0f\x00\xadt\x00')
        self.assertEqual(file_system[0x2012: 0x2020], b'\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\xff\xff\xff\xff')
        self.assertEqual(file_system[0x2020: 0x2030], b'\x01h\x00e\x00l\x00l\x00o\x00\x0f\x00\xadh\x00')
        self.assertEqual(file_system[0x2030: 0x2040], b'e\x00l\x00l\x00o\x00h\x00\x00\x00e\x00l\x00')
        self.assertEqual(file_system[0x2040: 0x2050], b'HELLOH~\x01TXT \x00\x00\x00\x00')
        self.assertEqual(file_system[0x2050: 0x2060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x0e\x00\x00\x00')
        self.assertEqual(file_system[0x6000: 0x6010], b'this is a test\x00\x00')

    def test_lfn_plain_name_no_ext(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2, long_names_enabled=True)
        fatfs.create_file('HELLOHELLOHELLO')
        fatfs.write_content(path_from_root=['HELLOHELLOHELLO'], content=b'this is a test')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x3000: 0x3010], b'Bl\x00o\x00\x00\x00\xff\xff\xff\xff\x0f\x00P\xff\xff')
        self.assertEqual(file_system[0x3012: 0x3020], b'\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\xff\xff\xff\xff')
        self.assertEqual(file_system[0x3020: 0x3030], b'\x01h\x00e\x00l\x00l\x00o\x00\x0f\x00Ph\x00')
        self.assertEqual(file_system[0x3030: 0x3040], b'e\x00l\x00l\x00o\x00h\x00\x00\x00e\x00l\x00')
        self.assertEqual(file_system[0x3040: 0x3050], b'HELLOH~\x01    \x00\x00\x00\x00')
        self.assertEqual(file_system[0x3050: 0x3060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x0e\x00\x00\x00')
        self.assertEqual(file_system[0x7000: 0x7010], b'this is a test\x00\x00')

    def test_lfn_plain_name_no_ext_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1, long_names_enabled=True)
        fatfs.create_file('HELLOHELLOHELLO')
        fatfs.write_content(path_from_root=['HELLOHELLOHELLO'], content=b'this is a test')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x2000: 0x2010], b'Bl\x00o\x00\x00\x00\xff\xff\xff\xff\x0f\x00P\xff\xff')
        self.assertEqual(file_system[0x2012: 0x2020], b'\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\xff\xff\xff\xff')
        self.assertEqual(file_system[0x2020: 0x2030], b'\x01h\x00e\x00l\x00l\x00o\x00\x0f\x00Ph\x00')
        self.assertEqual(file_system[0x2030: 0x2040], b'e\x00l\x00l\x00o\x00h\x00\x00\x00e\x00l\x00')
        self.assertEqual(file_system[0x2040: 0x2050], b'HELLOH~\x01    \x00\x00\x00\x00')
        self.assertEqual(file_system[0x2050: 0x2060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x0e\x00\x00\x00')
        self.assertEqual(file_system[0x6000: 0x6010], b'this is a test\x00\x00')

    def test_lfn_short_entry_exception(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2, long_names_enabled=True)
        self.assertRaises(LowerCaseException, fatfs.create_file, 'hello', extension='txt')

    def test_lfn_nested_empty(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2, long_names_enabled=True)
        fatfs.create_directory('VERYLONGTESTFOLD')
        fatfs.create_file('HELLO', extension='TXT', path_from_root=['VERYLONGTESTFOLD'])
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x3000: 0x3010], b'Bo\x00l\x00d\x00\x00\x00\xff\xff\x0f\x00\xa0\xff\xff')
        self.assertEqual(file_system[0x3012: 0x3020], b'\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\xff\xff\xff\xff')
        self.assertEqual(file_system[0x3020: 0x3030], b'\x01v\x00e\x00r\x00y\x00l\x00\x0f\x00\xa0o\x00')
        self.assertEqual(file_system[0x3030: 0x3040], b'n\x00g\x00t\x00e\x00s\x00\x00\x00t\x00f\x00')
        self.assertEqual(file_system[0x3040: 0x3050], b'VERYLO~\x01   \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x3050: 0x3060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')

        self.assertEqual(file_system[0x7000: 0x7010], b'.          \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7012: 0x7020], b'!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7020: 0x7030], b'..         \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7030: 0x7040], b'!\x00!\x00\x00\x00\x00\x00!\x00\x01\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7040: 0x7050], b'HELLO   TXT \x18\x00\x00\x00')
        self.assertEqual(file_system[0x7050: 0x7060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x03\x00\x00\x00\x00\x00')

    def test_lfn_nested_empty_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1, long_names_enabled=True)
        fatfs.create_directory('VERYLONGTESTFOLD')
        fatfs.create_file('HELLO', extension='TXT', path_from_root=['VERYLONGTESTFOLD'])
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x2000: 0x2010], b'Bo\x00l\x00d\x00\x00\x00\xff\xff\x0f\x00\xa0\xff\xff')
        self.assertEqual(file_system[0x2012: 0x2020], b'\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\xff\xff\xff\xff')
        self.assertEqual(file_system[0x2020: 0x2030], b'\x01v\x00e\x00r\x00y\x00l\x00\x0f\x00\xa0o\x00')
        self.assertEqual(file_system[0x2030: 0x2040], b'n\x00g\x00t\x00e\x00s\x00\x00\x00t\x00f\x00')
        self.assertEqual(file_system[0x2040: 0x2050], b'VERYLO~\x01   \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x2050: 0x2060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')

        self.assertEqual(file_system[0x6000: 0x6010], b'.          \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6012: 0x6020], b'!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6020: 0x6030], b'..         \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6030: 0x6040], b'!\x00!\x00\x00\x00\x00\x00!\x00\x01\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6040: 0x6050], b'HELLO   TXT \x18\x00\x00\x00')
        self.assertEqual(file_system[0x6050: 0x6060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x03\x00\x00\x00\x00\x00')

    def test_lfn_nested_long_empty(self) -> None:
        fatfs: fatfsgen.fatfs = fatfsgen.FATFS(fat_tables_cnt=2, long_names_enabled=True)
        fatfs.create_directory('verylongtestfold')
        fatfs.create_file('hellohellohello', extension='txt', path_from_root=['verylongtestfold'])
        fatfs.write_filesystem(CFG['output_file'])

        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x3000: 0x3010], b'Bo\x00l\x00d\x00\x00\x00\xff\xff\x0f\x00\n\xff\xff')
        self.assertEqual(file_system[0x3012: 0x3020], b'\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\xff\xff\xff\xff')
        self.assertEqual(file_system[0x3020: 0x3030], b'\x01v\x00e\x00r\x00y\x00l\x00\x0f\x00\no\x00')
        self.assertEqual(file_system[0x3030: 0x3040], b'n\x00g\x00t\x00e\x00s\x00\x00\x00t\x00f\x00')
        self.assertEqual(file_system[0x3040: 0x3050], b'verylo~\x01   \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x3050: 0x3060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')

        self.assertEqual(file_system[0x7000: 0x7010], b'.          \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7012: 0x7020], b'!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7020: 0x7030], b'..         \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7030: 0x7040], b'!\x00!\x00\x00\x00\x00\x00!\x00\x01\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7040: 0x7050], b'Bl\x00o\x00.\x00t\x00x\x00\x0f\x00\xcft\x00')
        self.assertEqual(file_system[0x7050: 0x7060],
                         b'\x00\x00\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\xff\xff\xff\xff')

    def test_lfn_nested_long_empty_one_fat(self) -> None:
        fatfs: fatfsgen.FATFS = fatfsgen.FATFS(fat_tables_cnt=1, long_names_enabled=True)
        fatfs.create_directory('verylongtestfold')
        fatfs.create_file('hellohellohello', extension='txt', path_from_root=['verylongtestfold'])
        fatfs.write_filesystem(CFG['output_file'])

        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x2000: 0x2010], b'Bo\x00l\x00d\x00\x00\x00\xff\xff\x0f\x00\n\xff\xff')
        self.assertEqual(file_system[0x2012: 0x2020], b'\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\xff\xff\xff\xff')
        self.assertEqual(file_system[0x2020: 0x2030], b'\x01v\x00e\x00r\x00y\x00l\x00\x0f\x00\no\x00')
        self.assertEqual(file_system[0x2030: 0x2040], b'n\x00g\x00t\x00e\x00s\x00\x00\x00t\x00f\x00')
        self.assertEqual(file_system[0x2040: 0x2050], b'verylo~\x01   \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x2050: 0x2060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')

        self.assertEqual(file_system[0x6000: 0x6010], b'.          \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6012: 0x6020], b'!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6020: 0x6030], b'..         \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6030: 0x6040], b'!\x00!\x00\x00\x00\x00\x00!\x00\x01\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6040: 0x6050], b'Bl\x00o\x00.\x00t\x00x\x00\x0f\x00\xcft\x00')
        self.assertEqual(file_system[0x6050: 0x6060],
                         b'\x00\x00\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\xff\xff\xff\xff')

    def test_lfn_nested_text(self) -> None:
        fatfs: fatfsgen.fatfs = fatfsgen.FATFS(fat_tables_cnt=2, long_names_enabled=True)
        fatfs.create_directory('VERYLONGTESTFOLD')
        fatfs.create_file('HELLO', extension='TXT', path_from_root=['VERYLONGTESTFOLD'])
        fatfs.write_content(path_from_root=['VERYLONGTESTFOLD', 'HELLO.TXT'], content=b'this is a test')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x3000: 0x3010], b'Bo\x00l\x00d\x00\x00\x00\xff\xff\x0f\x00\xa0\xff\xff')
        self.assertEqual(file_system[0x3012: 0x3020], b'\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\xff\xff\xff\xff')
        self.assertEqual(file_system[0x3020: 0x3030], b'\x01v\x00e\x00r\x00y\x00l\x00\x0f\x00\xa0o\x00')
        self.assertEqual(file_system[0x3030: 0x3040], b'n\x00g\x00t\x00e\x00s\x00\x00\x00t\x00f\x00')
        self.assertEqual(file_system[0x3040: 0x3050], b'VERYLO~\x01   \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x3050: 0x3060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')

        self.assertEqual(file_system[0x7000: 0x7010], b'.          \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7012: 0x7020], b'!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7020: 0x7030], b'..         \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7030: 0x7040], b'!\x00!\x00\x00\x00\x00\x00!\x00\x01\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7040: 0x7050], b'HELLO   TXT \x18\x00\x00\x00')
        self.assertEqual(file_system[0x7050: 0x7060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x03\x00\x0e\x00\x00\x00')

        self.assertEqual(file_system[0x8000: 0x8010], b'this is a test\x00\x00')

    def test_lfn_nested_text_one_fat(self) -> None:
        fatfs: fatfsgen.FATFS = fatfsgen.FATFS(fat_tables_cnt=1, long_names_enabled=True)
        fatfs.create_directory('VERYLONGTESTFOLD')
        fatfs.create_file('HELLO', extension='TXT', path_from_root=['VERYLONGTESTFOLD'])
        fatfs.write_content(path_from_root=['VERYLONGTESTFOLD', 'HELLO.TXT'], content=b'this is a test')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])
        self.assertEqual(file_system[0x2000: 0x2010], b'Bo\x00l\x00d\x00\x00\x00\xff\xff\x0f\x00\xa0\xff\xff')
        self.assertEqual(file_system[0x2012: 0x2020], b'\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\xff\xff\xff\xff')
        self.assertEqual(file_system[0x2020: 0x2030], b'\x01v\x00e\x00r\x00y\x00l\x00\x0f\x00\xa0o\x00')
        self.assertEqual(file_system[0x2030: 0x2040], b'n\x00g\x00t\x00e\x00s\x00\x00\x00t\x00f\x00')
        self.assertEqual(file_system[0x2040: 0x2050], b'VERYLO~\x01   \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x2050: 0x2060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')

        self.assertEqual(file_system[0x6000: 0x6010], b'.          \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6012: 0x6020], b'!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6020: 0x6030], b'..         \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6030: 0x6040], b'!\x00!\x00\x00\x00\x00\x00!\x00\x01\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6040: 0x6050], b'HELLO   TXT \x18\x00\x00\x00')
        self.assertEqual(file_system[0x6050: 0x6060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x03\x00\x0e\x00\x00\x00')

        self.assertEqual(file_system[0x7000: 0x7010], b'this is a test\x00\x00')

    def test_boundary_clusters12(self) -> None:
        output: bytes = check_output(['python', '../fatfsgen.py', '--partition_size', '16732160', 'test_dir'],
                                     stderr=STDOUT)
        self.assertEqual(
            output,
            b'WARNING: It is not recommended to create FATFS with bounding count of clusters: 4085 or 65525\n')

    def test_boundary_clusters16(self) -> None:
        output: bytes = check_output(['python', '../fatfsgen.py', '--partition_size', '268390400', 'test_dir'],
                                     stderr=STDOUT)
        self.assertEqual(
            output,
            b'WARNING: It is not recommended to create FATFS with bounding count of clusters: 4085 or 65525\n')

    def test_boundary_clusters_fat32(self) -> None:
        self.assertRaises(NotImplementedError, fatfsgen.FATFS, size=268419193)

    def test_inconsistent_fat12(self) -> None:
        self.assertRaises(InconsistentFATAttributes, fatfsgen.FATFS, size=20480000, explicit_fat_type=FAT12)

    def test_lfn_increasing(self) -> None:
        fatfs: fatfsgen.fatfs = fatfsgen.FATFS(fat_tables_cnt=2, long_names_enabled=True)
        fatfs.create_directory('VERYLONGTESTFOLD')
        fatfs.create_file('HELLOHELLOHELLOOOOOOO', extension='TXT', path_from_root=['VERYLONGTESTFOLD'])
        fatfs.create_file('HELLOHELLOHELLOOOOOOB', extension='TXT', path_from_root=['VERYLONGTESTFOLD'])
        fatfs.write_content(path_from_root=['VERYLONGTESTFOLD', 'HELLOHELLOHELLOOOOOOO.TXT'],
                            content=b'this is a test A')
        fatfs.write_content(path_from_root=['VERYLONGTESTFOLD', 'HELLOHELLOHELLOOOOOOB.TXT'],
                            content=b'this is a test B')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x3000: 0x3010], b'Bo\x00l\x00d\x00\x00\x00\xff\xff\x0f\x00\xa0\xff\xff')
        self.assertEqual(file_system[0x3011: 0x3020], b'\xff\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\xff\xff\xff\xff')
        self.assertEqual(file_system[0x3020: 0x3030], b'\x01v\x00e\x00r\x00y\x00l\x00\x0f\x00\xa0o\x00')
        self.assertEqual(file_system[0x3030: 0x3040], b'n\x00g\x00t\x00e\x00s\x00\x00\x00t\x00f\x00')
        self.assertEqual(file_system[0x3040: 0x3050], b'VERYLO~\x01   \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x3050: 0x3060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')

        self.assertEqual(file_system[0x7000: 0x7010], b'.          \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7011: 0x7020], b'\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7020: 0x7030], b'..         \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7030: 0x7040], b'!\x00!\x00\x00\x00\x00\x00!\x00\x01\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x7040: 0x7050], b'Bl\x00o\x00o\x00o\x00o\x00\x0f\x00\xado\x00')
        self.assertEqual(file_system[0x7050: 0x7060], b'o\x00o\x00.\x00t\x00x\x00\x00\x00t\x00\x00\x00')

        self.assertEqual(file_system[0x7050: 0x7060], b'o\x00o\x00.\x00t\x00x\x00\x00\x00t\x00\x00\x00')
        self.assertEqual(file_system[0x7060: 0x7070], b'\x01h\x00e\x00l\x00l\x00o\x00\x0f\x00\xadh\x00')
        self.assertEqual(file_system[0x7070: 0x7080], b'e\x00l\x00l\x00o\x00h\x00\x00\x00e\x00l\x00')
        self.assertEqual(file_system[0x7080: 0x7090], b'HELLOH~\x01TXT \x00\x00\x00\x00')
        self.assertEqual(file_system[0x7090: 0x70a0], b'!\x00!\x00\x00\x00\x00\x00!\x00\x03\x00\x10\x00\x00\x00')
        self.assertEqual(file_system[0x70a0: 0x70b0], b'Bl\x00o\x00o\x00o\x00o\x00\x0f\x00\x8do\x00')

        self.assertEqual(file_system[0x70b0: 0x70c0], b'o\x00b\x00.\x00t\x00x\x00\x00\x00t\x00\x00\x00')
        self.assertEqual(file_system[0x70c0: 0x70d0], b'\x01h\x00e\x00l\x00l\x00o\x00\x0f\x00\x8dh\x00')
        self.assertEqual(file_system[0x70d0: 0x70e0], b'e\x00l\x00l\x00o\x00h\x00\x00\x00e\x00l\x00')
        self.assertEqual(file_system[0x70e0: 0x70f0], b'HELLOH~\x02TXT \x00\x00\x00\x00')

    def test_lfn_increasing_one_fat(self) -> None:
        fatfs: fatfsgen.FATFS = fatfsgen.FATFS(fat_tables_cnt=1, long_names_enabled=True)
        fatfs.create_directory('VERYLONGTESTFOLD')
        fatfs.create_file('HELLOHELLOHELLOOOOOOO', extension='TXT', path_from_root=['VERYLONGTESTFOLD'])
        fatfs.create_file('HELLOHELLOHELLOOOOOOB', extension='TXT', path_from_root=['VERYLONGTESTFOLD'])
        fatfs.write_content(path_from_root=['VERYLONGTESTFOLD', 'HELLOHELLOHELLOOOOOOO.TXT'],
                            content=b'this is a test A')
        fatfs.write_content(path_from_root=['VERYLONGTESTFOLD', 'HELLOHELLOHELLOOOOOOB.TXT'],
                            content=b'this is a test B')
        fatfs.write_filesystem(CFG['output_file'])
        file_system = read_filesystem(CFG['output_file'])

        self.assertEqual(file_system[0x2000: 0x2010], b'Bo\x00l\x00d\x00\x00\x00\xff\xff\x0f\x00\xa0\xff\xff')
        self.assertEqual(file_system[0x2011: 0x2020], b'\xff\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\xff\xff\xff\xff')
        self.assertEqual(file_system[0x2020: 0x2030], b'\x01v\x00e\x00r\x00y\x00l\x00\x0f\x00\xa0o\x00')
        self.assertEqual(file_system[0x2030: 0x2040], b'n\x00g\x00t\x00e\x00s\x00\x00\x00t\x00f\x00')
        self.assertEqual(file_system[0x2040: 0x2050], b'VERYLO~\x01   \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x2050: 0x2060], b'!\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')

        self.assertEqual(file_system[0x6000: 0x6010], b'.          \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6011: 0x6020], b'\x00!\x00\x00\x00\x00\x00!\x00\x02\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6020: 0x6030], b'..         \x10\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6030: 0x6040], b'!\x00!\x00\x00\x00\x00\x00!\x00\x01\x00\x00\x00\x00\x00')
        self.assertEqual(file_system[0x6040: 0x6050], b'Bl\x00o\x00o\x00o\x00o\x00\x0f\x00\xado\x00')
        self.assertEqual(file_system[0x6050: 0x6060], b'o\x00o\x00.\x00t\x00x\x00\x00\x00t\x00\x00\x00')

        self.assertEqual(file_system[0x6050: 0x6060], b'o\x00o\x00.\x00t\x00x\x00\x00\x00t\x00\x00\x00')
        self.assertEqual(file_system[0x6060: 0x6070], b'\x01h\x00e\x00l\x00l\x00o\x00\x0f\x00\xadh\x00')
        self.assertEqual(file_system[0x6070: 0x6080], b'e\x00l\x00l\x00o\x00h\x00\x00\x00e\x00l\x00')
        self.assertEqual(file_system[0x6080: 0x6090], b'HELLOH~\x01TXT \x00\x00\x00\x00')
        self.assertEqual(file_system[0x6090: 0x60a0], b'!\x00!\x00\x00\x00\x00\x00!\x00\x03\x00\x10\x00\x00\x00')
        self.assertEqual(file_system[0x60a0: 0x60b0], b'Bl\x00o\x00o\x00o\x00o\x00\x0f\x00\x8do\x00')

        self.assertEqual(file_system[0x60b0: 0x60c0], b'o\x00b\x00.\x00t\x00x\x00\x00\x00t\x00\x00\x00')
        self.assertEqual(file_system[0x60c0: 0x60d0], b'\x01h\x00e\x00l\x00l\x00o\x00\x0f\x00\x8dh\x00')
        self.assertEqual(file_system[0x60d0: 0x60e0], b'e\x00l\x00l\x00o\x00h\x00\x00\x00e\x00l\x00')
        self.assertEqual(file_system[0x60e0: 0x60f0], b'HELLOH~\x02TXT \x00\x00\x00\x00')

    def test_bs_not_initialized(self) -> None:
        self.assertEqual(str(BootSector()), 'Boot sector is not initialized!')
        self.assertRaises(NotInitialized, BootSector().generate_boot_sector)
        self.assertRaises(NotInitialized, lambda: BootSector().binary_image)  # encapsulate property to callable

    def test_bs_str(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        bs = BootSector(fatfs.state.boot_sector_state)
        bs.generate_boot_sector()
        bs.parse_boot_sector(bs.binary_image)
        x = 'FATFS properties:,clusters: 251,data_region_start: 28672,data_sectors: ' \
            '249,entries_root_count: 512,fat_table_start_address: 4096,fat_tables_cnt: 2,' \
            'fatfs_type: 12,file_sys_type: FAT     ,hidden_sectors: 0,media_type: 248,' \
            'non_data_sectors: 7,num_heads: 255,oem_name: MSDOS5.0,reserved_sectors_cnt: 1,' \
            'root_dir_sectors_cnt: 4,root_directory_start: 12288,sec_per_track: 63,sector_size: 4096,' \
            'sectors_count: 256,sectors_per_cluster: 1,sectors_per_fat_cnt: 1,size: 1048576,' \
            'volume_label: Espressif  ,volume_uuid: 1144419653,'
        self.assertEqual(x.split(',')[:-2], str(bs).split('\n')[:-2])  # except for volume id

    def test_bs_str_one_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=1)
        bs = BootSector(fatfs.state.boot_sector_state)
        bs.generate_boot_sector()
        bs.parse_boot_sector(bs.binary_image)
        x = 'FATFS properties:,clusters: 252,data_region_start: 24576,data_sectors: ' \
            '250,entries_root_count: 512,fat_table_start_address: 4096,fat_tables_cnt: 1,' \
            'fatfs_type: 12,file_sys_type: FAT     ,hidden_sectors: 0,media_type: 248,' \
            'non_data_sectors: 6,num_heads: 255,oem_name: MSDOS5.0,reserved_sectors_cnt: 1,' \
            'root_dir_sectors_cnt: 4,root_directory_start: 8192,sec_per_track: 63,sector_size: 4096,' \
            'sectors_count: 256,sectors_per_cluster: 1,sectors_per_fat_cnt: 1,size: 1048576,' \
            'volume_label: Espressif  ,volume_uuid: 1144419653,'
        self.assertEqual(x.split(',')[:-2], str(bs).split('\n')[:-2])  # except for volume id

    def test_parsing_error(self) -> None:
        self.assertRaises(NotInitialized, BootSector().parse_boot_sector, b'')

    def test_not_implemented_fat32(self) -> None:
        self.assertEqual(
            Entry.get_cluster_id(
                Entry.ENTRY_FORMAT_SHORT_NAME.parse(
                    bytearray(b'AHOJ        \x18\x00\xb0[&U&U\x00\x00\xb0[&U\x02\x00\x08\x00\x00\x00'))),
            2)

    def test_get_cluster_value_from_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        self.assertEqual(fatfs.fat.get_cluster_value(1), 0xFFF)

    def test_is_cluster_last(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        self.assertEqual(fatfs.fat.is_cluster_last(2), False)

    def test_chain_in_fat(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        self.assertEqual(fatfs.fat.get_chained_content(1), b'\x00' * 0x1000)

    def test_retrieve_file_chaining(self) -> None:
        fatfs = fatfsgen.FATFS(fat_tables_cnt=2)
        fatfs.create_file('WRITEF', extension='TXT')
        fatfs.write_content(path_from_root=['WRITEF.TXT'], content=CFG['sector_size'] * b'a' + b'a')
        fatfs.write_filesystem(CFG['output_file'])
        self.assertEqual(fatfs.fat.get_chained_content(1)[:15], b'WRITEF  TXT \x00\x00\x00')
        self.assertEqual(fatfs.fat.get_chained_content(2)[:15], b'aaaaaaaaaaaaaaa')

    def test_lstrip(self) -> None:
        self.assertEqual(right_strip_string('\x20\x20\x20thisistest\x20\x20\x20'), '   thisistest')


if __name__ == '__main__':
    unittest.main()
