# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import re
from datetime import datetime
from typing import List

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'test_read_only_partition_gen',
        'test_read_only_partition_gen_default_dt',
        'test_read_write_partition_gen',
        'test_read_write_partition_gen_default_dt',
    ],
    indirect=True,
)
def test_examples_fatfs_advanced(config: str, dut: Dut) -> None:
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
    def evaluate_dates(
        date_reference: datetime, date_actual: datetime, days_tolerance: int
    ) -> None:
        td = date_actual - date_reference
        if abs(td.days) > days_tolerance:
            raise Exception(
                f'Too big date difference. Actual: {date_actual}, reference: {date_reference}, tolerance: {days_tolerance} day(s)'
            )

    # Expect timeout
    timeout = 20

    # We tolerate 30 days difference between actual file creation and date when test was executed.
    tolerance = 30

    expected_date = (
        datetime(1980, 1, 1) if config.endswith('default_dt') else datetime.today()
    )

    base_path = '/spiflash/'
    folder_name = 'subdirectoryfromhost/'
    read_filename = base_path + 'filegeneratedonhost.txt'
    write_filename = base_path + 'messagefromthedevice.txt'
    stat_filename = base_path + folder_name + 'innerfile.txt'

    dut.expect('example: Mounting FAT filesystem', timeout=timeout)

    # Check read
    expect_all(
        [
            f'example: Opening file \'{read_filename}\' for reading',
            f'example: Reading from file',
            'example: Read from file: \'This is generated on the host; it has long name\'',
            f'example: Closing file \'{read_filename}\'',
        ],
        timeout,
    )

    # Check write
    if config.startswith('test_read_write'):
        expect_all(
            [
                f'example: Opening file \'{write_filename}\' for writing',
                'example: Writing to file',
                'example: File written',
                'example: Closing file',
                f'example: Opening file \'{write_filename}\' for reading',
                f'example: Reading from file',
                'example: Read from file: \'This is written by the device\'',
                f'example: Closing file \'{write_filename}\'',
            ],
            timeout,
        )

    # Check date using stat
    dut.expect(
        f'example: Stating file \'{stat_filename}\' for modification time', timeout=timeout
    )

    actual_date = expect_date(
        f'The file \'{stat_filename}\' was modified at date: ', timeout
    )

    evaluate_dates(expected_date, actual_date, tolerance)

    # Finish
    expect_all(
        [
            'example: Unmounting FAT filesystem',
            'example: Done',
        ],
        timeout,
    )
