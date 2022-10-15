#!/usr/bin/env python
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
from datetime import datetime
from typing import Any, List, Optional

from fatfs_utils.boot_sector import BootSector
from fatfs_utils.exceptions import NoFreeClusterException
from fatfs_utils.fat import FAT
from fatfs_utils.fatfs_state import FATFSState
from fatfs_utils.fs_object import Directory
from fatfs_utils.long_filename_utils import get_required_lfn_entries_count
from fatfs_utils.utils import (BYTES_PER_DIRECTORY_ENTRY, FATFS_INCEPTION, FATFS_MIN_ALLOC_UNIT,
                               RESERVED_CLUSTERS_COUNT, FATDefaults, get_args_for_partition_generator,
                               get_fat_sectors_count, get_non_data_sectors_cnt, read_filesystem,
                               required_clusters_count)


class FATFS:
    """
    The class FATFS provides API for generating FAT file system.
    It contains reference to the FAT table and to the root directory.
    """

    def __init__(self,
                 binary_image_path: Optional[str] = None,
                 size: int = FATDefaults.SIZE,
                 reserved_sectors_cnt: int = FATDefaults.RESERVED_SECTORS_COUNT,
                 fat_tables_cnt: int = FATDefaults.FAT_TABLES_COUNT,
                 sectors_per_cluster: int = FATDefaults.SECTORS_PER_CLUSTER,
                 sector_size: int = FATDefaults.SECTOR_SIZE,
                 hidden_sectors: int = FATDefaults.HIDDEN_SECTORS,
                 long_names_enabled: bool = False,
                 use_default_datetime: bool = True,
                 num_heads: int = FATDefaults.NUM_HEADS,
                 oem_name: str = FATDefaults.OEM_NAME,
                 sec_per_track: int = FATDefaults.SEC_PER_TRACK,
                 volume_label: str = FATDefaults.VOLUME_LABEL,
                 file_sys_type: str = FATDefaults.FILE_SYS_TYPE,
                 root_entry_count: int = FATDefaults.ROOT_ENTRIES_COUNT,
                 explicit_fat_type: int = None,
                 media_type: int = FATDefaults.MEDIA_TYPE) -> None:
        # root directory bytes should be aligned by sector size
        assert (root_entry_count * BYTES_PER_DIRECTORY_ENTRY) % sector_size == 0
        # number of bytes in the root dir must be even multiple of BPB_BytsPerSec
        assert ((root_entry_count * BYTES_PER_DIRECTORY_ENTRY) // sector_size) % 2 == 0

        root_dir_sectors_cnt: int = (root_entry_count * BYTES_PER_DIRECTORY_ENTRY) // sector_size

        self.state: FATFSState = FATFSState(sector_size=sector_size,
                                            explicit_fat_type=explicit_fat_type,
                                            reserved_sectors_cnt=reserved_sectors_cnt,
                                            root_dir_sectors_cnt=root_dir_sectors_cnt,
                                            size=size,
                                            file_sys_type=file_sys_type,
                                            num_heads=num_heads,
                                            fat_tables_cnt=fat_tables_cnt,
                                            sectors_per_cluster=sectors_per_cluster,
                                            media_type=media_type,
                                            hidden_sectors=hidden_sectors,
                                            sec_per_track=sec_per_track,
                                            long_names_enabled=long_names_enabled,
                                            volume_label=volume_label,
                                            oem_name=oem_name,
                                            use_default_datetime=use_default_datetime)
        binary_image: bytes = bytearray(
            read_filesystem(binary_image_path) if binary_image_path else self.create_empty_fatfs())
        self.state.binary_image = binary_image

        self.fat: FAT = FAT(boot_sector_state=self.state.boot_sector_state, init_=True)

        root_dir_size = self.state.boot_sector_state.root_dir_sectors_cnt * self.state.boot_sector_state.sector_size
        self.root_directory: Directory = Directory(name='A',  # the name is not important, must be string
                                                   size=root_dir_size,
                                                   fat=self.fat,
                                                   cluster=self.fat.clusters[1],
                                                   fatfs_state=self.state)
        self.root_directory.init_directory()

    def create_file(self, name: str,
                    extension: str = '',
                    path_from_root: Optional[List[str]] = None,
                    object_timestamp_: datetime = FATFS_INCEPTION,
                    is_empty: bool = False) -> None:
        """
        Root directory recursively finds the parent directory of the new file, allocates cluster,
        entry and appends a new file into the parent directory.

        When path_from_root is None the dir is root.

        :param name: The name of the file.
        :param extension: The extension of the file.
        :param path_from_root: List of strings containing names of the ancestor directories in the given order.
        :param object_timestamp_: is not None, this will be propagated to the file's entry
        :param is_empty: True if there is no need to allocate any cluster, otherwise False
        """
        self.root_directory.new_file(name=name,
                                     extension=extension,
                                     path_from_root=path_from_root,
                                     object_timestamp_=object_timestamp_,
                                     is_empty=is_empty)

    def create_directory(self, name: str,
                         path_from_root: Optional[List[str]] = None,
                         object_timestamp_: datetime = FATFS_INCEPTION) -> None:
        """
        Initially recursively finds a parent of the new directory
        and then create a new directory inside the parent.

        When path_from_root is None the parent dir is root.

        :param name: The full name of the directory (excluding its path)
        :param path_from_root: List of strings containing names of the ancestor directories in the given order.
        :param object_timestamp_: in case the user preserves the timestamps, this will be propagated to the
        metadata of the directory (to the corresponding entry)
        :returns: None
        """
        parent_dir = self.root_directory
        if path_from_root:
            parent_dir = self.root_directory.recursive_search(path_from_root, self.root_directory)

        self.root_directory.new_directory(name=name,
                                          parent=parent_dir,
                                          path_from_root=path_from_root,
                                          object_timestamp_=object_timestamp_)

    def write_content(self, path_from_root: List[str], content: bytes) -> None:
        """
        fat fs invokes root directory to recursively find the required file and writes the content
        """
        self.root_directory.write_to_file(path_from_root, content)

    def create_empty_fatfs(self) -> Any:
        boot_sector_ = BootSector(boot_sector_state=self.state.boot_sector_state)
        boot_sector_.generate_boot_sector()
        return boot_sector_.binary_image

    def write_filesystem(self, output_path: str) -> None:
        with open(output_path, 'wb') as output:
            output.write(bytearray(self.state.binary_image))

    def _generate_partition_from_folder(self,
                                        folder_relative_path: str,
                                        folder_path: str = '',
                                        is_dir: bool = False) -> None:
        """
        Given path to folder and folder name recursively encodes folder into binary image.
        Used by method generate.
        """
        real_path: str = os.path.join(folder_path, folder_relative_path)
        lower_path: str = folder_relative_path

        folder_relative_path = folder_relative_path.upper()

        normal_path = os.path.normpath(folder_relative_path)
        split_path = normal_path.split(os.sep)
        object_timestamp = datetime.fromtimestamp(os.path.getctime(real_path))

        if os.path.isfile(real_path):
            with open(real_path, 'rb') as file:
                content = file.read()
            file_name, extension = os.path.splitext(split_path[-1])
            extension = extension[1:]  # remove the dot from the extension
            self.create_file(name=file_name,
                             extension=extension,
                             path_from_root=split_path[1:-1] or None,
                             object_timestamp_=object_timestamp,
                             is_empty=len(content) == 0)
            self.write_content(split_path[1:], content)
        elif os.path.isdir(real_path):
            if not is_dir:
                self.create_directory(name=split_path[-1],
                                      path_from_root=split_path[1:-1],
                                      object_timestamp_=object_timestamp)

            # sorting files for better testability
            dir_content = list(sorted(os.listdir(real_path)))
            for path in dir_content:
                self._generate_partition_from_folder(os.path.join(lower_path, path), folder_path=folder_path)

    def generate(self, input_directory: str) -> None:
        """
        Normalize path to folder and recursively encode folder to binary image
        """
        path_to_folder, folder_name = os.path.split(input_directory)
        self._generate_partition_from_folder(folder_name, folder_path=path_to_folder, is_dir=True)


def calculate_min_space(path: List[str],
                        fs_entity: str,
                        sector_size: int = 0x1000,
                        long_file_names: bool = False,
                        is_root: bool = False) -> int:
    if os.path.isfile(os.path.join(*path, fs_entity)):
        with open(os.path.join(*path, fs_entity), 'rb') as file_:
            content = file_.read()
        res: int = required_clusters_count(sector_size, content)
        return res
    buff: int = 0
    dir_size = 2 * FATDefaults.ENTRY_SIZE  # record for symlinks "." and ".."
    for file in sorted(os.listdir(os.path.join(*path, fs_entity))):
        if long_file_names and True:
            # LFN entries + one short entry
            dir_size += (get_required_lfn_entries_count(fs_entity) + 1) * FATDefaults.ENTRY_SIZE
        else:
            dir_size += FATDefaults.ENTRY_SIZE
        buff += calculate_min_space(path + [fs_entity], file, sector_size, long_file_names, is_root=False)
    if is_root and dir_size // FATDefaults.ENTRY_SIZE > FATDefaults.ROOT_ENTRIES_COUNT:
        raise NoFreeClusterException('Not enough space in root!')

    # roundup sectors, at least one is required
    buff += (dir_size + sector_size - 1) // sector_size
    return buff


def main() -> None:
    args = get_args_for_partition_generator('Create a FAT filesystem and populate it with directory content', wl=False)

    if args.partition_size == -1:
        clusters = calculate_min_space([], args.input_directory, args.sector_size, long_file_names=True, is_root=True)
        fats = get_fat_sectors_count(clusters, args.sector_size)
        root_dir_sectors = (FATDefaults.ROOT_ENTRIES_COUNT * FATDefaults.ENTRY_SIZE) // args.sector_size
        args.partition_size = max(FATFS_MIN_ALLOC_UNIT * args.sector_size,
                                  (clusters + fats + get_non_data_sectors_cnt(RESERVED_CLUSTERS_COUNT,
                                                                              fats,
                                                                              root_dir_sectors)
                                   ) * args.sector_size
                                  )

    fatfs = FATFS(sector_size=args.sector_size,
                  sectors_per_cluster=args.sectors_per_cluster,
                  size=args.partition_size,
                  root_entry_count=args.root_entry_count,
                  explicit_fat_type=args.fat_type,
                  long_names_enabled=args.long_name_support,
                  use_default_datetime=args.use_default_datetime)

    fatfs.generate(args.input_directory)
    fatfs.write_filesystem(args.output_file)


if __name__ == '__main__':
    main()
