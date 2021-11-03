# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0


class FATFSState:
    """
    The class represents the state and the configuration of the FATFS.
    """
    FAT12_MAX_CLUSTERS = 4085
    FAT16_MAX_CLUSTERS = 65525
    FAT12 = 12
    FAT16 = 16
    FAT32 = 32

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
                 wl_sectors: int,
                 long_names_enabled: bool = False):
        self._binary_image: bytearray = bytearray(b'')
        self.fat_tables_cnt: int = fat_tables_cnt
        self.oem_name: str = oem_name
        self.wl_sectors_cnt: int = wl_sectors
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
        return self.reserved_sectors_cnt + self.sectors_per_fat_cnt + self.root_dir_sectors_cnt + self.wl_sectors_cnt

    @property
    def data_region_start(self) -> int:
        return self.non_data_sectors * self.sector_size

    @property
    def max_clusters(self) -> int:
        return self.data_sectors // self.sectors_per_cluster

    @property
    def root_directory_start(self) -> int:
        return (self.reserved_sectors_cnt + self.sectors_per_fat_cnt) * self.sector_size

    @property
    def fatfs_type(self) -> int:
        if self.max_clusters < FATFSState.FAT12_MAX_CLUSTERS:
            return FATFSState.FAT12
        elif self.max_clusters < FATFSState.FAT16_MAX_CLUSTERS:
            return FATFSState.FAT16
        # fat is FAT.FAT32, not supported now
        raise NotImplementedError('FAT32 is currently not supported.')

    @property
    def entries_root_count(self) -> int:
        return (self.root_dir_sectors_cnt * self.sector_size) // self.entry_size
