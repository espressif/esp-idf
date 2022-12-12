# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from inspect import getmembers, isroutine
from typing import Optional

from construct import Const, Int8ul, Int16ul, Int32ul, PaddedString, Struct, core

from .exceptions import InconsistentFATAttributes, NotInitialized
from .fatfs_state import BootSectorState
from .utils import (ALLOWED_SECTOR_SIZES, ALLOWED_SECTORS_PER_CLUSTER, EMPTY_BYTE, FAT32, FULL_BYTE,
                    SHORT_NAMES_ENCODING, FATDefaults, generate_4bytes_random, pad_string)


class BootSector:
    """
    This class describes the first sector of the volume in the Reserved Region.
    It contains data from BPB (BIOS Parameter Block) and BS (Boot sector). The fields of the BPB and BS are mixed in
    the header of the physical boot sector. Fields with prefix BPB belongs to BPB block and with prefix BS
    belongs to the actual boot sector.

    Please beware, that the name of class BootSector refer to data both from the boot sector and BPB.
    ESP32 ignores fields with prefix "BS_"! Fields with prefix BPB_ are essential to read the filesystem.
    """
    MAX_VOL_LAB_SIZE = 11
    MAX_OEM_NAME_SIZE = 8
    MAX_FS_TYPE_SIZE = 8

    # the FAT specification defines 512 bytes for the boot sector header
    BOOT_HEADER_SIZE = 512

    BOOT_SECTOR_HEADER = Struct(
        # this value reflects BS_jmpBoot used for ESP32 boot sector (any other accepted)
        'BS_jmpBoot' / Const(b'\xeb\xfe\x90'),
        'BS_OEMName' / PaddedString(MAX_OEM_NAME_SIZE, SHORT_NAMES_ENCODING),
        'BPB_BytsPerSec' / Int16ul,
        'BPB_SecPerClus' / Int8ul,
        'BPB_RsvdSecCnt' / Int16ul,
        'BPB_NumFATs' / Int8ul,
        'BPB_RootEntCnt' / Int16ul,
        'BPB_TotSec16' / Int16ul,  # zero if the FAT type is 32, otherwise number of sectors
        'BPB_Media' / Int8ul,
        'BPB_FATSz16' / Int16ul,  # for FAT32 always zero, for FAT12/FAT16 number of sectors per FAT
        'BPB_SecPerTrk' / Int16ul,
        'BPB_NumHeads' / Int16ul,
        'BPB_HiddSec' / Int32ul,
        'BPB_TotSec32' / Int32ul,  # zero if the FAT type is 12/16, otherwise number of sectors
        'BS_DrvNum' / Const(b'\x80'),
        'BS_Reserved1' / Const(EMPTY_BYTE),
        'BS_BootSig' / Const(b'\x29'),
        'BS_VolID' / Int32ul,
        'BS_VolLab' / PaddedString(MAX_VOL_LAB_SIZE, SHORT_NAMES_ENCODING),
        'BS_FilSysType' / PaddedString(MAX_FS_TYPE_SIZE, SHORT_NAMES_ENCODING),
        'BS_EMPTY' / Const(448 * EMPTY_BYTE),
        'Signature_word' / Const(FATDefaults.SIGNATURE_WORD)
    )
    assert BOOT_SECTOR_HEADER.sizeof() == BOOT_HEADER_SIZE

    def __init__(self, boot_sector_state: Optional[BootSectorState] = None) -> None:
        self._parsed_header: dict = {}
        self.boot_sector_state: BootSectorState = boot_sector_state

    def generate_boot_sector(self) -> None:
        boot_sector_state: BootSectorState = self.boot_sector_state
        if boot_sector_state is None:
            raise NotInitialized('The BootSectorState instance is not initialized!')
        volume_uuid = generate_4bytes_random()
        pad_header: bytes = (boot_sector_state.sector_size - BootSector.BOOT_HEADER_SIZE) * EMPTY_BYTE
        data_content: bytes = boot_sector_state.data_sectors * boot_sector_state.sector_size * FULL_BYTE
        root_dir_content: bytes = boot_sector_state.root_dir_sectors_cnt * boot_sector_state.sector_size * EMPTY_BYTE
        fat_tables_content: bytes = (boot_sector_state.sectors_per_fat_cnt
                                     * boot_sector_state.fat_tables_cnt
                                     * boot_sector_state.sector_size
                                     * EMPTY_BYTE)
        self.boot_sector_state.binary_image = (
            BootSector.BOOT_SECTOR_HEADER.build(
                dict(BS_OEMName=pad_string(boot_sector_state.oem_name, size=BootSector.MAX_OEM_NAME_SIZE),
                     BPB_BytsPerSec=boot_sector_state.sector_size,
                     BPB_SecPerClus=boot_sector_state.sectors_per_cluster,
                     BPB_RsvdSecCnt=boot_sector_state.reserved_sectors_cnt,
                     BPB_NumFATs=boot_sector_state.fat_tables_cnt,
                     BPB_RootEntCnt=boot_sector_state.entries_root_count,
                     # if fat type is 12 or 16 BPB_TotSec16 is filled and BPB_TotSec32 is 0x00 and vice versa
                     BPB_TotSec16=0x00 if boot_sector_state.fatfs_type == FAT32 else boot_sector_state.sectors_count,
                     BPB_Media=boot_sector_state.media_type,
                     BPB_FATSz16=boot_sector_state.sectors_per_fat_cnt,
                     BPB_SecPerTrk=boot_sector_state.sec_per_track,
                     BPB_NumHeads=boot_sector_state.num_heads,
                     BPB_HiddSec=boot_sector_state.hidden_sectors,
                     BPB_TotSec32=boot_sector_state.sectors_count if boot_sector_state.fatfs_type == FAT32 else 0x00,
                     BS_VolID=volume_uuid,
                     BS_VolLab=pad_string(boot_sector_state.volume_label,
                                          size=BootSector.MAX_VOL_LAB_SIZE),
                     BS_FilSysType=pad_string(boot_sector_state.file_sys_type,
                                              size=BootSector.MAX_FS_TYPE_SIZE)
                     )
            ) + pad_header + fat_tables_content + root_dir_content + data_content
        )

    def parse_boot_sector(self, binary_data: bytes) -> None:
        """
        Checks the validity of the boot sector and derives the metadata from boot sector to the structured shape.
        """
        try:
            self._parsed_header = BootSector.BOOT_SECTOR_HEADER.parse(binary_data)
        except core.StreamError:
            raise NotInitialized('The boot sector header is not parsed successfully!')

        if self._parsed_header['BPB_TotSec16'] != 0x00:
            sectors_count_: int = self._parsed_header['BPB_TotSec16']
        elif self._parsed_header['BPB_TotSec32'] != 0x00:
            # uncomment for FAT32 implementation
            # sectors_count_ = self._parsed_header['BPB_TotSec32']
            # possible_fat_types = [FAT32]
            assert self._parsed_header['BPB_TotSec16'] == 0
            raise NotImplementedError('FAT32 not implemented!')
        else:
            raise InconsistentFATAttributes('The number of FS sectors cannot be zero!')

        if self._parsed_header['BPB_BytsPerSec'] not in ALLOWED_SECTOR_SIZES:
            raise InconsistentFATAttributes(f'The number of bytes '
                                            f"per sector is {self._parsed_header['BPB_BytsPerSec']}! "
                                            f'The accepted values are {ALLOWED_SECTOR_SIZES}')
        if self._parsed_header['BPB_SecPerClus'] not in ALLOWED_SECTORS_PER_CLUSTER:
            raise InconsistentFATAttributes(f'The number of sectors per cluster '
                                            f"is {self._parsed_header['BPB_SecPerClus']}"
                                            f'The accepted values are {ALLOWED_SECTORS_PER_CLUSTER}')

        total_root_bytes: int = self._parsed_header['BPB_RootEntCnt'] * FATDefaults.ENTRY_SIZE
        root_dir_sectors_cnt_: int = total_root_bytes // self._parsed_header['BPB_BytsPerSec']
        self.boot_sector_state = BootSectorState(oem_name=self._parsed_header['BS_OEMName'],
                                                 sector_size=self._parsed_header['BPB_BytsPerSec'],
                                                 sectors_per_cluster=self._parsed_header['BPB_SecPerClus'],
                                                 reserved_sectors_cnt=self._parsed_header['BPB_RsvdSecCnt'],
                                                 fat_tables_cnt=self._parsed_header['BPB_NumFATs'],
                                                 root_dir_sectors_cnt=root_dir_sectors_cnt_,
                                                 sectors_count=sectors_count_,
                                                 media_type=self._parsed_header['BPB_Media'],
                                                 sec_per_track=self._parsed_header['BPB_SecPerTrk'],
                                                 num_heads=self._parsed_header['BPB_NumHeads'],
                                                 hidden_sectors=self._parsed_header['BPB_HiddSec'],
                                                 volume_label=self._parsed_header['BS_VolLab'],
                                                 file_sys_type=self._parsed_header['BS_FilSysType'],
                                                 volume_uuid=self._parsed_header['BS_VolID'])
        self.boot_sector_state.binary_image = binary_data
        assert self.boot_sector_state.file_sys_type in (f'FAT{self.boot_sector_state.fatfs_type}   ', 'FAT     ')

    def __str__(self) -> str:
        """
        FATFS properties parser (internal helper tool for fatfsgen.py/fatfsparse.py)
        Provides all the properties of given FATFS instance by parsing its boot sector (returns formatted string)
        """

        if self._parsed_header == {}:
            return 'Boot sector is not initialized!'
        res: str = 'FATFS properties:\n'
        for member in getmembers(self.boot_sector_state, lambda a: not (isroutine(a))):
            prop_ = getattr(self.boot_sector_state, member[0])
            if isinstance(prop_, int) or isinstance(prop_, str) and not member[0].startswith('_'):
                res += f'{member[0]}: {prop_}\n'
        return res

    @property
    def binary_image(self) -> bytes:
        # when BootSector is not instantiated, self.boot_sector_state might be None
        if self.boot_sector_state is None or len(self.boot_sector_state.binary_image) == 0:
            raise NotInitialized('Boot sector is not initialized!')
        bin_image_: bytes = self.boot_sector_state.binary_image
        return bin_image_
