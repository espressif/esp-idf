# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import binascii
import os
import re
import uuid
from dataclasses import dataclass
from datetime import datetime
from typing import Any
from typing import cast

from construct import BitsInteger
from construct import BitStruct
from construct import Int16ul
from esp_pylib.logger import log

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
DATETIME = tuple[int, int, int]
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
ALLOWED_WL_SECTOR_SIZES: list[int] = [512, 4096]
ALLOWED_SECTOR_SIZES: list[int] = [512, 1024, 2048, 4096]

ALLOWED_SECTORS_PER_CLUSTER: list[int] = [1, 2, 4, 8, 16, 32, 64, 128]


def crc32(input_values: list[int], crc: int) -> int:
    """
    Name    Polynomial  Reversed?   Init-value                  XOR-out
    crc32   0x104C11DB7 True        4294967295 (UINT32_MAX)     0xFFFFFFFF
    """
    return binascii.crc32(bytearray(input_values), crc)


def number_of_clusters(number_of_sectors: int, sectors_per_cluster: int) -> int:
    return number_of_sectors // sectors_per_cluster


def get_non_data_sectors_cnt(
    reserved_sectors_cnt: int, sectors_per_fat_cnt: int, fat_tables_cnt: int, root_dir_sectors_cnt: int
) -> int:
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
        (clusters_count * 2 + cluster_s) if fatfs_type_ == FAT16 else (clusters_count * 3 + 1) // 2 + cluster_s
    )
    return (fat_size_bytes + sector_size - 1) // sector_size


def required_clusters_count(cluster_size: int, content: bytes) -> int:
    # compute number of required clusters for file text
    return (len(content) + cluster_size - 1) // cluster_size


def generate_4bytes_random() -> int:
    return uuid.uuid4().int & 0xFFFFFFFF


def pad_string(content: str, size: int | None = None, pad: int = PAD_CHAR) -> str:
    # cut string if longer and fill with pad character if shorter than size
    return content.ljust(size or len(content), chr(pad))[:size]


def right_strip_string(content: str, pad: int = PAD_CHAR) -> str:
    return content.rstrip(chr(pad))


def _gen_numname_suffix(seq: int, lfn: str) -> str:
    """
    Generate the numeric tail suffix for a short filename entry, matching
    the logic of gen_numname() in ff.c.

    For seq > 5, a CRC-based hash is computed from seq and the LFN to reduce
    collision probability. The suffix is rendered as hexadecimal digits
    (e.g. '~1', '~A', '~3F2') and always starts with '~'.
    """
    if seq > 5:
        # Hash path: CRC16-CCITT seeded with seq, fed with LFN characters
        sreg = seq
        for ch in lfn:
            wc = ord(ch)
            for _ in range(16):
                sreg = (sreg << 1) + (wc & 1)
                wc >>= 1
                if sreg & 0x10000:
                    sreg ^= 0x11021
        seq = sreg & 0xFFFF

    # Convert seq to uppercase hexadecimal digits (no '0x' prefix)
    hex_str = format(seq, 'X')
    return '~' + hex_str


def build_lfn_short_entry_name(name: str, extension: str, order: int, lfn: str = '') -> str:
    """
    Build the 8.3 short entry name for a long filename entry.

    Mirrors gen_numname() from ff.c: the suffix ('~' + hex digits) is built
    first, then the stem (beginning of the long name) is truncated to fit
    within MAX_NAME_SIZE (8) characters together with the suffix.
    """
    suffix = _gen_numname_suffix(order, lfn)
    name_part = name[: MAX_NAME_SIZE - len(suffix)] + suffix
    padded_name = pad_string(content=name_part, size=MAX_NAME_SIZE)
    padded_ext = pad_string(extension[:MAX_EXT_SIZE], size=MAX_EXT_SIZE)
    return f'{padded_name}{padded_ext}'


def lfn_checksum(short_entry_name: str) -> int:
    """
    Function defined by FAT specification. Computes checksum out of name in the short file name entry.
    """
    checksum_result = 0
    for i in range(MAX_NAME_SIZE + MAX_EXT_SIZE):
        # operation is a right rotation on 8 bits (Python equivalent for unsigned char in C)
        checksum_result = (0x80 if checksum_result & 1 else 0x00) + (checksum_result >> 1) + ord(short_entry_name[i])
        checksum_result &= 0xFF
    return checksum_result


def convert_to_utf16_and_pad(content: str, expected_size: int, pad: bytes = FULL_BYTE) -> bytes:
    # we need to get rid of the Byte order mark 0xfeff or 0xfffe, fatfs does not use it
    bom_utf16: bytes = b'\xfe\xff'
    encoded_content_utf16: bytes = content.encode(LONG_NAMES_ENCODING)[len(bom_utf16) :]
    return encoded_content_utf16.ljust(2 * expected_size, pad)


def split_to_name_and_extension(full_name: str) -> tuple[str, str]:
    name, extension = os.path.splitext(full_name)
    return name, extension.replace('.', '')


def is_valid_fatfs_name(string: str) -> bool:
    return string == string.upper()


def split_by_half_byte_12_bit_little_endian(value: int) -> tuple[int, int, int]:
    value_as_bytes: bytes = Int16ul.build(value)
    return value_as_bytes[0] & 0x0F, value_as_bytes[0] >> 4, value_as_bytes[1] & 0x0F


def merge_by_half_byte_12_bit_little_endian(v1: int, v2: int, v3: int) -> int:
    return v1 | v2 << 4 | v3 << 8


def build_byte(first_half: int, second_half: int) -> int:
    return (first_half << 4) | second_half


def split_content_into_sectors(content: bytes, sector_size: int) -> list[bytes]:
    result = []
    clusters_cnt: int = required_clusters_count(cluster_size=sector_size, content=content)

    for i in range(clusters_cnt):
        result.append(content[sector_size * i : (i + 1) * sector_size])
    return result


@dataclass
class PartitionGeneratorArgs:
    input_directory: str
    output_file: str
    partition_size: int
    sector_size: int
    sectors_per_cluster: int
    root_entry_count: int
    long_name_support: bool
    use_default_datetime: bool
    fat_type: int | None
    fat_count: int
    wl_mode: str | None


def _normalize_partition_size(partition_size: str | int, wl: bool) -> int:
    if partition_size == 'detect' and not wl:
        return -1
    return int(str(partition_size), 0)


def _partition_generator_args(  # type: ignore[no-untyped-def]
    input_directory,
    output_file,
    partition_size,
    sector_size,
    sectors_per_cluster,
    root_entry_count,
    long_name_support,
    use_default_datetime,
    fat_type,
    fat_count,
    wl_mode,
    wl: bool,
) -> PartitionGeneratorArgs:
    if not os.path.isdir(input_directory):
        log.die(f'The target directory `{input_directory}` does not exist!')
    if wl_mode is not None and sector_size != 512:
        log.die('Wear levelling mode can be set only for sector size 512')
    return PartitionGeneratorArgs(
        input_directory=input_directory,
        output_file=output_file,
        partition_size=_normalize_partition_size(partition_size, wl),
        sector_size=sector_size,
        sectors_per_cluster=sectors_per_cluster,
        root_entry_count=int(root_entry_count),
        long_name_support=long_name_support,
        use_default_datetime=use_default_datetime,
        fat_type=None if fat_type == 0 else fat_type,
        fat_count=fat_count,
        wl_mode=wl_mode,
    )


def _build_partition_generator_cli(desc: str, wl: bool) -> Any:
    import rich_click as click
    from esp_pylib.cli_types import AnyIntType

    sector_choices = ALLOWED_WL_SECTOR_SIZES if wl else ALLOWED_SECTOR_SIZES
    partition_size_help = 'Size of the partition in bytes.'
    if not wl:
        partition_size_help += ' Use `--partition_size detect` for detecting the minimal partition size.'

    @click.command(
        context_settings={'help_option_names': ['-h', '--help']},
        help=desc,
    )
    @click.argument('input_directory', type=click.Path(exists=False, file_okay=False))
    @click.option(
        '--output_file',
        default='fatfs_image.img',
        show_default=True,
        help='Filename of the generated fatfs image',
    )
    @click.option(
        '--partition_size',
        default=str(FATDefaults.SIZE),
        show_default=True,
        help=partition_size_help,
    )
    @click.option(
        '--sector_size',
        type=click.Choice([str(s) for s in sector_choices], case_sensitive=False),
        default=str(FATDefaults.SECTOR_SIZE),
        show_default=True,
        help='Size of the partition sector in bytes',
    )
    @click.option(
        '--sectors_per_cluster',
        type=click.Choice([str(s) for s in ALLOWED_SECTORS_PER_CLUSTER], case_sensitive=False),
        default='1',
        show_default=True,
        help='Number of sectors per cluster',
    )
    @click.option(
        '--root_entry_count',
        default=str(FATDefaults.ROOT_ENTRIES_COUNT),
        show_default=True,
        type=AnyIntType(),
        help='Number of entries in the root directory',
    )
    @click.option(
        '--long_name_support',
        is_flag=True,
        default=False,
        help='Set flag to enable long names support.',
    )
    @click.option(
        '--use_default_datetime',
        is_flag=True,
        default=False,
        help='For test purposes. If the flag is set the files are created with '
        'the default timestamp that is the 1st of January 1980',
    )
    @click.option(
        '--fat_type',
        type=click.Choice(['12', '16', '0'], case_sensitive=False),
        default='0',
        show_default=True,
        help='Type of the FAT file-system. Select 12 for FAT12, 16 for FAT16. '
        'Leave unset or select 0 for automatic file-system type detection.',
    )
    @click.option(
        '--fat_count',
        type=click.Choice(['1', '2'], case_sensitive=False),
        default=str(FATDefaults.FAT_TABLES_COUNT),
        show_default=True,
        help='Number of file allocation tables (FATs) in the filesystem.',
    )
    @click.option(
        '--wl_mode',
        type=click.Choice(['safe', 'perf'], case_sensitive=False),
        default=None,
        help='Wear levelling mode to use. Safe or performance. Only for sector size of 512',
    )
    def cli(  # type: ignore[no-untyped-def]
        input_directory,
        output_file,
        partition_size,
        sector_size,
        sectors_per_cluster,
        root_entry_count,
        long_name_support,
        use_default_datetime,
        fat_type,
        fat_count,
        wl_mode,
    ):
        return _partition_generator_args(
            input_directory=input_directory,
            output_file=output_file,
            partition_size=partition_size,
            sector_size=int(sector_size),
            sectors_per_cluster=int(sectors_per_cluster),
            root_entry_count=root_entry_count,
            long_name_support=long_name_support,
            use_default_datetime=use_default_datetime,
            fat_type=int(fat_type),
            fat_count=int(fat_count),
            wl_mode=wl_mode,
            wl=wl,
        )

    return cli


def get_args_for_partition_generator(desc: str, wl: bool) -> PartitionGeneratorArgs:
    return cast(PartitionGeneratorArgs, _build_partition_generator_cli(desc, wl)(standalone_mode=False))


def read_filesystem(path: str) -> bytearray:
    with open(path, 'rb') as fs_file:
        return bytearray(fs_file.read())


DATE_ENTRY = BitStruct('year' / BitsInteger(7), 'month' / BitsInteger(4), 'day' / BitsInteger(5))

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
    return int.from_bytes(
        TIME_ENTRY.build(dict(hour=hour, minute=minute, second=sec // FATFS_SECONDS_GRANULARITY)), byteorder='big'
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
    NUM_HEADS: int = 0xFF
    OEM_NAME: str = 'MSDOS5.0'
    SEC_PER_TRACK: int = 0x3F
    VOLUME_LABEL: str = 'Espressif'
    FILE_SYS_TYPE: str = 'FAT'
    ROOT_ENTRIES_COUNT: int = 512  # number of entries in the root directory, recommended 512
    MEDIA_TYPE: int = 0xF8
    SIGNATURE_WORD: bytes = b'\x55\xaa'

    # wear levelling defaults
    VERSION: int = 2
    TEMP_BUFFER_SIZE: int = 32
    UPDATE_RATE: int = 16
    WR_SIZE: int = 16
    # wear leveling metadata (config sector) contains always sector size 4096
    WL_SECTOR_SIZE: int = 4096
