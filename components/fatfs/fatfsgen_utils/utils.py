# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import binascii
import os
import uuid
from typing import List, Optional, Tuple

from construct import Int16ul


def crc32(input_values: List[int], crc: int) -> int:
    """
    Name    Polynomial  Reversed?   Init-value                  XOR-out
    crc32   0x104C11DB7 True        4294967295 (UINT32_MAX)     0xFFFFFFFF
    """
    return binascii.crc32(bytearray(input_values), crc)


def required_clusters_count(cluster_size: int, content: bytes) -> int:
    # compute number of required clusters for file text
    return (len(content) + cluster_size - 1) // cluster_size


def generate_4bytes_random() -> int:
    return uuid.uuid4().int & 0xFFFFFFFF


def pad_string(content: str, size: Optional[int] = None, pad: int = 0x20) -> str:
    # cut string if longer and fill with pad character if shorter than size
    return content.ljust(size or len(content), chr(pad))[:size]


def split_to_name_and_extension(full_name: str) -> Tuple[str, str]:
    name, extension = os.path.splitext(full_name)
    return name, extension.replace('.', '')


def is_valid_fatfs_name(string: str) -> bool:
    return string == string.upper()


def split_by_half_byte_12_bit_little_endian(value: int) -> Tuple[int, int, int]:
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


def split_content_into_sectors(content: bytes, sector_size: int) -> List[bytes]:
    result = []
    clusters_cnt = required_clusters_count(cluster_size=sector_size, content=content)

    for i in range(clusters_cnt):
        result.append(content[sector_size * i:(i + 1) * sector_size])
    return result


def get_args_for_partition_generator(desc: str) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=desc)
    parser.add_argument('input_directory',
                        help='Path to the directory that will be encoded into fatfs image')
    parser.add_argument('--output_file',
                        default='fatfs_image.img',
                        help='Filename of the generated fatfs image')
    parser.add_argument('--partition_size',
                        default=1024 * 1024,
                        help='Size of the partition in bytes')
    parser.add_argument('--sector_size',
                        default=4096,
                        help='Size of the partition in bytes')
    args = parser.parse_args()
    if not os.path.isdir(args.input_directory):
        raise NotADirectoryError(f'The target directory `{args.input_directory}` does not exist!')
    return args
