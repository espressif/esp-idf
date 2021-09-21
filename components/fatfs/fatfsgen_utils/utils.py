# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import typing

from construct import Int16ul


def required_clusters_count(cluster_size: int, content: str) -> int:
    # compute number of required clusters for file text
    return (len(content) + cluster_size - 1) // cluster_size


def pad_string(content: str, size: typing.Optional[int] = None, pad: int = 0x20) -> str:
    # cut string if longer and fill with pad character if shorter than size
    return content.ljust(size or len(content), chr(pad))[:size]


def split_to_name_and_extension(full_name: str) -> typing.Tuple[str, str]:
    name, extension = os.path.splitext(full_name)
    return name, extension.replace('.', '')


def is_valid_fatfs_name(string: str) -> bool:
    return string == string.upper()


def split_by_half_byte_12_bit_little_endian(value: int) -> typing.Tuple[int, int, int]:
    value_as_bytes = Int16ul.build(value)
    return value_as_bytes[0] & 0x0f, value_as_bytes[0] >> 4, value_as_bytes[1] & 0x0f


def build_byte(first_half: int, second_half: int) -> int:
    return (first_half << 4) | second_half


def clean_first_half_byte(bytes_array: bytearray, address: int) -> None:
    """
    the function sets to zero first four bits of the byte.
    E.g. 10111100 -> 10110000
    """
    bytes_array[address] &= 0xf0


def clean_second_half_byte(bytes_array: bytearray, address: int) -> None:
    """
    the function sets to zero last four bits of the byte.
    E.g. 10111100 -> 00001100
    """
    bytes_array[address] &= 0x0f


def split_content_into_sectors(content: str, sector_size: int) -> typing.List[str]:
    result = []
    clusters_cnt = required_clusters_count(cluster_size=sector_size, content=content)

    for i in range(clusters_cnt):
        result.append(content[sector_size * i:(i + 1) * sector_size])
    return result
