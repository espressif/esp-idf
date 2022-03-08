# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from textwrap import dedent

from .exceptions import InconsistentFATAttributes
from .utils import (FAT12, FAT12_MAX_CLUSTERS, FAT16, FAT16_MAX_CLUSTERS, get_fatfs_type, get_non_data_sectors_cnt,
                    number_of_clusters)


class FATFSState:
    """
    The class represents the state and the configuration of the FATFS.
    """

    def __init__(self,
                 entry_size: int,
                 sector_size: int,
                 reserved_sectors_cnt: int,
                 root_dir_sectors_cnt: int,
                 size: int,
                 media_type: int,
                 sectors_per_fat: int,
                 sectors_per_cluster: int,
                 volume_label: str,
                 oem_name: str,
                 fat_tables_cnt: int,
                 sec_per_track: int,
                 num_heads: int,
                 hidden_sectors: int,
                 file_sys_type: str,
                 explicit_fat_type: int = None,
                 long_names_enabled: bool = False):

        self._explicit_fat_type = explicit_fat_type
        self._binary_image: bytearray = bytearray(b'')
        self.fat_tables_cnt: int = fat_tables_cnt
        self.oem_name: str = oem_name
        self.file_sys_type: str = file_sys_type
        self.sec_per_track: int = sec_per_track
        self.hidden_sectors: int = hidden_sectors
        self.volume_label: str = volume_label
        self.media_type: int = media_type
        self.long_names_enabled: bool = long_names_enabled
        self.entry_size: int = entry_size
        self.num_heads: int = num_heads
        self.sector_size: int = sector_size
        self.root_dir_sectors_cnt: int = root_dir_sectors_cnt
        self.reserved_sectors_cnt: int = reserved_sectors_cnt
        self.size: int = size
        self.sectors_per_fat_cnt: int = sectors_per_fat
        self.sectors_per_cluster: int = sectors_per_cluster

        if self.clusters in (FAT12_MAX_CLUSTERS, FAT16_MAX_CLUSTERS):
            print('WARNING: It is not recommended to create FATFS with bounding '
                  f'count of clusters: {FAT12_MAX_CLUSTERS} or {FAT16_MAX_CLUSTERS}')
        self.check_fat_type()

    @property
    def binary_image(self) -> bytearray:
        return self._binary_image

    @binary_image.setter
    def binary_image(self, value: bytearray) -> None:
        self._binary_image = value

    def get_dir_size(self, is_root: bool) -> int:
        return self.root_dir_sectors_cnt * self.sector_size if is_root else self.sector_size

    @property
    def start_address(self) -> int:
        return self.sector_size * self.reserved_sectors_cnt

    @property
    def data_sectors(self) -> int:
        return (self.size // self.sector_size) - self.non_data_sectors

    @property
    def non_data_sectors(self) -> int:
        return get_non_data_sectors_cnt(self.reserved_sectors_cnt, self.sectors_per_fat_cnt,  # type: ignore
                                        self.root_dir_sectors_cnt)

    @property
    def data_region_start(self) -> int:
        return self.non_data_sectors * self.sector_size

    @property
    def clusters(self) -> int:
        return number_of_clusters(self.data_sectors, self.sectors_per_cluster)  # type: ignore

    @property
    def root_directory_start(self) -> int:
        return (self.reserved_sectors_cnt + self.sectors_per_fat_cnt) * self.sector_size

    def check_fat_type(self) -> None:
        _type = self.fatfs_type
        if self._explicit_fat_type is not None and self._explicit_fat_type != _type:
            raise InconsistentFATAttributes(dedent(
                f"""FAT type you specified is inconsistent with other attributes of the system.
                    The specified FATFS type: FAT{self._explicit_fat_type}
                    The actual FATFS type: FAT{_type}"""))
        if _type not in (FAT12, FAT16):
            raise NotImplementedError('FAT32 is currently not supported.')

    @property
    def fatfs_type(self) -> int:
        # variable typed_fatfs_type must be explicitly typed to avoid mypy error
        typed_fatfs_type: int = get_fatfs_type(self.clusters)
        return typed_fatfs_type

    @property
    def entries_root_count(self) -> int:
        return (self.root_dir_sectors_cnt * self.sector_size) // self.entry_size
