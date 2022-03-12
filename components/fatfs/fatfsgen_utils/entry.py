# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from typing import Any, List, Optional

from construct import Const, Int8ul, Int16ul, Int32ul, PaddedString, Struct

from .exceptions import LowerCaseException, TooLongNameException
from .fatfs_state import FATFSState
from .utils import MAX_EXT_SIZE, MAX_NAME_SIZE, is_valid_fatfs_name, pad_string


class Entry:
    """
    The class Entry represents entry of the directory.
    """
    ATTR_READ_ONLY: int = 0x01
    ATTR_HIDDEN: int = 0x02
    ATTR_SYSTEM: int = 0x04
    ATTR_VOLUME_ID: int = 0x08
    ATTR_DIRECTORY: int = 0x10
    ATTR_ARCHIVE: int = 0x20
    ATTR_LONG_NAME: int = ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID

    # indexes in the entry structure and sizes in bytes, not in characters (encoded using 2 bytes for lfn)
    LDIR_Name1_IDX: int = 1
    LDIR_Name1_SIZE: int = 5
    LDIR_Name2_IDX: int = 14
    LDIR_Name2_SIZE: int = 6
    LDIR_Name3_IDX: int = 28
    LDIR_Name3_SIZE: int = 2

    # one entry can hold 13 characters with size 2 bytes distributed in three regions of the 32 bytes entry
    CHARS_PER_ENTRY: int = LDIR_Name1_SIZE + LDIR_Name2_SIZE + LDIR_Name3_SIZE

    SHORT_ENTRY: int = -1
    SHORT_ENTRY_LN: int = 0

    ENTRY_FORMAT_SHORT_NAME = Struct(
        'DIR_Name' / PaddedString(MAX_NAME_SIZE, 'utf-8'),
        'DIR_Name_ext' / PaddedString(MAX_EXT_SIZE, 'utf-8'),
        'DIR_Attr' / Int8ul,
        'DIR_NTRes' / Const(b'\x00'),
        'DIR_CrtTimeTenth' / Const(b'\x00'),
        'DIR_CrtTime' / Const(b'\x00\x00'),
        'DIR_CrtDate' / Const(b'\x21\x00'),
        'DIR_LstAccDate' / Const(b'\x00\x00'),
        'DIR_FstClusHI' / Const(b'\x00\x00'),
        'DIR_WrtTime' / Const(b'\x00\x00'),
        'DIR_WrtDate' / Const(b'\x21\x00'),
        'DIR_FstClusLO' / Int16ul,
        'DIR_FileSize' / Int32ul,
    )

    def __init__(self,
                 entry_id: int,
                 parent_dir_entries_address: int,
                 fatfs_state: FATFSState) -> None:
        self.fatfs_state: FATFSState = fatfs_state
        self.id: int = entry_id
        self.entry_address: int = parent_dir_entries_address + self.id * self.fatfs_state.entry_size
        self._is_alias: bool = False
        self._is_empty: bool = True

    @property
    def is_empty(self) -> bool:
        return self._is_empty

    @staticmethod
    def _parse_entry(entry_bytearray: Optional[bytearray]) -> dict:
        return Entry.ENTRY_FORMAT_SHORT_NAME.parse(entry_bytearray)  # type: ignore

    @staticmethod
    def _build_entry(**kwargs) -> Any:  # type: ignore
        return Entry.ENTRY_FORMAT_SHORT_NAME.build(dict(**kwargs))

    @staticmethod
    def _build_entry_long(names: List[bytes], checksum: int, order: int, is_last: bool, entity_type: int) -> bytes:
        """
        Long entry starts with 1 bytes of the order, if the entry is the last in the chain it is or-masked with 0x40,
        otherwise is without change (or masked with 0x00). The following example shows 3 entries:
        first two (0x2000-0x2040) are long in the reverse order and the last one (0x2040-0x2060) is short.
        The entries define file name "thisisverylongfilenama.txt".

        00002000: 42 67 00 66 00 69 00 6C 00 65 00 0F 00 43 6E 00    Bg.f.i.l.e...Cn.
        00002010: 61 00 6D 00 61 00 2E 00 74 00 00 00 78 00 74 00    a.m.a...t...x.t.
        00002020: 01 74 00 68 00 69 00 73 00 69 00 0F 00 43 73 00    .t.h.i.s.i...Cs.
        00002030: 76 00 65 00 72 00 79 00 6C 00 00 00 6F 00 6E 00    v.e.r.y.l...o.n.
        00002040: 54 48 49 53 49 53 7E 31 54 58 54 20 00 00 00 00    THISIS~1TXT.....
        00002050: 21 00 00 00 00 00 00 00 21 00 02 00 15 00 00 00    !.......!.......
        """
        order |= (0x40 if is_last else 0x00)
        long_entry: bytes = (Int8ul.build(order) +  # order of the long name entry (possibly masked with 0x40)
                             names[0] +  # first 5 characters (10 bytes) of the name part
                             Int8ul.build(entity_type) +  # one byte entity type ATTR_LONG_NAME
                             Int8ul.build(0) +  # one byte of zeros
                             Int8ul.build(checksum) +  # lfn_checksum defined in utils.py
                             names[1] +  # next 6 characters (12 bytes) of the name part
                             Int16ul.build(0) +  # 2 bytes of zeros
                             names[2])  # last 2 characters (4 bytes) of the name part
        return long_entry

    @property
    def entry_bytes(self) -> Any:
        return self.fatfs_state.binary_image[self.entry_address: self.entry_address + self.fatfs_state.entry_size]

    @entry_bytes.setter
    def entry_bytes(self, value: int) -> None:
        self.fatfs_state.binary_image[self.entry_address: self.entry_address + self.fatfs_state.entry_size] = value

    def _clean_entry(self) -> None:
        self.entry_bytes: bytes = self.fatfs_state.entry_size * b'\x00'

    def allocate_entry(self,
                       first_cluster_id: int,
                       entity_name: str,
                       entity_type: int,
                       entity_extension: str = '',
                       size: int = 0,
                       lfn_order: int = SHORT_ENTRY,
                       lfn_names: Optional[List[bytes]] = None,
                       lfn_checksum_: int = 0,
                       lfn_is_last: bool = False) -> None:
        """
        :param first_cluster_id: id of the first data cluster for given entry
        :param entity_name: name recorded in the entry
        :param entity_extension: extension recorded in the entry
        :param size: size of the content of the file
        :param entity_type: type of the entity (file [0x20] or directory [0x10])
        :param lfn_order: if long names support is enabled, defines order in long names entries sequence (-1 for short)
        :param lfn_names: if the entry is dedicated for long names the lfn_names contains
            LDIR_Name1, LDIR_Name2 and LDIR_Name3 in this order
        :param lfn_checksum_: use only for long file names, checksum calculated lfn_checksum function
        :param lfn_is_last: determines if the long file name entry is holds last part of the name,
            thus its address is first in the physical order
        :returns: None

        :raises LowerCaseException: In case when long_names_enabled is set to False and filename exceeds 8 chars
        for name or 3 chars for extension the exception is raised
        """
        valid_full_name: bool = is_valid_fatfs_name(entity_name) and is_valid_fatfs_name(entity_extension)
        if not (valid_full_name or lfn_order >= 0):
            raise LowerCaseException('Lower case is not supported in short name entry, use upper case.')

        # clean entry before allocation
        self._clean_entry()
        self._is_empty = False

        object_name = entity_name.upper() if not self.fatfs_state.long_names_enabled else entity_name
        object_extension = entity_extension.upper() if not self.fatfs_state.long_names_enabled else entity_extension

        exceeds_short_name: bool = len(object_name) > MAX_NAME_SIZE or len(object_extension) > MAX_EXT_SIZE
        if not self.fatfs_state.long_names_enabled and exceeds_short_name:
            raise TooLongNameException(
                'Maximal length of the object name is {} characters and {} characters for extension!'.format(
                    MAX_NAME_SIZE, MAX_EXT_SIZE
                ))

        start_address = self.entry_address
        end_address = start_address + self.fatfs_state.entry_size
        if lfn_order in (self.SHORT_ENTRY, self.SHORT_ENTRY_LN):
            self.fatfs_state.binary_image[start_address: end_address] = self._build_entry(
                DIR_Name=pad_string(object_name, size=MAX_NAME_SIZE),
                DIR_Name_ext=pad_string(object_extension, size=MAX_EXT_SIZE),
                DIR_Attr=entity_type,
                DIR_FstClusLO=first_cluster_id,
                DIR_FileSize=size
            )
        else:
            assert lfn_names is not None
            self.fatfs_state.binary_image[start_address: end_address] = self._build_entry_long(lfn_names,
                                                                                               lfn_checksum_,
                                                                                               lfn_order,
                                                                                               lfn_is_last,
                                                                                               self.ATTR_LONG_NAME)

    def update_content_size(self, content_size: int) -> None:
        parsed_entry = self._parse_entry(self.entry_bytes)
        parsed_entry.DIR_FileSize = content_size  # type: ignore
        self.entry_bytes = Entry.ENTRY_FORMAT_SHORT_NAME.build(parsed_entry)
