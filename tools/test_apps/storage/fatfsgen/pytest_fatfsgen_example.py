# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os
import re
import shutil
import sys
import typing as t
from datetime import datetime
from subprocess import run
from subprocess import STDOUT
from time import sleep

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

idf_path = os.environ['IDF_PATH']  # get value of IDF_PATH from environment
parttool_dir = os.path.join(idf_path, 'components', 'partition_table')


sys.path.append(parttool_dir)
from parttool import PartitionName, ParttoolTarget  # noqa E402  # pylint: disable=C0413


FileStructure = t.Dict[str, t.Union[t.Optional[str], 'FileStructure']]


class DirectoryStructureError(Exception):
    """Base exception for directory structure errors."""

    pass


class MissingStructureError(DirectoryStructureError):
    """Raised when a directory or file is missing."""

    pass


class ContentMismatchError(DirectoryStructureError):
    """Raised when a file's content does not match the expected content."""

    pass


class IOErrorInStructure(DirectoryStructureError):
    """Raised when an I/O error occurs during structure validation."""

    pass


def validate_directory_structure(base_path: str, expected_structure: FileStructure) -> None:
    """
    Checks if the directory structure
    and file contents match the expected structure.

    :param base_path: The root directory to check.
    :param expected_structure: A nested dictionary representing
                               the expected structure.
                               Keys are folder names or file names.

                               For files, values are either:
                                   - None (skip content check) or
                                   - string (expected content).

                               For folders values are dictionaries defining
                               their structure.

    :raises DirectoryStructureError: If the structure or contents do not match.
    """

    def normalize_case(items: t.List[str]) -> t.Dict[str, str]:
        return {item.lower(): item for item in items}

    def escape_output(text: str) -> str:
        return text.replace('"', '"').replace('\n', '\\n').replace('\t', '\\t').replace('\r', '\\r')

    def validate_structure(current_path: str, structure: FileStructure) -> None:
        current_items = os.listdir(current_path)
        normalized_items = normalize_case(current_items)

        for name, substructure in structure.items():
            expected_name = normalized_items.get(name.lower())
            if not expected_name:
                raise MissingStructureError(f'Missing item: {os.path.join(current_path, name)}')

            full_path = os.path.join(current_path, expected_name)

            # File
            if isinstance(substructure, str) or substructure is None:
                if not os.path.isfile(full_path):
                    raise MissingStructureError(f'Missing file: {full_path}')

                if isinstance(substructure, str):  # Check file content
                    try:
                        with open(full_path, 'r', encoding='utf-8') as file:
                            content = file.read()
                        if content != substructure:
                            raise ContentMismatchError(
                                f'Content mismatch in file: {full_path}\n'
                                f'Expected: "{escape_output(substructure)}"\n'
                                f'Found: "{escape_output(content)}"'
                            )
                    except OSError as e:
                        raise IOErrorInStructure(f'Error reading file {full_path}: {e}')

            # Folder
            elif isinstance(substructure, dict):
                if not os.path.isdir(full_path):
                    raise MissingStructureError(f'Missing directory: {full_path}')
                # Recursively check the subdirectory
                validate_structure(full_path, substructure)

            else:
                raise DirectoryStructureError(f'Invalid structure definition for {name}.')

    validate_structure(base_path, expected_structure)


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'test_read_only_partition_gen',
        'test_read_only_partition_gen_default_dt',
        'test_read_only_partition_gen_ln',
        'test_read_only_partition_gen_ln_default_dt',
        'test_read_write_partition_gen',
        'test_read_write_partition_gen_default_dt',
        'test_read_write_partition_gen_ln',
        'test_read_write_partition_gen_ln_default_dt',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_examples_fatfsgen(config: str, dut: Dut) -> None:
    # Default timeout - a bit overkill, but better than failing tests
    timeout = 600

    def expect(msg: str, timeout: int = timeout) -> None:
        dut.expect(msg, timeout=timeout)

    # Expects list of strings sequentially
    def expect_all(msg_list: t.List[str], timeout: int = timeout) -> None:
        for msg in msg_list:
            expect(msg, timeout)

    # Expects prefix string followed by date in the format 'yyyy-mm-dd'
    def expect_date(prefix: str, timeout: int = timeout) -> datetime:
        expect_str = prefix + '(\\d+)-(\\d+)-(\\d+)'
        match_ = dut.expect(re.compile(str.encode(expect_str)), timeout=timeout)
        year_ = int(match_[1].decode())
        month_ = int(match_[2].decode())
        day_ = int(match_[3].decode())
        return datetime(year_, month_, day_)

    # Calculates absolute difference in days
    # between date_reference and date_actual.
    # Raises exception if difference exceeds date_tolerance
    # 30 days by default,
    def check_dates(date_reference: datetime, date_actual: datetime, days_date_tolerance: int = 30) -> None:
        td = date_actual - date_reference
        if abs(td.days) > days_date_tolerance:
            raise Exception(
                f'Too big date difference. \
                Actual: {date_actual}, \
                reference: {date_reference}, \
                date_tolerance: {days_date_tolerance} day(s)'
            )

    fatfs_parser_path = os.path.join(idf_path, 'components', 'fatfs', 'fatfsparse.py')

    config_read_only = '_read_only' in config
    config_long_names = '_ln' in config
    config_default_date = '_default_dt' in config

    expect('example: Mounting FAT filesystem')

    if not config_read_only:
        expect_all([
            'example: Opening file',
            'example: File written',
            'example: Reading file',
            'example: Read from file: ' + "'This is written by the device'",
        ])

    expect('example: Reading file')

    stat_filename = 'sublongnames/testlongfilenames.txt' if config_long_names else 'sub/test.txt'

    modification_date = expect_date(f"The file '/spiflash/{stat_filename}' was modified at date: ")

    date_ref = datetime(1980, 1, 1) if config_default_date else datetime.today()

    check_dates(date_ref, modification_date)

    expect_all(['example: Unmounting FAT filesystem', 'example: Done'])

    dut.serial.close()
    sleep(1)

    target = ParttoolTarget(dut.serial.port, 1843200)
    target.read_partition(PartitionName('storage'), 'temp.img')
    if config_long_names:
        run(['python', fatfs_parser_path, '--long-name-support', 'temp.img'], stderr=STDOUT)
    else:
        run(['python', fatfs_parser_path, 'temp.img'], stderr=STDOUT)

    long_names_read_only: FileStructure = {
        'sublongnames': {
            'testlongfilenames.txt': 'this is test; long name it has\n',
        },
        'hellolongname.txt': 'This is generated on the host; long name it has\n',
    }

    short_names_read_only: FileStructure = {
        'sub': {
            'test.txt': 'this is test\n',
        },
        'hello.txt': 'This is generated on the host\n',
    }

    file_structure: FileStructure = long_names_read_only if config_long_names else short_names_read_only

    if not config_read_only and not config_long_names:
        file_structure['inner.txt'] = 'This is written by the device'

    try:
        validate_directory_structure('Espressif', file_structure)
    finally:
        shutil.rmtree('Espressif', ignore_errors=True)
