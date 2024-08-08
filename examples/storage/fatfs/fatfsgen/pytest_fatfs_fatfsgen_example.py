# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os
import re
from datetime import datetime
from typing import List

import pytest
from pytest_embedded import Dut

idf_path = os.environ['IDF_PATH']  # get value of IDF_PATH from environment
parttool_dir = os.path.join(idf_path, 'components', 'partition_table')


@pytest.mark.esp32
@pytest.mark.generic
@pytest.mark.parametrize('config', ['test_read_only_partition_gen_ln',
                                    'test_read_write_partition_gen_ln',
                                    ], indirect=True)
def test_examples_fatfs_fatfsgen(config: str, dut: Dut) -> None:
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
    filename = 'sublongnames/testlongfilenames.txt'
    date_ref = datetime.today()

    if config in ['test_read_write_partition_gen']:
        filename_expected = f'/spiflash/{filename}'
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

    elif config in ['test_read_only_partition_gen']:
        filename_expected = f'/spiflash/{filename}'
        expect_all(['example: Mounting FAT filesystem',
                    'example: Reading file'], timeout)
        date_act = expect_date(f'The file \'{filename_expected}\' was modified at date: ', timeout)
        evaluate_dates(date_ref, date_act, tolerance)
        expect_all(['example: Read from file: \'this is test\'',
                    'example: Unmounting FAT filesystem',
                    'example: Done'], timeout)
