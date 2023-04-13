# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from typing import List, Optional, Union

from construct import Const, Int8ul, Int16ul, Int32ul, PaddedString, Struct

from .exceptions import LowerCaseException, TooLongNameException
from .fatfs_state import FATFSState
from .utils import (DATETIME, EMPTY_BYTE, FATFS_INCEPTION, MAX_EXT_SIZE, MAX_NAME_SIZE, SHORT_NAMES_ENCODING,
                    FATDefaults, build_date_entry, build_time_entry, is_valid_fatfs_name, pad_string)


class Entry:
    """
    The class Entry represents entry of the directory.
    """
    ATTR_READ_ONLY: int = 0x01
    ATTR_HIDDEN: int = 0x02
    ATTR_SYSTEM: int = 0x04
    ATTR_VOLUME_ID: int = 0x08
    ATTR_DIRECTORY: int = 0x10  # directory
    ATTR_ARCHIVE: int = 0x20  # file
    ATTR_LONG_NAME: int = ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID

    # indexes in the entry structure and sizes in bytes, not in characters (encoded using 2 bytes for lfn)
    LDIR_Name1_IDX: int = 1
    LDIR_Name1_SIZE: int = 5
    LDIR_Name2_IDX: int = 14
    LDIR_Name2_SIZE: int = 6
    LDIR_Name3_IDX: int = 28
    LDIR_Name3_SIZE: int = 2

    # short entry in long file names
    LDIR_DIR_NTRES: int = 0x18
    # one entry can hold 13 characters with size 2 bytes distributed in three regions of the 32 bytes entry
    CHARS_PER_ENTRY: int = LDIR_Name1_SIZE + LDIR_Name2_SIZE + LDIR_Name3_SIZE

    # the last 16 bytes record in the LFN entry has first byte masked with the following value
    LAST_RECORD_LFN_ENTRY: int = 0x40
    SHORT_ENTRY: int = -1
    # this value is used for short-like entry but with accepted lower case
    SHORT_ENTRY_LN: int = 0

    # The 1st January 1980 00:00:00
    DEFAULT_DATE: DATETIME = (FATFS_INCEPTION.year, FATFS_INCEPTION.month, FATFS_INCEPTION.day)
    DEFAULT_TIME: DATETIME = (FATFS_INCEPTION.hour, FATFS_INCEPTION.minute, FATFS_INCEPTION.second)

    ENTRY_FORMAT_SHORT_NAME = Struct(
        'DIR_Name' / PaddedString(MAX_NAME_SIZE, SHORT_NAMES_ENCODING),
        'DIR_Name_ext' / PaddedString(MAX_EXT_SIZE, SHORT_NAMES_ENCODING),
        'DIR_Attr' / Int8ul,
        'DIR_NTRes' / Int8ul,  # this tagged for lfn (0x00 for short entry in lfn, 0x18 for short name)
        'DIR_CrtTimeTenth' / Const(EMPTY_BYTE),  # ignored by esp-idf fatfs library
        'DIR_CrtTime' / Int16ul,  # ignored by esp-idf fatfs library
        'DIR_CrtDate' / Int16ul,  # ignored by esp-idf fatfs library
        'DIR_LstAccDate' / Int16ul,  # must be same as DIR_WrtDate
        'DIR_FstClusHI' / Const(2 * EMPTY_BYTE),
        'DIR_WrtTime' / Int16ul,
        'DIR_WrtDate' / Int16ul,
        'DIR_FstClusLO' / Int16ul,
        'DIR_FileSize' / Int32ul,
    )

    def __init__(self,
                 entry_id: int,
                 parent_dir_entries_address: int,
                 fatfs_state: FATFSState) -> None:
        self.fatfs_state: FATFSState = fatfs_state
        self.id: int = entry_id
        self.entry_address: int = parent_dir_entries_address + self.id * FATDefaults.ENTRY_SIZE
        self._is_alias: bool = False
        self._is_empty: bool = True

    @staticmethod
    def get_cluster_id(obj_: dict) -> int:
        cluster_id_: int = obj_['DIR_FstClusLO']
        return cluster_id_

    @property
    def is_empty(self) -> bool:
        return self._is_empty

    @staticmethod
    def _parse_entry(entry_bytearray: Union[bytearray, bytes]) -> dict:
        entry_: dict = Entry.ENTRY_FORMAT_SHORT_NAME.parse(entry_bytearray)
        return entry_

    @staticmethod
    def _build_entry(**kwargs) -> bytes:  # type: ignore
        entry_: bytes = Entry.ENTRY_FORMAT_SHORT_NAME.build(dict(**kwargs))
        return entry_

    @staticmethod
    def _build_entry_long(names: List[bytes], checksum: int, order: int, is_last: bool) -> bytes:
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
        order |= (Entry.LAST_RECORD_LFN_ENTRY if is_last else 0x00)
        long_entry: bytes = (Int8ul.build(order) +  # order of the long name entry (possibly masked with 0x40)
                             names[0] +  # first 5 characters (10 bytes) of the name part
                             Int8ul.build(Entry.ATTR_LONG_NAME) +  # one byte entity type ATTR_LONG_NAME
                             Int8ul.build(0) +  # one byte of zeros
                             Int8ul.build(checksum) +  # lfn_checksum defined in utils.py
                             names[1] +  # next 6 characters (12 bytes) of the name part
                             Int16ul.build(0) +  # 2 bytes of zeros
                             names[2])  # last 2 characters (4 bytes) of the name part
        return long_entry

    @staticmethod
    def parse_entry_long(entry_bytes_: bytes, my_check: int) -> dict:
        order_ = Int8ul.parse(entry_bytes_[0:1])
        names0 = entry_bytes_[1:11]
        if Int8ul.parse(entry_bytes_[12:13]) != 0 or Int16ul.parse(entry_bytes_[26:28]) != 0 or Int8ul.parse(entry_bytes_[11:12]) != 15:
            return {}
        if Int8ul.parse(entry_bytes_[13:14]) != my_check:
            return {}
        names1 = entry_bytes_[14:26]
        names2 = entry_bytes_[28:32]
        return {
            'order': order_,
            'name1': names0,
            'name2': names1,
            'name3': names2,
            'is_last': bool(order_ & Entry.LAST_RECORD_LFN_ENTRY == Entry.LAST_RECORD_LFN_ENTRY)
        }

    @property
    def entry_bytes(self) -> bytes:
        """
        :returns: Bytes defining the entry belonging to the given instance.
        """
        start_: int = self.entry_address
        entry_: bytes = self.fatfs_state.binary_image[start_: start_ + FATDefaults.ENTRY_SIZE]
        return entry_

    @entry_bytes.setter
    def entry_bytes(self, value: bytes) -> None:
        """
        :param value: new content of the entry
        :returns: None

        The setter sets the content of the entry in bytes.
        """
        self.fatfs_state.binary_image[self.entry_address: self.entry_address + FATDefaults.ENTRY_SIZE] = value

    def _clean_entry(self) -> None:
        self.entry_bytes: bytes = FATDefaults.ENTRY_SIZE * EMPTY_BYTE

    def allocate_entry(self,
                       first_cluster_id: int,
                       entity_name: str,
                       entity_type: int,
                       entity_extension: str = '',
                       size: int = 0,
                       date: DATETIME = DEFAULT_DATE,
                       time: DATETIME = DEFAULT_TIME,
                       lfn_order: int = SHORT_ENTRY,
                       lfn_names: Optional[List[bytes]] = None,
                       lfn_checksum_: int = 0,
                       fits_short: bool = False,
                       lfn_is_last: bool = False) -> None:
        """
        :param first_cluster_id: id of the first data cluster for given entry
        :param entity_name: name recorded in the entry
        :param entity_extension: extension recorded in the entry
        :param size: size of the content of the file
        :param date: denotes year (actual year minus 1980), month number day of the month (minimal valid is (0, 1, 1))
        :param time: denotes hour, minute and second with granularity 2 seconds (sec // 2)
        :param entity_type: type of the entity (file [0x20] or directory [0x10])
        :param lfn_order: if long names support is enabled, defines order in long names entries sequence (-1 for short)
        :param lfn_names: if the entry is dedicated for long names the lfn_names contains
            LDIR_Name1, LDIR_Name2 and LDIR_Name3 in this order
        :param lfn_checksum_: use only for long file names, checksum calculated lfn_checksum function
        :param fits_short: determines if the name fits in 8.3 filename
        :param lfn_is_last: determines if the long file name entry is holds last part of the name,
            thus its address is first in the physical order
        :returns: None

        :raises LowerCaseException: In case when long_names_enabled is set to False and filename exceeds 8 chars
        for name or 3 chars for extension the exception is raised
        :raises TooLongNameException: When long_names_enabled is set to False and name doesn't fit to 8.3 filename
        an exception is raised
        """
        valid_full_name: bool = is_valid_fatfs_name(entity_name) and is_valid_fatfs_name(entity_extension)
        if not (valid_full_name or lfn_order >= 0):
            raise LowerCaseException('Lower case is not supported in short name entry, use upper case.')

        if self.fatfs_state.use_default_datetime:
            date = self.DEFAULT_DATE
            time = self.DEFAULT_TIME

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
                )
            )

        start_address = self.entry_address
        end_address = start_address + FATDefaults.ENTRY_SIZE
        if lfn_order in (self.SHORT_ENTRY, self.SHORT_ENTRY_LN):
            date_entry_: int = build_date_entry(*date)
            time_entry: int = build_time_entry(*time)
            self.fatfs_state.binary_image[start_address: end_address] = self._build_entry(
                DIR_Name=pad_string(object_name, size=MAX_NAME_SIZE),
                DIR_Name_ext=pad_string(object_extension, size=MAX_EXT_SIZE),
                DIR_Attr=entity_type,
                DIR_NTRes=0x00 if (not self.fatfs_state.long_names_enabled) or (not fits_short) else 0x18,
                DIR_FstClusLO=first_cluster_id,
                DIR_FileSize=size,
                DIR_CrtDate=date_entry_,  # ignored by esp-idf fatfs library
                DIR_LstAccDate=date_entry_,  # must be same as DIR_WrtDate
                DIR_WrtDate=date_entry_,
                DIR_CrtTime=time_entry,  # ignored by esp-idf fatfs library
                DIR_WrtTime=time_entry
            )
        else:
            assert lfn_names is not None
            self.fatfs_state.binary_image[start_address: end_address] = self._build_entry_long(lfn_names,
                                                                                               lfn_checksum_,
                                                                                               lfn_order,
                                                                                               lfn_is_last)

    def update_content_size(self, content_size: int) -> None:
        """
        :param content_size: the new size of the file content in bytes
        :returns: None

        This method parses the binary entry to the construct structure, updates the content size of the file
        and builds new binary entry.
        """
        parsed_entry = self._parse_entry(self.entry_bytes)
        parsed_entry.DIR_FileSize = content_size  # type: ignore
        self.entry_bytes = Entry.ENTRY_FORMAT_SHORT_NAME.build(parsed_entry)
