# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from construct import Const, Int8ul, Int16ul, Int32ul, PaddedString, Struct

from .utils import (BYTES_PER_DIRECTORY_ENTRY, get_fatfs_type, get_non_data_sectors_cnt, number_of_clusters,
                    read_filesystem)


class FATFSParser:
    MAX_VOL_LAB_SIZE = 11
    MAX_OEM_NAME_SIZE = 8
    MAX_FS_TYPE_SIZE = 8

    # the FAT specification defines 512 bytes for the boot sector header
    BOOT_HEADER_SIZE = 512

    BOOT_SECTOR_HEADER = Struct(
        'BS_jmpBoot' / Const(b'\xeb\xfe\x90'),
        'BS_OEMName' / PaddedString(MAX_OEM_NAME_SIZE, 'utf-8'),
        'BPB_BytsPerSec' / Int16ul,
        'BPB_SecPerClus' / Int8ul,
        'BPB_RsvdSecCnt' / Int16ul,
        'BPB_NumFATs' / Int8ul,
        'BPB_RootEntCnt' / Int16ul,
        'BPB_TotSec16' / Int16ul,
        'BPB_Media' / Int8ul,
        'BPB_FATSz16' / Int16ul,
        'BPB_SecPerTrk' / Int16ul,
        'BPB_NumHeads' / Int16ul,
        'BPB_HiddSec' / Int32ul,
        'BPB_TotSec32' / Int32ul,
        'BS_DrvNum' / Const(b'\x80'),
        'BS_Reserved1' / Const(b'\x00'),
        'BS_BootSig' / Const(b'\x29'),
        'BS_VolID' / Int32ul,
        'BS_VolLab' / PaddedString(MAX_VOL_LAB_SIZE, 'utf-8'),
        'BS_FilSysType' / PaddedString(MAX_FS_TYPE_SIZE, 'utf-8'),
        'BS_EMPTY' / Const(448 * b'\x00'),
        'Signature_word' / Const(b'\x55\xAA')
    )
    assert BOOT_SECTOR_HEADER.sizeof() == BOOT_HEADER_SIZE

    def __init__(self, image_file_path: str, wl_support: bool = False) -> None:
        if wl_support:
            raise NotImplementedError('Parser is not implemented for WL yet.')
        self.fatfs = read_filesystem(image_file_path)

        # when wl is not supported we expect boot sector to be the first
        self.parsed_header = FATFSParser.BOOT_SECTOR_HEADER.parse(self.fatfs[:FATFSParser.BOOT_HEADER_SIZE])

    def print(self) -> None:
        print('Properties of the FATFS:')
        for key in self.parsed_header.keys():
            if key in ('_io', 'BS_EMPTY', 'Signature_word'):
                continue
            print('  {}: {}'.format(key.replace('BPB_', '').replace('BS_', ''), self.parsed_header[key]))
        root_dir_cnt = (self.parsed_header['BPB_RootEntCnt'] * BYTES_PER_DIRECTORY_ENTRY) // self.parsed_header[
            'BPB_BytsPerSec']
        non_data_sectors = get_non_data_sectors_cnt(self.parsed_header['BPB_RsvdSecCnt'],
                                                    # this has to be changed when FAT32 is supported
                                                    self.parsed_header['BPB_FATSz16'], root_dir_cnt)
        data_clusters = self.parsed_header['BPB_TotSec16'] - non_data_sectors
        clusters_num = number_of_clusters(data_clusters, self.parsed_header['BPB_SecPerClus'])
        assert self.parsed_header['BPB_BytsPerSec'] in (512, 1024, 2048, 4096)
        assert self.parsed_header['BPB_SecPerClus'] in (1, 2, 4, 8, 16, 32, 64, 128)
        print(f'  Clusters number: {clusters_num}')
        print(f'  FATFS type: FAT{get_fatfs_type(clusters_num)}')
