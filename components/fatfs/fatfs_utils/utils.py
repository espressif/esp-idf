# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import binascii
import os
import re
import uuid
from datetime import datetime
from typing import List
from typing import Optional
from typing import Tuple

from construct import BitsInteger
from construct import BitStruct
from construct import Int16ul

# the regex pattern defines symbols that are allowed by long file names but not by short file names
INVALID_SFN_CHARS_PATTERN = re.compile(r'[.+,;=\[\]]')

FATFS_MIN_ALLOC_UNIT: int = 128
FAT12_MAX_CLUSTERS: int = 4085
FAT16_MAX_CLUSTERS: int = 65525
RESERVED_CLUSTERS_COUNT: int = 2
PAD_CHAR: int = 0x20
FAT12: int = 12
FAT16: int = 16
FAT32: int = 32
FULL_BYTE: bytes = b'\xff'
EMPTY_BYTE: bytes = b'\x00'
# redundant
BYTES_PER_DIRECTORY_ENTRY: int = 32
UINT32_MAX: int = (1 << 32) - 1
MAX_NAME_SIZE: int = 8
MAX_EXT_SIZE: int = 3
DATETIME = Tuple[int, int, int]
FATFS_INCEPTION_YEAR: int = 1980

FATFS_INCEPTION: datetime = datetime(FATFS_INCEPTION_YEAR, 1, 1, 0, 0, 0, 0)

FATFS_MAX_HOURS = 24
FATFS_MAX_MINUTES = 60
FATFS_MAX_SECONDS = 60

FATFS_MAX_DAYS = 31
FATFS_MAX_MONTHS = 12
FATFS_MAX_YEARS = 127

FATFS_SECONDS_GRANULARITY: int = 2

# long names are encoded to two bytes in utf-16
LONG_NAMES_ENCODING: str = 'utf-16'
SHORT_NAMES_ENCODING: str = 'utf-8'

# compatible with WL_SECTOR_SIZE
# choices for WL are WL_SECTOR_SIZE_512 and WL_SECTOR_SIZE_4096
ALLOWED_WL_SECTOR_SIZES: List[int] = [512, 4096]
ALLOWED_SECTOR_SIZES: List[int] = [512, 1024, 2048, 4096]

ALLOWED_SECTORS_PER_CLUSTER: List[int] = [1, 2, 4, 8, 16, 32, 64, 128]


def crc32(input_values: List[int], crc: int) -> int:
    """
    Name    Polynomial  Reversed?   Init-value                  XOR-out
    crc32   0x104C11DB7 True        4294967295 (UINT32_MAX)     0xFFFFFFFF
    """
    return binascii.crc32(bytearray(input_values), crc)


def number_of_clusters(number_of_sectors: int, sectors_per_cluster: int) -> int:
    return number_of_sectors // sectors_per_cluster


def get_non_data_sectors_cnt(reserved_sectors_cnt: int, sectors_per_fat_cnt: int, fat_tables_cnt: int, root_dir_sectors_cnt: int) -> int:
    return reserved_sectors_cnt + sectors_per_fat_cnt * fat_tables_cnt + root_dir_sectors_cnt


def get_fatfs_type(clusters_count: int) -> int:
    if clusters_count < FAT12_MAX_CLUSTERS:
        return FAT12
    if clusters_count <= FAT16_MAX_CLUSTERS:
        return FAT16
    return FAT32


def get_fat_sectors_count(clusters_count: int, sector_size: int) -> int:
    fatfs_type_ = get_fatfs_type(clusters_count)
    if fatfs_type_ == FAT32:
        raise NotImplementedError('FAT32 is not supported!')
    # number of byte halves
    cluster_s: int = fatfs_type_ // 4
    fat_size_bytes: int = (
        clusters_count * 2 + cluster_s) if fatfs_type_ == FAT16 else (clusters_count * 3 + 1) // 2 + cluster_s
    return (fat_size_bytes + sector_size - 1) // sector_size


def required_clusters_count(cluster_size: int, content: bytes) -> int:
    # compute number of required clusters for file text
    return (len(content) + cluster_size - 1) // cluster_size


def generate_4bytes_random() -> int:
    return uuid.uuid4().int & 0xFFFFFFFF


def pad_string(content: str, size: Optional[int] = None, pad: int = PAD_CHAR) -> str:
    # cut string if longer and fill with pad character if shorter than size
    return content.ljust(size or len(content), chr(pad))[:size]


def right_strip_string(content: str, pad: int = PAD_CHAR) -> str:
    return content.rstrip(chr(pad))


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
                             pad: bytes = FULL_BYTE) -> bytes:
    # we need to get rid of the Byte order mark 0xfeff or 0xfffe, fatfs does not use it
    bom_utf16: bytes = b'\xfe\xff'
    encoded_content_utf16: bytes = content.encode(LONG_NAMES_ENCODING)[len(bom_utf16):]
    return encoded_content_utf16.ljust(2 * expected_size, pad)


def split_to_name_and_extension(full_name: str) -> Tuple[str, str]:
    name, extension = os.path.splitext(full_name)
    return name, extension.replace('.', '')


def is_valid_fatfs_name(string: str) -> bool:
    return string == string.upper()


def split_by_half_byte_12_bit_little_endian(value: int) -> Tuple[int, int, int]:
    value_as_bytes: bytes = Int16ul.build(value)
    return value_as_bytes[0] & 0x0f, value_as_bytes[0] >> 4, value_as_bytes[1] & 0x0f


def merge_by_half_byte_12_bit_little_endian(v1: int, v2: int, v3: int) -> int:
    return v1 | v2 << 4 | v3 << 8


def build_byte(first_half: int, second_half: int) -> int:
    return (first_half << 4) | second_half


def split_content_into_sectors(content: bytes, sector_size: int) -> List[bytes]:
    result = []
    clusters_cnt: int = required_clusters_count(cluster_size=sector_size, content=content)

    for i in range(clusters_cnt):
        result.append(content[sector_size * i:(i + 1) * sector_size])
    return result


def get_args_for_partition_generator(desc: str, wl: bool) -> argparse.Namespace:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(description=desc)
    parser.add_argument('input_directory',
                        help='Path to the directory that will be encoded into fatfs image')
    parser.add_argument('--output_file',
                        default='fatfs_image.img',
                        help='Filename of the generated fatfs image')
    parser.add_argument('--partition_size',
                        default=FATDefaults.SIZE,
                        help='Size of the partition in bytes.' +
                             ('' if wl else ' Use `--partition_size detect` for detecting the minimal partition size.')
                        )
    parser.add_argument('--sector_size',
                        default=FATDefaults.SECTOR_SIZE,
                        type=int,
                        choices=ALLOWED_WL_SECTOR_SIZES if wl else ALLOWED_SECTOR_SIZES,
                        help='Size of the partition in bytes')
    parser.add_argument('--sectors_per_cluster',
                        default=1,
                        type=int,
                        choices=ALLOWED_SECTORS_PER_CLUSTER,
                        help='Number of sectors per cluster')
    parser.add_argument('--root_entry_count',
                        default=FATDefaults.ROOT_ENTRIES_COUNT,
                        help='Number of entries in the root directory')
    parser.add_argument('--long_name_support',
                        action='store_true',
                        help='Set flag to enable long names support.')
    parser.add_argument('--use_default_datetime',
                        action='store_true',
                        help='For test purposes. If the flag is set the files are created with '
                             'the default timestamp that is the 1st of January 1980')
    parser.add_argument('--fat_type',
                        default=0,
                        type=int,
                        choices=[FAT12, FAT16, 0],
                        help="""
                        Type of the FAT file-system. Select '12' for FAT12, '16' for FAT16.
                        Leave unset or select 0 for automatic file-system type detection.
                        """)
    parser.add_argument('--fat_count',
                        default=FATDefaults.FAT_TABLES_COUNT,
                        type=int,
                        choices=[1, 2],
                        help='Number of file allocation tables (FATs) in the filesystem.')
    parser.add_argument('--wl_mode',
                        default=None,
                        type=str,
                        choices=['safe', 'perf'],
                        help='Wear levelling mode to use. Safe or performance. Only for sector size of 512')

    args = parser.parse_args()
    if args.fat_type == 0:
        args.fat_type = None
    if args.partition_size == 'detect' and not wl:
        args.partition_size = -1
    args.partition_size = int(str(args.partition_size), 0)
    if not os.path.isdir(args.input_directory):
        raise NotADirectoryError(f'The target directory `{args.input_directory}` does not exist!')
    if args.wl_mode is not None:
        if args.sector_size != 512:
            raise ValueError('Wear levelling mode can be set only for sector size 512')
    return args


def read_filesystem(path: str) -> bytearray:
    with open(path, 'rb') as fs_file:
        return bytearray(fs_file.read())


DATE_ENTRY = BitStruct(
    'year' / BitsInteger(7),
    'month' / BitsInteger(4),
    'day' / BitsInteger(5))

TIME_ENTRY = BitStruct(
    'hour' / BitsInteger(5),
    'minute' / BitsInteger(6),
    'second' / BitsInteger(5),
)


def build_name(name: str, extension: str) -> str:
    return f'{name}.{extension}' if len(extension) > 0 else name


def build_date_entry(year: int, mon: int, mday: int) -> int:
    """
    :param year: denotes year starting from 1980 (0 ~ 1980, 1 ~ 1981, etc), valid values are 1980 + 0..127 inclusive
    thus theoretically 1980 - 2107
    :param mon: denotes number of month of year in common order (1 ~ January, 2 ~ February, etc.),
    valid values: 1..12 inclusive
    :param mday: denotes number of day in month, valid values are 1..31 inclusive

    :returns: 16 bit integer number (7 bits for year, 4 bits for month and 5 bits for day of the month)
    """
    assert year in range(FATFS_INCEPTION_YEAR, FATFS_INCEPTION_YEAR + FATFS_MAX_YEARS)
    assert mon in range(1, FATFS_MAX_MONTHS + 1)
    assert mday in range(1, FATFS_MAX_DAYS + 1)
    return int.from_bytes(DATE_ENTRY.build(dict(year=year - FATFS_INCEPTION_YEAR, month=mon, day=mday)), 'big')


def build_time_entry(hour: int, minute: int, sec: int) -> int:
    """
    :param hour: denotes number of hour, valid values are 0..23 inclusive
    :param minute: denotes minutes, valid range 0..59 inclusive
    :param sec: denotes seconds with granularity 2 seconds (e.g. 1 ~ 2, 29 ~ 58), valid range 0..29 inclusive

    :returns: 16 bit integer number (5 bits for hour, 6 bits for minute and 5 bits for second)
    """
    assert hour in range(FATFS_MAX_HOURS)
    assert minute in range(FATFS_MAX_MINUTES)
    assert sec in range(FATFS_MAX_SECONDS)
    return int.from_bytes(TIME_ENTRY.build(
        dict(hour=hour, minute=minute, second=sec // FATFS_SECONDS_GRANULARITY)),
        byteorder='big'
    )


class FATDefaults:
    # FATFS defaults
    SIZE: int = 1024 * 1024
    RESERVED_SECTORS_COUNT: int = 1
    FAT_TABLES_COUNT: int = 2
    SECTORS_PER_CLUSTER: int = 1
    SECTOR_SIZE: int = 0x1000
    HIDDEN_SECTORS: int = 0
    ENTRY_SIZE: int = 32
    NUM_HEADS: int = 0xff
    OEM_NAME: str = 'MSDOS5.0'
    SEC_PER_TRACK: int = 0x3f
    VOLUME_LABEL: str = 'Espressif'
    FILE_SYS_TYPE: str = 'FAT'
    ROOT_ENTRIES_COUNT: int = 512  # number of entries in the root directory, recommended 512
    MEDIA_TYPE: int = 0xf8
    SIGNATURE_WORD: bytes = b'\x55\xAA'

    # wear levelling defaults
    VERSION: int = 2
    TEMP_BUFFER_SIZE: int = 32
    UPDATE_RATE: int = 16
    WR_SIZE: int = 16
    # wear leveling metadata (config sector) contains always sector size 4096
    WL_SECTOR_SIZE: int = 4096
