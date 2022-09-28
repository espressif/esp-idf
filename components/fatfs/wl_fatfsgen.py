#!/usr/bin/env python
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from construct import Const, Int32ul, Struct
from fatfs_utils.exceptions import WLNotInitialized
from fatfs_utils.utils import (FULL_BYTE, UINT32_MAX, FATDefaults, crc32, generate_4bytes_random,
                               get_args_for_partition_generator)
from fatfsgen import FATFS


def remove_wl(binary_image: bytes) -> bytes:
    partition_size: int = len(binary_image)
    total_sectors: int = partition_size // FATDefaults.WL_SECTOR_SIZE
    wl_state_size: int = WLFATFS.WL_STATE_HEADER_SIZE + WLFATFS.WL_STATE_RECORD_SIZE * total_sectors
    wl_state_sectors_cnt: int = (wl_state_size + FATDefaults.WL_SECTOR_SIZE - 1) // FATDefaults.WL_SECTOR_SIZE
    wl_state_total_size: int = wl_state_sectors_cnt * FATDefaults.WL_SECTOR_SIZE
    wl_sectors_size: int = (wl_state_sectors_cnt
                            * FATDefaults.WL_SECTOR_SIZE
                            * WLFATFS.WL_STATE_COPY_COUNT
                            + FATDefaults.WL_SECTOR_SIZE)

    correct_wl_configuration = binary_image[-wl_sectors_size:]

    data_ = WLFATFS.WL_STATE_T_DATA.parse(correct_wl_configuration[:WLFATFS.WL_STATE_HEADER_SIZE])

    total_records = 0
    # iterating over records field of the first copy of the state sector
    for i in range(WLFATFS.WL_STATE_HEADER_SIZE, wl_state_total_size, WLFATFS.WL_STATE_RECORD_SIZE):
        if correct_wl_configuration[i:i + WLFATFS.WL_STATE_RECORD_SIZE] != WLFATFS.WL_STATE_RECORD_SIZE * b'\xff':
            total_records += 1
        else:
            break
    before_dummy = binary_image[:total_records * FATDefaults.WL_SECTOR_SIZE]
    after_dummy = binary_image[total_records * FATDefaults.WL_SECTOR_SIZE + FATDefaults.WL_SECTOR_SIZE:]
    new_image: bytes = before_dummy + after_dummy

    # remove wl sectors
    new_image = new_image[:len(new_image) - (FATDefaults.WL_SECTOR_SIZE + 2 * wl_state_total_size)]

    # reorder to preserve original order
    new_image = (new_image[-data_['move_count'] * FATDefaults.WL_SECTOR_SIZE:]
                 + new_image[:-data_['move_count'] * FATDefaults.WL_SECTOR_SIZE])
    return new_image


class WLFATFS:
    # pylint: disable=too-many-instance-attributes
    WL_CFG_SECTORS_COUNT = 1
    WL_DUMMY_SECTORS_COUNT = 1
    WL_CONFIG_HEADER_SIZE = 48
    WL_STATE_RECORD_SIZE = 16
    WL_STATE_HEADER_SIZE = 64
    WL_STATE_COPY_COUNT = 2  # always 2 copies for power failure safety
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
        'sector_size' / Int32ul,  # always 4096 for the types of NOR flash supported by ESP-IDF!
        'updaterate' / Int32ul,
        'wr_size' / Int32ul,
        'version' / Int32ul,
        'temp_buff_size' / Int32ul
    )
    WL_CONFIG_T_HEADER_SIZE = 48

    def __init__(self,
                 size: int = FATDefaults.SIZE,
                 sector_size: int = FATDefaults.SECTOR_SIZE,
                 reserved_sectors_cnt: int = FATDefaults.RESERVED_SECTORS_COUNT,
                 fat_tables_cnt: int = FATDefaults.FAT_TABLES_COUNT,
                 sectors_per_cluster: int = FATDefaults.SECTORS_PER_CLUSTER,
                 explicit_fat_type: int = None,
                 hidden_sectors: int = FATDefaults.HIDDEN_SECTORS,
                 long_names_enabled: bool = False,
                 num_heads: int = FATDefaults.NUM_HEADS,
                 oem_name: str = FATDefaults.OEM_NAME,
                 sec_per_track: int = FATDefaults.SEC_PER_TRACK,
                 volume_label: str = FATDefaults.VOLUME_LABEL,
                 file_sys_type: str = FATDefaults.FILE_SYS_TYPE,
                 use_default_datetime: bool = True,
                 version: int = FATDefaults.VERSION,
                 temp_buff_size: int = FATDefaults.TEMP_BUFFER_SIZE,
                 device_id: int = None,
                 root_entry_count: int = FATDefaults.ROOT_ENTRIES_COUNT,
                 media_type: int = FATDefaults.MEDIA_TYPE) -> None:
        self._initialized = False
        self._version = version
        self._temp_buff_size = temp_buff_size
        self._device_id = device_id
        self.partition_size = size
        self.total_sectors = self.partition_size // FATDefaults.WL_SECTOR_SIZE
        self.wl_state_size = WLFATFS.WL_STATE_HEADER_SIZE + WLFATFS.WL_STATE_RECORD_SIZE * self.total_sectors

        # determine the number of required sectors (roundup to sector size)
        self.wl_state_sectors = (self.wl_state_size + FATDefaults.WL_SECTOR_SIZE - 1) // FATDefaults.WL_SECTOR_SIZE

        self.boot_sector_start = FATDefaults.WL_SECTOR_SIZE  # shift by one "dummy" sector
        self.fat_table_start = self.boot_sector_start + reserved_sectors_cnt * FATDefaults.WL_SECTOR_SIZE

        wl_sectors = (WLFATFS.WL_DUMMY_SECTORS_COUNT + WLFATFS.WL_CFG_SECTORS_COUNT +
                      self.wl_state_sectors * WLFATFS.WL_STATE_COPY_COUNT)
        self.plain_fat_sectors = self.total_sectors - wl_sectors
        self.plain_fatfs = FATFS(
            explicit_fat_type=explicit_fat_type,
            size=self.plain_fat_sectors * FATDefaults.WL_SECTOR_SIZE,
            reserved_sectors_cnt=reserved_sectors_cnt,
            fat_tables_cnt=fat_tables_cnt,
            sectors_per_cluster=sectors_per_cluster,
            sector_size=sector_size,
            root_entry_count=root_entry_count,
            hidden_sectors=hidden_sectors,
            long_names_enabled=long_names_enabled,
            num_heads=num_heads,
            use_default_datetime=use_default_datetime,
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
        self.fatfs_binary_image = FATDefaults.WL_SECTOR_SIZE * FULL_BYTE + self.fatfs_binary_image

    def _add_config_sector(self) -> None:
        wl_config_data = WLFATFS.WL_CONFIG_T_DATA.build(
            dict(
                start_addr=0,
                full_mem_size=self.partition_size,
                page_size=FATDefaults.WL_SECTOR_SIZE,  # equal to sector size (always 4096)
                sector_size=FATDefaults.WL_SECTOR_SIZE,
                updaterate=FATDefaults.UPDATE_RATE,
                wr_size=FATDefaults.WR_SIZE,
                version=self._version,
                temp_buff_size=self._temp_buff_size
            )
        )

        crc = crc32(list(wl_config_data), UINT32_MAX)
        wl_config_crc = Int32ul.build(crc)

        # adding three 4 byte zeros to align the structure
        wl_config = wl_config_data + wl_config_crc + Int32ul.build(0) + Int32ul.build(0) + Int32ul.build(0)

        self.fatfs_binary_image += (
            wl_config + (FATDefaults.WL_SECTOR_SIZE - WLFATFS.WL_CONFIG_HEADER_SIZE) * FULL_BYTE)

    def _add_state_sectors(self) -> None:
        wl_state_data = WLFATFS.WL_STATE_T_DATA.build(
            dict(
                pos=0,
                max_pos=self.plain_fat_sectors + WLFATFS.WL_DUMMY_SECTORS_COUNT,
                move_count=0,
                access_count=0,
                max_count=FATDefaults.UPDATE_RATE,
                block_size=FATDefaults.WL_SECTOR_SIZE,  # equal to page size, thus equal to wl sector size (4096)
                version=self._version,
                device_id=self._device_id or generate_4bytes_random(),
            )
        )
        crc = crc32(list(wl_state_data), UINT32_MAX)
        wl_state_crc = Int32ul.build(crc)
        wl_state = wl_state_data + wl_state_crc
        wl_state_sector_padding: bytes = (FATDefaults.WL_SECTOR_SIZE - WLFATFS.WL_STATE_HEADER_SIZE) * FULL_BYTE
        wl_state_sector: bytes = (
            wl_state + wl_state_sector_padding + (self.wl_state_sectors - 1) * FATDefaults.WL_SECTOR_SIZE * FULL_BYTE
        )
        self.fatfs_binary_image += (WLFATFS.WL_STATE_COPY_COUNT * wl_state_sector)

    def wl_write_filesystem(self, output_path: str) -> None:
        if not self._initialized:
            raise WLNotInitialized('FATFS is not initialized with WL. First call method WLFATFS.init_wl!')
        with open(output_path, 'wb') as output:
            output.write(bytearray(self.fatfs_binary_image))


if __name__ == '__main__':
    desc = 'Create a FAT filesystem with support for wear levelling and populate it with directory content'
    args = get_args_for_partition_generator(desc, wl=True)
    wl_fatfs = WLFATFS(sectors_per_cluster=args.sectors_per_cluster,
                       size=args.partition_size,
                       sector_size=args.sector_size,
                       root_entry_count=args.root_entry_count,
                       explicit_fat_type=args.fat_type,
                       long_names_enabled=args.long_name_support,
                       use_default_datetime=args.use_default_datetime)

    wl_fatfs.plain_fatfs.generate(args.input_directory)
    wl_fatfs.init_wl()
    wl_fatfs.wl_write_filesystem(args.output_file)
