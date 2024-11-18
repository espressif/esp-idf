# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os
import re
import shutil
import sys
from datetime import datetime
from subprocess import run
from subprocess import STDOUT
from typing import List

import pytest
from pytest_embedded import Dut

idf_path = os.environ['IDF_PATH']  # get value of IDF_PATH from environment
parttool_dir = os.path.join(idf_path, 'components', 'partition_table')


sys.path.append(parttool_dir)
from parttool import PartitionName, ParttoolTarget  # noqa E402  # pylint: disable=C0413


def file_(x: str, content_: str = 'hey this is a test') -> dict:
    return {
        'type': 'file',
        'name': x,
        'content': content_
    }


def generate_local_folder_structure(structure_: dict, path_: str) -> None:
    if structure_['type'] == 'folder':
        new_path_ = os.path.join(path_, structure_['name'])
        os.makedirs(new_path_)
        for item_ in structure_['content']:
            generate_local_folder_structure(item_, new_path_)
    else:
        new_path_ = os.path.join(path_, structure_['name'])
        with open(new_path_, 'w') as f_:
            f_.write(structure_['content'])


def compare_folders(fp1: str, fp2: str) -> bool:
    if os.path.isdir(fp1) != os.path.isdir(fp2):
        return False
    if os.path.isdir(fp1):
        if set(os.listdir(fp1)) != set(os.listdir(fp2)):
            return False
        return all([compare_folders(os.path.join(fp1, path_), os.path.join(fp2, path_)) for path_ in os.listdir(fp1)])
    with open(fp1, 'rb') as f1_, open(fp2, 'rb') as f2_:
        return f1_.read() == f2_.read()


@pytest.mark.esp32
@pytest.mark.generic
@pytest.mark.parametrize('config', ['test_read_only_partition_gen',
                                    'test_read_only_partition_gen_default_dt',
                                    'test_read_only_partition_gen_ln',
                                    'test_read_only_partition_gen_ln_default_dt',
                                    'test_read_write_partition_gen',
                                    'test_read_write_partition_gen_default_dt',
                                    'test_read_write_partition_gen_ln',
                                    'test_read_write_partition_gen_ln_default_dt',
                                    ], indirect=True)
def test_examples_fatfsgen(config: str, dut: Dut) -> None:
    # Expects list of strings sequentially
    def expect_all(msg_list: List[str], to: int) -> None:
        for msg in msg_list:
            dut.expect(msg, timeout=to)

    # Expects prefix string followed by date in the format 'yyyy-mm-dd'
    def expect_date(prefix: str, to: int) -> datetime:
        expect_str = prefix + '(\\d+)-(\\d+)-(\\d+)'
        match_ = dut.expect(re.compile(str.encode(expect_str)), timeout=to)
        year_ = int(match_[1].decode())
        month_ = int(match_[2].decode())
        day_ = int(match_[3].decode())
        return datetime(year_, month_, day_)

    # Calculates absolute difference in days between date_reference and date_actual.
    # Raises exception if difference exceeds tolerance
    def evaluate_dates(date_reference: datetime, date_actual: datetime, days_tolerance: int) -> None:
        td = date_actual - date_reference
        if abs(td.days) > days_tolerance:
            raise Exception(f'Too big date difference. Actual: {date_actual}, reference: {date_reference}, tolerance: {days_tolerance} day(s)')

    # Expect timeout
    timeout = 20

    # We tolerate 30 days difference between actual file creation and date when test was executed.
    tolerance = 30
    filename_ln = 'sublongnames/testlongfilenames.txt'
    filename_sn = 'sub/test.txt'
    date_modified = datetime.today()
    date_default = datetime(1980, 1, 1)
    fatfs_parser_path = os.path.join(idf_path, 'components', 'fatfs', 'fatfsparse.py')

    if config in ['test_read_write_partition_gen', 'test_read_write_partition_gen_default_dt']:
        filename = filename_sn
        filename_expected = f'/spiflash/{filename}'
        date_ref = date_default if config == 'test_read_write_partition_gen_default_dt' else date_modified
        expect_all(['example: Mounting FAT filesystem',
                    'example: Opening file',
                    'example: File written',
                    'example: Reading file',
                    'example: Read from file: \'This is written by the device\'',
                    'example: Reading file'], timeout)
        date_act = expect_date(f'The file \'{filename_expected}\' was modified at date: ', timeout)
        evaluate_dates(date_ref, date_act, tolerance)
        expect_all(['example: Read from file: \'This is generated on the host\'',
                    'example: Unmounting FAT filesystem',
                    'example: Done'], timeout)

        target = ParttoolTarget(dut.port)
        target.read_partition(PartitionName('storage'), 'temp.img')
        run(['python', fatfs_parser_path, '--wear-leveling', 'temp.img'], stderr=STDOUT)
        folder_ = {
            'type': 'folder',
            'name': 'SUB',
            'content': [
                file_('TEST.TXT', content_='this is test\n'),
            ]
        }
        struct_: dict = {
            'type': 'folder',
            'name': 'testf',
            'content': [
                file_('HELLO.TXT', content_='This is generated on the host\n'),
                file_('INNER.TXT', content_='This is written by the device'),
                folder_
            ]
        }
        generate_local_folder_structure(struct_, path_='.')
        try:
            assert compare_folders('testf', 'Espressif')
        finally:
            shutil.rmtree('Espressif', ignore_errors=True)
            shutil.rmtree('testf', ignore_errors=True)

    elif config in ['test_read_only_partition_gen', 'test_read_only_partition_gen_default_dt']:
        filename = filename_sn
        filename_expected = f'/spiflash/{filename}'
        date_ref = date_default if config == 'test_read_only_partition_gen_default_dt' else date_modified
        expect_all(['example: Mounting FAT filesystem',
                    'example: Reading file'], timeout)
        date_act = expect_date(f'The file \'{filename_expected}\' was modified at date: ', timeout)
        evaluate_dates(date_ref, date_act, tolerance)
        expect_all(['example: Read from file: \'this is test\'',
                    'example: Unmounting FAT filesystem',
                    'example: Done'], timeout)
        target = ParttoolTarget(dut.port)
        target.read_partition(PartitionName('storage'), 'temp.img')
        run(['python', fatfs_parser_path, '--long-name-support', 'temp.img'], stderr=STDOUT)
        folder_ = {
            'type': 'folder',
            'name': 'sublongnames',
            'content': [
                file_('testlongfilenames.txt', content_='this is test; long name it has\n'),
            ]
        }
        struct_ = {
            'type': 'folder',
            'name': 'testf',
            'content': [
                file_('hellolongname.txt', content_='This is generated on the host; long name it has\n'),
                folder_
            ]
        }
        generate_local_folder_structure(struct_, path_='.')
        try:
            assert compare_folders('testf', 'Espressif')
        finally:
            shutil.rmtree('Espressif', ignore_errors=True)
            shutil.rmtree('testf', ignore_errors=True)

    elif config in ['test_read_write_partition_gen_ln', 'test_read_write_partition_gen_ln_default_dt']:
        filename = filename_ln
        filename_expected = f'/spiflash/{filename}'
        date_ref = date_default if config == 'test_read_write_partition_gen_ln_default_dt' else date_modified
        expect_all(['example: Mounting FAT filesystem',
                    'example: Opening file',
                    'example: File written',
                    'example: Reading file',
                    'example: Read from file: \'This is written by the device\'',
                    'example: Reading file'], timeout)
        date_act = expect_date(f'The file \'{filename_expected}\' was modified at date: ', timeout)
        evaluate_dates(date_ref, date_act, tolerance)
        expect_all(['example: Read from file: \'This is generated on the host; long name it has\'',
                    'example: Unmounting FAT filesystem',
                    'example: Done'], timeout)

    elif config in ['test_read_only_partition_gen_ln', 'test_read_only_partition_gen_ln_default_dt']:
        filename = filename_ln
        filename_expected = f'/spiflash/{filename}'
        date_ref = date_default if config == 'test_read_only_partition_gen_ln_default_dt' else date_modified
        expect_all(['example: Mounting FAT filesystem',
                    'example: Reading file'], timeout)
        date_act = expect_date(f'The file \'{filename_expected}\' was modified at date: ', timeout)
        evaluate_dates(date_ref, date_act, tolerance)
        expect_all(['example: Read from file: \'this is test; long name it has\'',
                    'example: Unmounting FAT filesystem',
                    'example: Done'], timeout)
        target = ParttoolTarget(dut.port)
        target.read_partition(PartitionName('storage'), 'temp.img')
        run(['python', fatfs_parser_path, 'temp.img'], stderr=STDOUT)
        folder_ = {
            'type': 'folder',
            'name': 'SUB',
            'content': [
                file_('TEST.TXT', content_='this is test\n'),
            ]
        }
        struct_ = {
            'type': 'folder',
            'name': 'testf',
            'content': [
                file_('HELLO.TXT', content_='This is generated on the host\n'),
                folder_
            ]
        }
        generate_local_folder_structure(struct_, path_='.')
        try:
            assert compare_folders('testf', 'Espressif')
        finally:
            shutil.rmtree('Espressif', ignore_errors=True)
            shutil.rmtree('testf', ignore_errors=True)
