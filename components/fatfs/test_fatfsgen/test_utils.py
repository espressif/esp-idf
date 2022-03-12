# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import sys
from typing import Any, Dict, Union

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
import fatfsgen  # noqa E402  # pylint: disable=C0413

CFG = dict(
    sector_size=4096,
    entry_size=32,
    fat_start=0x1000,
    data_start=0x7000,
    root_start=0x2000,
    output_file=os.path.join('output_data', 'tmp_file.img'),
    test_dir=os.path.join('output_data', 'test'),
    test_dir2=os.path.join('output_data', 'tst_str'),
)  # type: Union[Dict[str, Any]]


def generate_test_dir_1() -> None:
    os.makedirs(os.path.join(CFG['test_dir'], 'test', 'test'))
    with open(os.path.join(CFG['test_dir'], 'test', 'test', 'lastfile'), 'w') as file:
        file.write('deeptest\n')
    with open(os.path.join(CFG['test_dir'], 'test', 'testfil2'), 'w') as file:
        file.write('thisistest\n')
    with open(os.path.join(CFG['test_dir'], 'testfile'), 'w') as file:
        file.write('ahoj\n')


def generate_test_dir_2() -> None:
    os.makedirs(os.path.join(CFG['test_dir2'], 'test', 'test'))
    with open(os.path.join(CFG['test_dir2'], 'test', 'test', 'lastfile.txt'), 'w') as file:
        file.write('deeptest\n')
    with open(os.path.join(CFG['test_dir2'], 'test', 'testfil2'), 'w') as file:
        file.write('thisistest\n')
    with open(os.path.join(CFG['test_dir2'], 'testfile'), 'w') as file:
        file.write('ahoj\n')


def fill_sector(fatfs: fatfsgen.FATFS, file_prefix: str = 'A') -> None:
    for i in range(CFG['sector_size'] // CFG['entry_size']):
        fatfs.create_file(f'{file_prefix}{str(i).upper()}', path_from_root=['TESTFOLD'])
