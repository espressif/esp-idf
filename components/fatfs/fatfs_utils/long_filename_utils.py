# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from typing import List

from .entry import Entry
from .exceptions import NoFreeClusterException
from .utils import build_name, convert_to_utf16_and_pad

#  File name with long filenames support can be as long as memory allows. It is split into entries
#  holding 13 characters of the filename, thus the number of required entries is ceil(len(long_name) / 13).
#  This is computed using `get_required_lfn_entries_count`.
#  For creating long name entries we need to split the name by 13 characters using `split_name_to_lfn_entries`
#  and in every entry into three blocks with sizes 5, 6 and 2 characters using `split_name_to_lfn_entry`.

MAXIMAL_FILES_SAME_PREFIX: int = 127


def get_required_lfn_entries_count(lfn_full_name: str) -> int:
    """
    Compute the number of entries required to store the long name.
    One long filename entry can hold 13 characters with size 2 bytes.

    E.g. "thisisverylongfilenama.txt" with length of 26 needs 2 lfn entries,
    but "thisisverylongfilenamax.txt" with 27 characters needs 3 lfn entries.
    """
    entries_count: int = (len(lfn_full_name) + Entry.CHARS_PER_ENTRY - 1) // Entry.CHARS_PER_ENTRY
    return entries_count


def split_name_to_lfn_entries(name: str, entries: int) -> List[str]:
    """
    If the filename is longer than 8 (name) + 3 (extension) characters,
    generator uses long name structure and splits the name into suitable amount of blocks.

    E.g. 'thisisverylongfilenama.txt' would be split to ['THISISVERYLON', 'GFILENAMA.TXT'],
    in case of 'thisisverylongfilenamax.txt' - ['THISISVERYLON', 'GFILENAMAX.TX', 'T']
    """
    return [name[i * Entry.CHARS_PER_ENTRY:(i + 1) * Entry.CHARS_PER_ENTRY] for i in range(entries)]


def split_name_to_lfn_entry_blocks(name: str) -> List[bytes]:
    """
    Filename is divided into three blocks in every long file name entry. Sizes of the blocks are defined
    by LDIR_Name1_SIZE, LDIR_Name2_SIZE and LDIR_Name3_SIZE, thus every block contains LDIR_Name{X}_SIZE * 2 bytes.

    If the filename ends in one of the blocks, it is terminated by zero encoded to two bytes (0x0000). Other unused
    characters are set to 0xFFFF.
    E.g.:
    'GFILENAMA.TXT' -> [b'G\x00F\x00I\x00L\x00E\x00', b'N\x00A\x00M\x00A\x00.\x00T\x00', b'X\x00T\x00'];
    'T' -> [b'T\x00\x00\x00\xff\xff\xff\xff\xff\xff', b'\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff',
            b'\xff\xff\xff\xff']

    Notice that since every character is coded using 2 bytes be must add 0x00 to ASCII symbols ('G' -> 'G\x00', etc.),
    since character 'T' ends in the first block, we must add '\x00\x00' after 'T\x00'.
    """
    max_entry_size: int = Entry.LDIR_Name1_SIZE + Entry.LDIR_Name2_SIZE + Entry.LDIR_Name2_SIZE
    assert len(name) <= max_entry_size
    blocks_: List[bytes] = [
        convert_to_utf16_and_pad(content=name[:Entry.LDIR_Name1_SIZE],
                                 expected_size=Entry.LDIR_Name1_SIZE),
        convert_to_utf16_and_pad(content=name[Entry.LDIR_Name1_SIZE:Entry.LDIR_Name1_SIZE + Entry.LDIR_Name2_SIZE],
                                 expected_size=Entry.LDIR_Name2_SIZE),
        convert_to_utf16_and_pad(content=name[Entry.LDIR_Name1_SIZE + Entry.LDIR_Name2_SIZE:],
                                 expected_size=Entry.LDIR_Name3_SIZE)
    ]
    return blocks_


def build_lfn_unique_entry_name_order(entities: list, lfn_entry_name: str) -> int:
    """
    The short entry contains only the first 6 characters of the file name,
    and we have to distinguish it from other names within the directory starting with the same 6 characters.
    To make it unique, we add its order in relation to other names such that lfn_entry_name[:6] == other[:6].
    The order is specified by the character, starting with chr(1).

    E.g. the file in directory 'thisisverylongfilenama.txt' will be named 'THISIS~1TXT' in its short entry.
    If we add another file 'thisisverylongfilenamax.txt' its name in the short entry will be 'THISIS~2TXT'.
    """
    preceding_entries: int = 1
    for entity in entities:
        if entity.name[:6] == lfn_entry_name[:6]:
            preceding_entries += 1
    if preceding_entries > MAXIMAL_FILES_SAME_PREFIX:
        raise NoFreeClusterException('Maximal number of files with the same prefix is 127')
    return preceding_entries


def build_lfn_full_name(name: str, extension: str) -> str:
    """
    The extension is optional, and the long filename entry explicitly specifies it,
    on the opposite as for short file names.
    """
    lfn_record: str = build_name(name, extension)
    # the name must be terminated with NULL terminator
    # if it doesn't fit into the set of long name directory entries
    if len(lfn_record) % Entry.CHARS_PER_ENTRY != 0:
        return lfn_record + chr(0)
    return lfn_record
