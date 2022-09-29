#!/usr/bin/env python
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import shutil
import sys
import unittest
from subprocess import STDOUT, run

from test_utils import compare_folders, fill_sector, generate_local_folder_structure, generate_test_dir_2

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
import fatfsgen  # noqa E402  # pylint: disable=C0413
from fatfs_utils.entry import Entry  # noqa E402  # pylint: disable=C0413


class FatFSGen(unittest.TestCase):
    def setUp(self) -> None:
        os.makedirs('output_data')
        generate_test_dir_2()

    def tearDown(self) -> None:
        shutil.rmtree('output_data', ignore_errors=True)
        shutil.rmtree('Espressif', ignore_errors=True)
        shutil.rmtree('testf', ignore_errors=True)
        shutil.rmtree('testf_wl', ignore_errors=True)

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

    @staticmethod
    def test_file_chaining() -> None:
        fatfs = fatfsgen.FATFS()
        fatfs.create_file('WRITEF', extension='TXT')
        fatfs.write_content(path_from_root=['WRITEF.TXT'], content=4096 * b'a' + b'a')
        fatfs.write_filesystem('fatfs_image.img')

        run(['python', '../fatfsparse.py', 'fatfs_image.img'], stderr=STDOUT)
        with open('Espressif/WRITEF.TXT', 'rb') as in_:
            assert in_.read() == 4097 * b'a'

    @staticmethod
    def test_full_two_sectors_folder() -> None:
        fatfs = fatfsgen.FATFS(size=2 * 1024 * 1024)
        fatfs.create_directory('TESTFOLD')

        for i in range((2 * 4096) // 32):
            fatfs.create_file(f'A{str(i).upper()}', path_from_root=['TESTFOLD'])
        fatfs.write_content(path_from_root=['TESTFOLD', 'A253'], content=b'later')
        fatfs.write_content(path_from_root=['TESTFOLD', 'A255'], content=b'last')
        fatfs.write_filesystem('fatfs_image.img')

        run(['python', '../fatfsparse.py', 'fatfs_image.img'], stderr=STDOUT)
        assert set(os.listdir('Espressif')) == {'TESTFOLD'}
        assert set(os.listdir('Espressif/TESTFOLD')) == {f'A{str(i).upper()}' for i in range(256)}

        with open('Espressif/TESTFOLD/A253', 'rb') as in_:
            assert in_.read() == b'later'

        with open('Espressif/TESTFOLD/A255', 'rb') as in_:
            assert in_.read() == b'last'

    @staticmethod
    def test_empty_fat16() -> None:
        fatfs = fatfsgen.FATFS(size=17 * 1024 * 1024)
        fatfs.write_filesystem('fatfs_image.img')
        run(['python', '../fatfsparse.py', 'fatfs_image.img'], stderr=STDOUT)

    @staticmethod
    def test_chaining_fat16() -> None:
        fatfs = fatfsgen.FATFS(size=17 * 1024 * 1024)
        fatfs.create_file('WRITEF', extension='TXT')
        fatfs.write_content(path_from_root=['WRITEF.TXT'], content=4096 * b'a' + b'a')
        fatfs.write_filesystem('fatfs_image.img')
        run(['python', '../fatfsparse.py', 'fatfs_image.img'], stderr=STDOUT)
        with open('Espressif/WRITEF.TXT', 'rb') as in_:
            assert in_.read() == 4097 * b'a'

    @staticmethod
    def test_full_sector_folder_fat16() -> None:
        fatfs = fatfsgen.FATFS(size=17 * 1024 * 1024)
        fatfs.create_directory('TESTFOLD')

        fill_sector(fatfs)
        fatfs.write_content(path_from_root=['TESTFOLD', 'A0'], content=b'first')
        fatfs.write_content(path_from_root=['TESTFOLD', 'A126'], content=b'later')
        fatfs.write_filesystem('fatfs_image.img')
        run(['python', '../fatfsparse.py', 'fatfs_image.img'], stderr=STDOUT)
        assert set(os.listdir('Espressif')) == {'TESTFOLD'}
        assert set(os.listdir('Espressif/TESTFOLD')) == {f'A{str(i).upper()}' for i in range(128)}
        with open('Espressif/TESTFOLD/A0', 'rb') as in_:
            assert in_.read() == b'first'

        with open('Espressif/TESTFOLD/A126', 'rb') as in_:
            assert in_.read() == b'later'

    @staticmethod
    def file_(x: str, content_: str = 'hey this is a test') -> dict:
        return {
            'type': 'file',
            'name': x,
            'content': content_
        }

    def test_e2e_file(self) -> None:
        struct_: dict = {
            'type': 'folder',
            'name': 'testf',
            'content': [self.file_('NEWF')]
        }
        generate_local_folder_structure(struct_, path_='.')
        run([
            'python',
            f'{os.path.join(os.path.dirname(__file__), "..", "fatfsgen.py")}',
            'testf'
        ], stderr=STDOUT)
        run(['python', '../fatfsparse.py', 'fatfs_image.img'], stderr=STDOUT)
        assert compare_folders('testf', 'Espressif')
        shutil.rmtree('Espressif', ignore_errors=True)

        run([
            'python',
            f'{os.path.join(os.path.dirname(__file__), "..", "wl_fatfsgen.py")}',
            'testf'
        ], stderr=STDOUT)
        run(['python', '../fatfsparse.py', 'fatfs_image.img'], stderr=STDOUT)
        assert compare_folders('testf', 'Espressif')

    def test_e2e_deeper(self) -> None:
        folder_ = {
            'type': 'folder',
            'name': 'XYZ',
            'content': [
                self.file_('NEWFLE'),
                self.file_('NEW.TXT'),
                self.file_('NEWE.TXT'),
                self.file_('NEW4.TXT'),
                self.file_('NEW5.TXT'),
            ]
        }
        struct_: dict = {
            'type': 'folder',
            'name': 'testf',
            'content': [
                self.file_('MY_NEW'),
                folder_
            ]
        }

        generate_local_folder_structure(struct_, path_='.')
        run([
            'python',
            f'{os.path.join(os.path.dirname(__file__), "..", "fatfsgen.py")}',
            'testf'
        ], stderr=STDOUT)
        run(['python', '../fatfsparse.py', 'fatfs_image.img'], stderr=STDOUT)
        assert compare_folders('testf', 'Espressif')
        shutil.rmtree('Espressif', ignore_errors=True)

        run([
            'python',
            f'{os.path.join(os.path.dirname(__file__), "..", "wl_fatfsgen.py")}',
            'testf'
        ], stderr=STDOUT)
        run(['python', '../fatfsparse.py', 'fatfs_image.img'], stderr=STDOUT)
        assert compare_folders('testf', 'Espressif')

    def test_e2e_deeper_large(self) -> None:
        folder_ = {
            'type': 'folder',
            'name': 'XYZ',
            'content': [
                self.file_('NEWFLE', content_=4097 * 'a'),
                self.file_('NEW.TXT', content_=2 * 4097 * 'a'),
                self.file_('NEWE.TXT'),
                self.file_('NEW4.TXT'),
                self.file_('NEW5.TXT'),
            ]
        }
        folder2_ = {
            'type': 'folder',
            'name': 'XYZ3',
            'content': [
                self.file_('NEWFLE', content_=4097 * 'a'),
                self.file_('NEW.TXT', content_=2 * 4097 * 'a'),
                self.file_('NEWE.TXT'),
                self.file_('NEW4.TXT'),
                self.file_('NEW5.TXT'),
            ]
        }
        folder3_ = {
            'type': 'folder',
            'name': 'XYZ2',
            'content': [self.file_(f'A{i}') for i in range(50)]
        }
        struct_: dict = {
            'type': 'folder',
            'name': 'testf',
            'content': [
                self.file_('MY_NEW'),
                folder_,
                folder2_,
                folder3_
            ]
        }
        generate_local_folder_structure(struct_, path_='.')
        run([
            'python',
            f'{os.path.join(os.path.dirname(__file__), "..", "fatfsgen.py")}',
            'testf'
        ], stderr=STDOUT)
        run(['python', '../fatfsparse.py', 'fatfs_image.img'], stderr=STDOUT)
        assert compare_folders('testf', 'Espressif')
        shutil.rmtree('Espressif', ignore_errors=True)

        run([
            'python',
            f'{os.path.join(os.path.dirname(__file__), "..", "wl_fatfsgen.py")}',
            'testf'
        ], stderr=STDOUT)
        run(['python', '../fatfsparse.py', 'fatfs_image.img'], stderr=STDOUT)
        assert compare_folders('testf', 'Espressif')

    def test_e2e_very_deep(self) -> None:
        folder_ = {
            'type': 'folder',
            'name': 'XYZ',
            'content': [
                self.file_('NEWFLE', content_=4097 * 'a'),
                self.file_('NEW.TXT', content_=2 * 4097 * 'a'),
                self.file_('NEWE.TXT'),
                self.file_('NEW4.TXT'),
                self.file_('NEW5.TXT'),
            ]
        }
        folder2_ = {
            'type': 'folder',
            'name': 'XYZ3',
            'content': [
                self.file_('NEWFLE', content_=4097 * 'a'),
                self.file_('NEW.TXT', content_=2 * 4097 * 'a'),
                self.file_('NEWE.TXT'),
                self.file_('NEW4.TXT'),
                self.file_('NEW5.TXT'),
                folder_,
            ]
        }
        folder3_ = {
            'type': 'folder',
            'name': 'XYZ2',
            'content': [self.file_(f'A{i}') for i in range(50)] + [folder2_]
        }

        struct_: dict = {
            'type': 'folder',
            'name': 'testf',
            'content': [
                self.file_('MY_NEW'),
                folder_,
                folder2_,
                folder3_
            ]
        }
        generate_local_folder_structure(struct_, path_='.')
        run([
            'python',
            f'{os.path.join(os.path.dirname(__file__), "..", "fatfsgen.py")}',
            'testf'
        ], stderr=STDOUT)
        run(['python', '../fatfsparse.py', 'fatfs_image.img'], stderr=STDOUT)
        assert compare_folders('testf', 'Espressif')

    def test_e2e_very_deep_long(self) -> None:
        folder_ = {
            'type': 'folder',
            'name': 'veryveryverylong111',
            'content': [
                self.file_('myndewveryverylongfile1.txt', content_=4097 * 'a'),
                self.file_('mynewveryverylongfile22.txt', content_=2 * 4097 * 'a'),
                self.file_('mynewveryverylongfile333.txt' * 8),
                self.file_('mynewveryverylongfile4444.txt' * 8),
                self.file_('mynewveryverylongfile5555.txt'),
                self.file_('SHORT.TXT'),
            ]
        }
        struct_: dict = {
            'type': 'folder',
            'name': 'testf',
            'content': [
                self.file_('mynewveryverylongfile.txt' * 5),
                folder_,
            ]
        }
        generate_local_folder_structure(struct_, path_='.')
        run([
            'python',
            f'{os.path.join(os.path.dirname(__file__), "..", "fatfsgen.py")}',
            'testf', '--long_name_support'
        ], stderr=STDOUT)
        run(['python', '../fatfsparse.py', 'fatfs_image.img'], stderr=STDOUT)
        assert compare_folders('testf', 'Espressif')

    def test_parse_long_name(self) -> None:
        self.assertEqual(
            Entry.parse_entry_long(
                b'\x01t\x00h\x00i\x00s\x00_\x00\x0f\x00\xfbi\x00s\x00_\x00l\x00o\x00n\x00\x00\x00g\x00_\x00', 251),
            {
                'order': 1,
                'name1': b't\x00h\x00i\x00s\x00_\x00',
                'name2': b'i\x00s\x00_\x00l\x00o\x00n\x00',
                'name3': b'g\x00_\x00',
                'is_last': False
            }
        )
        self.assertEqual(
            Entry.parse_entry_long(
                b'\x01t\x00h\x00i\x00s\x00_\x00\x0f\x00\xfbi\x00s\x00_\x00l\x00o\x00n\x00\x00\x00g\x00_\x00', 252
            ),
            {}
        )
        self.assertEqual(
            Entry.parse_entry_long(
                b'\x01t\x00h\x00i\x00s\x00_\x00\x0f\x01\xfbi\x00s\x00_\x00l\x00o\x00n\x00\x00\x00g\x00_\x00', 251
            ),
            {}
        )


if __name__ == '__main__':
    unittest.main()
