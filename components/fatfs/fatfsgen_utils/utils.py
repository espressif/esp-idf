# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import binascii
import os
import uuid
from typing import List, Optional, Tuple

from construct import Int16ul

FAT12_MAX_CLUSTERS: int = 4085
FAT16_MAX_CLUSTERS: int = 65525
FAT12: int = 12
FAT16: int = 16
FAT32: int = 32
BYTES_PER_DIRECTORY_ENTRY: int = 32
UINT32_MAX: int = (1 << 32) - 1
MAX_NAME_SIZE: int = 8
MAX_EXT_SIZE: int = 3

# long names are encoded to two bytes in utf-16
LONG_NAMES_ENCODING: str = 'utf-16'


def crc32(input_values: List[int], crc: int) -> int:
    """
    Name    Polynomial  Reversed?   Init-value                  XOR-out
    crc32   0x104C11DB7 True        4294967295 (UINT32_MAX)     0xFFFFFFFF
    """
    return binascii.crc32(bytearray(input_values), crc)


def number_of_clusters(number_of_sectors: int, sectors_per_cluster: int) -> int:
    return number_of_sectors // sectors_per_cluster


def get_non_data_sectors_cnt(reserved_sectors_cnt: int, sectors_per_fat_cnt: int, root_dir_sectors_cnt: int) -> int:
    return reserved_sectors_cnt + sectors_per_fat_cnt + root_dir_sectors_cnt


def get_fatfs_type(clusters_count: int) -> int:
    if clusters_count < FAT12_MAX_CLUSTERS:
        return FAT12
    if clusters_count <= FAT16_MAX_CLUSTERS:
        return FAT16
    return FAT32


def required_clusters_count(cluster_size: int, content: bytes) -> int:
    # compute number of required clusters for file text
    return (len(content) + cluster_size - 1) // cluster_size


def generate_4bytes_random() -> int:
    return uuid.uuid4().int & 0xFFFFFFFF


def pad_string(content: str, size: Optional[int] = None, pad: int = 0x20) -> str:
    # cut string if longer and fill with pad character if shorter than size
    return content.ljust(size or len(content), chr(pad))[:size]


def build_lfn_short_entry_name(name: str, extension: str, order: int) -> str:
    return '{}{}'.format(pad_string(content=name[:MAX_NAME_SIZE - 2] + '~' + chr(order), size=MAX_NAME_SIZE),
                         pad_string(extension[:MAX_EXT_SIZE], size=MAX_EXT_SIZE))


def lfn_checksum(short_entry_name: str) -> int:
    """
    Function defined by FAT specification. Computes checksum out of name in the short file name entry.
    """
    checksum_result = 0
    for i in range(MAX_NAME_SIZE + MAX_EXT_SIZE):
        # operation is a right rotation on 8 bits (Python equivalent for unsigned char in C)
        checksum_result = (0x80 if checksum_result & 1 else 0x00) + (checksum_result >> 1) + ord(short_entry_name[i])
        checksum_result &= 0xff
    return checksum_result


def convert_to_utf16_and_pad(content: str,
                             expected_size: int,
                             pad: bytes = b'\xff',
                             terminator: bytes = b'\x00\x00') -> bytes:
    # we need to get rid of the Byte order mark 0xfeff or 0xfffe, fatfs does not use it
    bom_utf16: bytes = b'\xfe\xff'
    encoded_content_utf16: bytes = content.encode(LONG_NAMES_ENCODING)[len(bom_utf16):]
    terminated_encoded_content_utf16: bytes = (encoded_content_utf16 + terminator) if (2 * expected_size > len(
        encoded_content_utf16) > 0) else encoded_content_utf16
    return terminated_encoded_content_utf16.ljust(2 * expected_size, pad)


def split_to_name_and_extension(full_name: str) -> Tuple[str, str]:
    name, extension = os.path.splitext(full_name)
    return name, extension.replace('.', '')


def is_valid_fatfs_name(string: str) -> bool:
    return string == string.upper()


def split_by_half_byte_12_bit_little_endian(value: int) -> Tuple[int, int, int]:
    value_as_bytes: bytes = Int16ul.build(value)
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
    clusters_cnt: int = required_clusters_count(cluster_size=sector_size, content=content)

    for i in range(clusters_cnt):
        result.append(content[sector_size * i:(i + 1) * sector_size])
    return result


def get_args_for_partition_generator(desc: str) -> argparse.Namespace:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(description=desc)
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
                        type=int,
                        choices=[512, 1024, 2048, 4096],
                        help='Size of the partition in bytes')
    parser.add_argument('--sectors_per_cluster',
                        default=1,
                        type=int,
                        choices=[1, 2, 4, 8, 16, 32, 64, 128],
                        help='Number of sectors per cluster')
    parser.add_argument('--root_entry_count',
                        default=512,
                        help='Number of entries in the root directory')
    parser.add_argument('--long_name_support',
                        action='store_true',
                        help='Set flag to enable long names support.')
    parser.add_argument('--fat_type',
                        default=0,
                        type=int,
                        choices=[12, 16, 0],
                        help="""
                        Type of fat. Select 12 for fat12, 16 for fat16. Don't set, or set to 0 for automatic
                        calculation using cluster size and partition size.
                        """)

    args = parser.parse_args()
    if args.fat_type == 0:
        args.fat_type = None
    args.partition_size = int(str(args.partition_size), 0)
    if not os.path.isdir(args.input_directory):
        raise NotADirectoryError(f'The target directory `{args.input_directory}` does not exist!')
    return args


def read_filesystem(path: str) -> bytearray:
    with open(path, 'rb') as fs_file:
        return bytearray(fs_file.read())
