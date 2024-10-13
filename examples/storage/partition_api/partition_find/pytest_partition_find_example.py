# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import re

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.generic
def test_partition_find_example(dut: Dut) -> None:
    def expect_partition(name: str, offset: int, size: int) -> None:
        dut.expect(re.compile(str.encode("found partition '{}' at offset {:#x} with size {:#x}".format(name, offset, size))), timeout=5)

    def expect_find_partition(_type: str, subtype: str, label: str, name: str, offset: int, size: int) -> None:
        dut.expect(re.compile(str.encode('Find partition with type {}, subtype {}, label {}'.format(_type, subtype, label))), timeout=5)
        expect_partition(name, offset, size)

    dut.expect('----------------Find partitions---------------', timeout=20)

    expect_find_partition('ESP_PARTITION_TYPE_DATA', 'ESP_PARTITION_SUBTYPE_DATA_NVS', 'NULL',
                          'nvs', 0x9000, 0x6000)
    expect_find_partition('ESP_PARTITION_TYPE_DATA', 'ESP_PARTITION_SUBTYPE_DATA_PHY', 'NULL',
                          'phy_init', 0xf000,  0x1000)
    expect_find_partition('ESP_PARTITION_TYPE_APP', 'ESP_PARTITION_SUBTYPE_APP_FACTORY', 'NULL',
                          'factory', 0x10000, 0x100000)
    expect_find_partition('ESP_PARTITION_TYPE_DATA', 'ESP_PARTITION_SUBTYPE_DATA_FAT', 'NULL',
                          'storage1', 0x110000, 0x40000)

    dut.expect('Find second FAT partition by specifying the label', timeout=5)

    expect_find_partition('ESP_PARTITION_TYPE_DATA', 'ESP_PARTITION_SUBTYPE_DATA_FAT', 'storage2',
                          'storage2', 0x150000, 0x40000)

    dut.expect('----------------Iterate through partitions---------------',timeout=5)
    dut.expect('Iterating through app partitions...', timeout=5)

    expect_partition('factory', 0x10000, 0x100000)

    dut.expect('Iterating through data partitions...', timeout=5)

    expect_partition('nvs', 0x9000, 0x6000)
    expect_partition('phy_init', 0xf000,  0x1000)
    expect_partition('storage1', 0x110000, 0x40000)
    expect_partition('storage2', 0x150000, 0x40000)

    dut.expect('Example end', timeout=5)
