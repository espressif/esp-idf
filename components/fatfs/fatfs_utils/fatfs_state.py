# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from textwrap import dedent
from typing import Optional

from .exceptions import InconsistentFATAttributes
from .utils import (ALLOWED_SECTOR_SIZES, FAT12, FAT12_MAX_CLUSTERS, FAT16, FAT16_MAX_CLUSTERS,
                    RESERVED_CLUSTERS_COUNT, FATDefaults, get_fat_sectors_count, get_fatfs_type,
                    get_non_data_sectors_cnt, number_of_clusters)


class FATFSState:
    """
    The class represents the state and the configuration of the FATFS.
    """

    def __init__(self,
                 sector_size: int,
                 reserved_sectors_cnt: int,
                 root_dir_sectors_cnt: int,
                 size: int,
                 media_type: int,
                 sectors_per_cluster: int,
                 volume_label: str,
                 oem_name: str,
                 fat_tables_cnt: int,
                 sec_per_track: int,
                 num_heads: int,
                 hidden_sectors: int,
                 file_sys_type: str,
                 use_default_datetime: bool,
                 explicit_fat_type: Optional[int] = None,
                 long_names_enabled: bool = False):
        self.boot_sector_state = BootSectorState(oem_name=oem_name,
                                                 sector_size=sector_size,
                                                 sectors_per_cluster=sectors_per_cluster,
                                                 reserved_sectors_cnt=reserved_sectors_cnt,
                                                 fat_tables_cnt=fat_tables_cnt,
                                                 root_dir_sectors_cnt=root_dir_sectors_cnt,
                                                 sectors_count=size // sector_size,
                                                 media_type=media_type,
                                                 sec_per_track=sec_per_track,
                                                 num_heads=num_heads,
                                                 hidden_sectors=hidden_sectors,
                                                 volume_label=volume_label,
                                                 file_sys_type=file_sys_type,
                                                 volume_uuid=-1)

        self._explicit_fat_type: Optional[int] = explicit_fat_type
        self.long_names_enabled: bool = long_names_enabled
        self.use_default_datetime: bool = use_default_datetime

        if (size // sector_size) * sectors_per_cluster in (FAT12_MAX_CLUSTERS, FAT16_MAX_CLUSTERS):
            print('WARNING: It is not recommended to create FATFS with bounding '
                  f'count of clusters: {FAT12_MAX_CLUSTERS} or {FAT16_MAX_CLUSTERS}')
        self.check_fat_type()

    @property
    def binary_image(self) -> bytearray:
        return self.boot_sector_state.binary_image

    @binary_image.setter
    def binary_image(self, value: bytearray) -> None:
        self.boot_sector_state.binary_image = value

    def check_fat_type(self) -> None:
        _type = self.boot_sector_state.fatfs_type
        if self._explicit_fat_type is not None and self._explicit_fat_type != _type:
            raise InconsistentFATAttributes(dedent(
                f"""FAT type you specified is inconsistent with other attributes of the system.
                    The specified FATFS type: FAT{self._explicit_fat_type}
                    The actual FATFS type: FAT{_type}"""))
        if _type not in (FAT12, FAT16):
            raise NotImplementedError('FAT32 is currently not supported.')


class BootSectorState:
    # pylint: disable=too-many-instance-attributes
    def __init__(self,
                 oem_name: str,
                 sector_size: int,
                 sectors_per_cluster: int,
                 reserved_sectors_cnt: int,
                 fat_tables_cnt: int,
                 root_dir_sectors_cnt: int,
                 sectors_count: int,
                 media_type: int,
                 sec_per_track: int,
                 num_heads: int,
                 hidden_sectors: int,
                 volume_label: str,
                 file_sys_type: str,
                 volume_uuid: int = -1) -> None:
        self.oem_name: str = oem_name
        self.sector_size: int = sector_size
        assert self.sector_size in ALLOWED_SECTOR_SIZES
        self.sectors_per_cluster: int = sectors_per_cluster
        self.reserved_sectors_cnt: int = reserved_sectors_cnt
        self.fat_tables_cnt: int = fat_tables_cnt
        self.root_dir_sectors_cnt: int = root_dir_sectors_cnt
        self.sectors_count: int = sectors_count
        self.media_type: int = media_type
        self.sectors_per_fat_cnt = get_fat_sectors_count(self.size // self.sector_size, self.sector_size)
        self.sec_per_track: int = sec_per_track
        self.num_heads: int = num_heads
        self.hidden_sectors: int = hidden_sectors
        self.volume_label: str = volume_label
        self.file_sys_type: str = file_sys_type
        self.volume_uuid: int = volume_uuid
        self._binary_image: bytearray = bytearray(b'')

    @property
    def binary_image(self) -> bytearray:
        return self._binary_image

    @binary_image.setter
    def binary_image(self, value: bytearray) -> None:
        self._binary_image = value

    @property
    def size(self) -> int:
        return self.sector_size * self.sectors_count

    @property
    def data_region_start(self) -> int:
        return self.non_data_sectors * self.sector_size

    @property
    def fatfs_type(self) -> int:
        # variable typed_fatfs_type must be explicitly typed to avoid mypy error
        typed_fatfs_type: int = get_fatfs_type(self.clusters)
        return typed_fatfs_type

    @property
    def clusters(self) -> int:
        """
        The actual number of clusters is calculated by `number_of_clusters`,
        however, the initial two blocks of FAT are reserved (device type and root directory),
        despite they don't refer to the data region.
        Since that, two clusters are added to use the full potential of the FAT file system partition.
        """
        clusters_cnt_: int = number_of_clusters(self.data_sectors, self.sectors_per_cluster) + RESERVED_CLUSTERS_COUNT
        return clusters_cnt_

    @property
    def data_sectors(self) -> int:
        # self.sector_size is checked in constructor if has one of allowed values (ALLOWED_SECTOR_SIZES)
        return (self.size // self.sector_size) - self.non_data_sectors

    @property
    def non_data_sectors(self) -> int:
        non_data_sectors_: int = get_non_data_sectors_cnt(self.reserved_sectors_cnt,
                                                          self.sectors_per_fat_cnt,
                                                          self.fat_tables_cnt,
                                                          self.root_dir_sectors_cnt)
        return non_data_sectors_

    @property
    def fat_table_start_address(self) -> int:
        return self.sector_size * self.reserved_sectors_cnt

    @property
    def entries_root_count(self) -> int:
        entries_root_count_: int = (self.root_dir_sectors_cnt * self.sector_size) // FATDefaults.ENTRY_SIZE
        return entries_root_count_

    @property
    def root_directory_start(self) -> int:
        root_dir_start: int = (self.reserved_sectors_cnt + self.sectors_per_fat_cnt * self.fat_tables_cnt) * self.sector_size
        return root_dir_start
