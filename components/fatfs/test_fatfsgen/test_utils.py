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
