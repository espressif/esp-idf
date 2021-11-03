# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from typing import Any, Optional

from construct import Const, Int8ul, Int16ul, Int32ul, PaddedString, Struct

from .exceptions import LowerCaseException, TooLongNameException
from .fatfs_state import FATFSState
from .utils import is_valid_fatfs_name, pad_string


class Entry:
    """
    The class Entry represents entry of the directory.
    """
    ATTR_READ_ONLY = 0x01
    ATTR_HIDDEN = 0x02
    ATTR_SYSTEM = 0x04
    ATTR_VOLUME_ID = 0x08
    ATTR_DIRECTORY = 0x10
    ATTR_ARCHIVE = 0x20
    MAX_NAME_SIZE_S = 8
    MAX_EXT_SIZE_S = 3

    ENTRY_FORMAT_SHORT_NAME = Struct(
        'DIR_Name' / PaddedString(MAX_NAME_SIZE_S, 'utf-8'),
        'DIR_Name_ext' / PaddedString(MAX_EXT_SIZE_S, 'utf-8'),
        'DIR_Attr' / Int8ul,
        'DIR_NTRes' / Const(b'\x00'),
        'DIR_CrtTimeTenth' / Const(b'\x00'),
        'DIR_CrtTime' / Const(b'\x01\x00'),
        'DIR_CrtDate' / Const(b'\x21\x00'),
        'DIR_LstAccDate' / Const(b'\x00\x00'),
        'DIR_FstClusHI' / Const(b'\x00\x00'),
        'DIR_WrtTime' / Const(b'\x01\x00'),
        'DIR_WrtDate' / Const(b'\x01\x00'),
        'DIR_FstClusLO' / Int16ul,
        'DIR_FileSize' / Int32ul,
    )

    # IDF-4044
    ENTRY_FORMAT_LONG_NAME = Struct()

    def __init__(self,
                 entry_id: int,
                 parent_dir_entries_address: int,
                 fatfs_state: FATFSState) -> None:
        self.fatfs_state = fatfs_state
        self.id = entry_id
        self.entry_address = parent_dir_entries_address + self.id * self.fatfs_state.entry_size
        self._is_alias = False
        self._is_empty = True

    @property
    def is_empty(self) -> bool:
        return self._is_empty

    def _parse_entry(self, entry_bytearray: Optional[bytearray]) -> dict:
        if self.fatfs_state.long_names_enabled:
            return Entry.ENTRY_FORMAT_LONG_NAME.parse(entry_bytearray)  # type: ignore
        return Entry.ENTRY_FORMAT_SHORT_NAME.parse(entry_bytearray)  # type: ignore

    def _build_entry(self, **kwargs) -> Any:  # type: ignore
        if self.fatfs_state.long_names_enabled:
            return Entry.ENTRY_FORMAT_LONG_NAME.build(dict(**kwargs))
        return Entry.ENTRY_FORMAT_SHORT_NAME.build(dict(**kwargs))

    @property
    def entry_bytes(self) -> Any:
        return self.fatfs_state.binary_image[self.entry_address: self.entry_address + self.fatfs_state.entry_size]

    @entry_bytes.setter
    def entry_bytes(self, value: int) -> None:
        self.fatfs_state.binary_image[self.entry_address: self.entry_address + self.fatfs_state.entry_size] = value

    def _clean_entry(self) -> None:
        self.entry_bytes = self.fatfs_state.entry_size * b'\x00'

    def allocate_entry(self,
                       first_cluster_id: int,
                       entity_name: str,
                       entity_type: int,
                       entity_extension: str = '',
                       size: int = 0) -> None:
        """
        :param first_cluster_id: id of the first data cluster for given entry
        :param entity_name: name recorded in the entry
        :param entity_extension: extension recorded in the entry
        :param size: size of the content of the file
        :param entity_type: type of the entity (file [0x20] or directory [0x10])
        :returns: None

        :raises LowerCaseException: In case when long_names_enabled is set to False and filename exceeds 8 chars
        for name or 3 chars for extension the exception is raised
        """
        if not ((is_valid_fatfs_name(entity_name) and
                 is_valid_fatfs_name(entity_extension)) or
                self.fatfs_state.long_names_enabled):
            raise LowerCaseException('Lower case is not supported because long name support is not enabled!')

        # clean entry before allocation
        self._clean_entry()
        self._is_empty = False
        object_name = entity_name.upper()
        object_extension = entity_extension.upper()

        # implementation of long names support will be part of IDF-4044
        exceeds_short_name = len(object_name) > Entry.MAX_NAME_SIZE_S or len(object_extension) > Entry.MAX_EXT_SIZE_S
        if not self.fatfs_state.long_names_enabled and exceeds_short_name:
            raise TooLongNameException(
                'Maximal length of the object name is 8 characters and 3 characters for extension!')

        start_address = self.entry_address
        end_address = start_address + self.fatfs_state.entry_size
        self.fatfs_state.binary_image[start_address: end_address] = self._build_entry(
            DIR_Name=pad_string(object_name, size=Entry.MAX_NAME_SIZE_S),
            DIR_Name_ext=pad_string(object_extension, size=Entry.MAX_EXT_SIZE_S),
            DIR_Attr=entity_type,
            DIR_FstClusLO=first_cluster_id,
            DIR_FileSize=size
        )

    def update_content_size(self, content_size: int) -> None:
        parsed_entry = self._parse_entry(self.entry_bytes)
        parsed_entry.DIR_FileSize = content_size  # type: ignore
        self.entry_bytes = Entry.ENTRY_FORMAT_SHORT_NAME.build(parsed_entry)
