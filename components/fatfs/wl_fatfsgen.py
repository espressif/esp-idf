#!/usr/bin/env python
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from typing import List, Optional

from construct import Const, Int32ul, Struct
from fatfsgen import FATFS
from fatfsgen_utils.exceptions import WLNotInitialized
from fatfsgen_utils.utils import UINT32_MAX, crc32, generate_4bytes_random, get_args_for_partition_generator


class WLFATFS:
    # pylint: disable=too-many-instance-attributes
    CFG_SECTORS_COUNT = 1
    DUMMY_SECTORS_COUNT = 1
    WL_CONFIG_HEADER_SIZE = 48
    WL_STATE_RECORD_SIZE = 16
    WL_STATE_HEADER_SIZE = 64
    WL_STATE_COPY_COUNT = 2
    WL_SECTOR_SIZE = 0x1000

    WL_STATE_T_DATA = Struct(
        'pos' / Int32ul,
        'max_pos' / Int32ul,
        'move_count' / Int32ul,
        'access_count' / Int32ul,
        'max_count' / Int32ul,
        'block_size' / Int32ul,
        'version' / Int32ul,
        'device_id' / Int32ul,
        'reserved' / Const(28 * b'\x00')
    )

    WL_CONFIG_T_DATA = Struct(
        'start_addr' / Int32ul,
        'full_mem_size' / Int32ul,
        'page_size' / Int32ul,
        'sector_size' / Int32ul,
        'updaterate' / Int32ul,
        'wr_size' / Int32ul,
        'version' / Int32ul,
        'temp_buff_size' / Int32ul
    )
    WL_CONFIG_T_HEADER_SIZE = 48

    def __init__(self,
                 size: int = 1024 * 1024,
                 reserved_sectors_cnt: int = 1,
                 fat_tables_cnt: int = 1,
                 sectors_per_cluster: int = 1,
                 sector_size: int = 0x1000,
                 sectors_per_fat: int = 1,
                 explicit_fat_type: int = None,
                 hidden_sectors: int = 0,
                 long_names_enabled: bool = False,
                 entry_size: int = 32,
                 num_heads: int = 0xff,
                 oem_name: str = 'MSDOS5.0',
                 sec_per_track: int = 0x3f,
                 volume_label: str = 'Espressif',
                 file_sys_type: str = 'FAT',
                 version: int = 2,
                 temp_buff_size: int = 32,
                 update_rate: int = 16,
                 device_id: int = None,
                 root_entry_count: int = 512,
                 media_type: int = 0xf8) -> None:
        if sector_size != WLFATFS.WL_SECTOR_SIZE:
            raise NotImplementedError(f'The only supported sector size is currently {WLFATFS.WL_SECTOR_SIZE}')

        self._initialized = False
        self.sector_size = sector_size
        self._version = version
        self._temp_buff_size = temp_buff_size
        self._device_id = device_id
        self._update_rate = update_rate
        self.partition_size = size
        self.total_sectors = self.partition_size // self.sector_size
        self.wl_state_size = WLFATFS.WL_STATE_HEADER_SIZE + WLFATFS.WL_STATE_RECORD_SIZE * self.total_sectors

        # determine the number of required sectors (roundup to sector size)
        self.wl_state_sectors = (self.wl_state_size + self.sector_size - 1) // self.sector_size

        self.boot_sector_start = self.sector_size  # shift by one "dummy" sector
        self.fat_table_start = self.boot_sector_start + reserved_sectors_cnt * self.sector_size

        wl_sectors = WLFATFS.DUMMY_SECTORS_COUNT + WLFATFS.CFG_SECTORS_COUNT + self.wl_state_sectors * 2
        self.plain_fat_sectors = self.total_sectors - wl_sectors

        self.plain_fatfs = FATFS(
            explicit_fat_type=explicit_fat_type,
            size=self.plain_fat_sectors * self.sector_size,
            reserved_sectors_cnt=reserved_sectors_cnt,
            fat_tables_cnt=fat_tables_cnt,
            sectors_per_cluster=sectors_per_cluster,
            sector_size=sector_size,
            sectors_per_fat=sectors_per_fat,
            root_entry_count=root_entry_count,
            hidden_sectors=hidden_sectors,
            long_names_enabled=long_names_enabled,
            entry_size=entry_size,
            num_heads=num_heads,
            oem_name=oem_name,
            sec_per_track=sec_per_track,
            volume_label=volume_label,
            file_sys_type=file_sys_type,
            media_type=media_type
        )

        self.fatfs_binary_image = self.plain_fatfs.state.binary_image

    def init_wl(self) -> None:
        self.fatfs_binary_image = self.plain_fatfs.state.binary_image
        self._add_dummy_sector()
        # config must be added after state, do not change the order of these two calls!
        self._add_state_sectors()
        self._add_config_sector()
        self._initialized = True

    def _add_dummy_sector(self) -> None:
        self.fatfs_binary_image = self.sector_size * b'\xff' + self.fatfs_binary_image

    def _add_config_sector(self) -> None:
        wl_config_data = WLFATFS.WL_CONFIG_T_DATA.build(
            dict(
                start_addr=0,
                full_mem_size=self.partition_size,
                page_size=self.sector_size,
                sector_size=self.sector_size,
                updaterate=self._update_rate,
                wr_size=16,
                version=self._version,
                temp_buff_size=self._temp_buff_size
            )
        )

        crc = crc32(list(wl_config_data), UINT32_MAX)
        wl_config_crc = Int32ul.build(crc)

        # adding three 4 byte zeros to align the structure
        wl_config = wl_config_data + wl_config_crc + Int32ul.build(0) + Int32ul.build(0) + Int32ul.build(0)

        self.fatfs_binary_image += (wl_config + (self.sector_size - WLFATFS.WL_CONFIG_HEADER_SIZE) * b'\xff')

    def _add_state_sectors(self) -> None:
        wl_state_data = WLFATFS.WL_STATE_T_DATA.build(
            dict(
                pos=0,
                max_pos=self.plain_fat_sectors + WLFATFS.DUMMY_SECTORS_COUNT,
                move_count=0,
                access_count=0,
                max_count=self._update_rate,
                block_size=self.sector_size,
                version=self._version,
                device_id=self._device_id or generate_4bytes_random(),
            )
        )
        crc = crc32(list(wl_state_data), UINT32_MAX)
        wl_state_crc = Int32ul.build(crc)
        wl_state = wl_state_data + wl_state_crc
        self.fatfs_binary_image += WLFATFS.WL_STATE_COPY_COUNT * (
            (wl_state + (self.sector_size - WLFATFS.WL_STATE_HEADER_SIZE) * b'\xff') + (
                self.wl_state_sectors - 1) * self.sector_size * b'\xff')

    def wl_write_filesystem(self, output_path: str) -> None:
        if not self._initialized:
            raise WLNotInitialized('FATFS is not initialized with WL. First call method WLFATFS.init_wl!')
        with open(output_path, 'wb') as output:
            output.write(bytearray(self.fatfs_binary_image))

    def wl_generate(self, input_directory: str) -> None:
        """
        Normalize path to folder and recursively encode folder to binary image
        """
        self.plain_fatfs.generate(input_directory=input_directory)

    def wl_create_file(self, name: str, extension: str = '', path_from_root: Optional[List[str]] = None) -> None:
        self.plain_fatfs.create_file(name, extension, path_from_root)

    def wl_create_directory(self, name: str, path_from_root: Optional[List[str]] = None) -> None:
        self.plain_fatfs.create_directory(name, path_from_root)

    def wl_write_content(self, path_from_root: List[str], content: bytes) -> None:
        self.plain_fatfs.write_content(path_from_root, content)


if __name__ == '__main__':
    desc = 'Create a FAT filesystem with support for wear levelling and populate it with directory content'
    args = get_args_for_partition_generator(desc)

    wl_fatfs = WLFATFS(sector_size=args.sector_size,
                       sectors_per_cluster=args.sectors_per_cluster,
                       size=args.partition_size,
                       root_entry_count=args.root_entry_count,
                       explicit_fat_type=args.fat_type,
                       long_names_enabled=args.long_name_support)

    wl_fatfs.wl_generate(args.input_directory)
    wl_fatfs.init_wl()
    wl_fatfs.wl_write_filesystem(args.output_file)
