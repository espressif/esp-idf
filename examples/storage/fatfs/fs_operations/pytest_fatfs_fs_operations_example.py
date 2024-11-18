# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
from datetime import datetime
from typing import List

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.generic
def test_examples_fatfs_fs_operations(config: str, dut: Dut) -> None:
    # Expects list of strings sequentially
    def expect_all(msg_list: List[str], to: int) -> None:
        for msg in msg_list:
            dut.expect(msg, timeout=to)

    def parse_date() -> datetime:
        months = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']

        pattern = r'([A-Z][a-z]{2}) ([A-Z][a-z]{2}) ([ \d]\d) (\d{2}):(\d{2}):(\d{2}) (\d{4})'

        match = dut.expect(pattern)
        return datetime(
            month=months.index(match[2].decode('utf-8')) + 1,
            day=int(match[3]),
            hour=int(match[4]),
            minute=int(match[5]),
            second=int(match[6]),
            year=int(match[7]),
        )

    expect_all(
        [
            'example: Mounting FAT filesystem',
            'example: Creating a file',
            'example: Writing to the file',
            'example: File stats:',
            'File size:',
        ],
        5
    )

    original = parse_date()

    expect_all(
        [
            'example: Wait for 3 seconds',
            'example: Write more to the file',
            'example: Force cached data and metadata to the filesystem',
            'File size:',
        ],
        5
    )

    updated = parse_date()

    assert updated > original

    expect_all(
        [
            'example: Go to the beginning of the file',
            'example: Reading from file',
            'Hello World!',
            'Hello World!',
            'example: Closing file',
            'example: Listing files in /spiflash:',
            'hello.txt',
            'example: Creating a new directory',
            'example: Listing files in /spiflash:',
            'hello.txt',
            'new_dir',
            'example: Rename a file',
            'example: Listing files in /spiflash:',
            'new_dir',
            'example: Listing files in /spiflash/new_dir:',
            'hello_renamed.txt',
        ],
        5
    )
