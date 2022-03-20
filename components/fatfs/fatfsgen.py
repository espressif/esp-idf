#!/usr/bin/env python
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
from typing import Any, List, Optional

from fatfsgen_utils.fat import FAT
from fatfsgen_utils.fatfs_parser import FATFSParser
from fatfsgen_utils.fatfs_state import FATFSState
from fatfsgen_utils.fs_object import Directory
from fatfsgen_utils.utils import (BYTES_PER_DIRECTORY_ENTRY, FAT32, generate_4bytes_random,
                                  get_args_for_partition_generator, pad_string, read_filesystem)


class FATFS:
    """
    The class FATFS provides API for generating FAT file system.
    It contains reference to the FAT table and to the root directory.
    """

    def __init__(self,
                 binary_image_path: Optional[str] = None,
                 size: int = 1024 * 1024,
                 reserved_sectors_cnt: int = 1,
                 fat_tables_cnt: int = 1,
                 sectors_per_cluster: int = 1,
                 sector_size: int = 0x1000,
                 sectors_per_fat: int = 1,
                 hidden_sectors: int = 0,
                 long_names_enabled: bool = False,
                 entry_size: int = 32,
                 num_heads: int = 0xff,
                 oem_name: str = 'MSDOS5.0',
                 sec_per_track: int = 0x3f,
                 volume_label: str = 'Espressif',
                 file_sys_type: str = 'FAT',
                 root_entry_count: int = 512,
                 explicit_fat_type: int = None,
                 media_type: int = 0xf8) -> None:

        assert (root_entry_count * BYTES_PER_DIRECTORY_ENTRY) % sector_size == 0
        assert ((root_entry_count * BYTES_PER_DIRECTORY_ENTRY) // sector_size) % 2 == 0

        root_dir_sectors_cnt: int = (root_entry_count * BYTES_PER_DIRECTORY_ENTRY) // sector_size

        self.state: FATFSState = FATFSState(entry_size=entry_size,
                                            sector_size=sector_size,
                                            explicit_fat_type=explicit_fat_type,
                                            reserved_sectors_cnt=reserved_sectors_cnt,
                                            root_dir_sectors_cnt=root_dir_sectors_cnt,
                                            size=size,
                                            file_sys_type=file_sys_type,
                                            num_heads=num_heads,
                                            fat_tables_cnt=fat_tables_cnt,
                                            sectors_per_fat=sectors_per_fat,
                                            sectors_per_cluster=sectors_per_cluster,
                                            media_type=media_type,
                                            hidden_sectors=hidden_sectors,
                                            sec_per_track=sec_per_track,
                                            long_names_enabled=long_names_enabled,
                                            volume_label=volume_label,
                                            oem_name=oem_name)
        binary_image: bytes = bytearray(
            read_filesystem(binary_image_path) if binary_image_path else self.create_empty_fatfs())
        self.state.binary_image = binary_image

        self.fat: FAT = FAT(fatfs_state=self.state, reserved_sectors_cnt=self.state.reserved_sectors_cnt)

        self.root_directory: Directory = Directory(name='A',  # the name is not important, must be string
                                                   size=self.state.root_dir_sectors_cnt * self.state.sector_size,
                                                   fat=self.fat,
                                                   cluster=self.fat.clusters[1],
                                                   fatfs_state=self.state)
        self.root_directory.init_directory()

    def create_file(self, name: str, extension: str = '', path_from_root: Optional[List[str]] = None) -> None:
        # when path_from_root is None the dir is root
        self.root_directory.new_file(name=name, extension=extension, path_from_root=path_from_root)

    def create_directory(self, name: str, path_from_root: Optional[List[str]] = None) -> None:
        # when path_from_root is None the dir is root
        parent_dir = self.root_directory
        if path_from_root:
            parent_dir = self.root_directory.recursive_search(path_from_root, self.root_directory)

        self.root_directory.new_directory(name=name, parent=parent_dir, path_from_root=path_from_root)

    def write_content(self, path_from_root: List[str], content: bytes) -> None:
        """
        fat fs invokes root directory to recursively find the required file and writes the content
        """
        self.root_directory.write_to_file(path_from_root, content)

    def create_empty_fatfs(self) -> Any:
        sectors_count = self.state.size // self.state.sector_size
        volume_uuid = generate_4bytes_random()
        return (
            FATFSParser.BOOT_SECTOR_HEADER.build(
                dict(BS_OEMName=pad_string(self.state.oem_name, size=FATFSParser.MAX_OEM_NAME_SIZE),
                     BPB_BytsPerSec=self.state.sector_size,
                     BPB_SecPerClus=self.state.sectors_per_cluster,
                     BPB_RsvdSecCnt=self.state.reserved_sectors_cnt,
                     BPB_NumFATs=self.state.fat_tables_cnt,
                     BPB_RootEntCnt=self.state.entries_root_count,
                     BPB_TotSec16=0x00 if self.state.fatfs_type == FAT32 else sectors_count,
                     BPB_Media=self.state.media_type,
                     BPB_FATSz16=self.state.sectors_per_fat_cnt,
                     BPB_SecPerTrk=self.state.sec_per_track,
                     BPB_NumHeads=self.state.num_heads,
                     BPB_HiddSec=self.state.hidden_sectors,
                     BPB_TotSec32=sectors_count if self.state.fatfs_type == FAT32 else 0x00,
                     BS_VolID=volume_uuid,
                     BS_VolLab=pad_string(self.state.volume_label, size=FATFSParser.MAX_VOL_LAB_SIZE),
                     BS_FilSysType=pad_string(self.state.file_sys_type, size=FATFSParser.MAX_FS_TYPE_SIZE)
                     )
            )
            + (self.state.sector_size - FATFSParser.BOOT_HEADER_SIZE) * b'\x00'
            + self.state.sectors_per_fat_cnt * self.state.fat_tables_cnt * self.state.sector_size * b'\x00'
            + self.state.root_dir_sectors_cnt * self.state.sector_size * b'\x00'
            + self.state.data_sectors * self.state.sector_size * b'\xff'
        )

    def write_filesystem(self, output_path: str) -> None:
        with open(output_path, 'wb') as output:
            output.write(bytearray(self.state.binary_image))

    def _generate_partition_from_folder(self,
                                        folder_relative_path: str,
                                        folder_path: str = '',
                                        is_dir: bool = False) -> None:
        """
        Given path to folder and folder name recursively encodes folder into binary image.
        Used by method generate
        """
        real_path = os.path.join(folder_path, folder_relative_path)
        smaller_path = folder_relative_path

        folder_relative_path = folder_relative_path.upper()

        normal_path = os.path.normpath(folder_relative_path)
        split_path = normal_path.split(os.sep)
        if os.path.isfile(real_path):
            with open(real_path, 'rb') as file:
                content = file.read()
            file_name, extension = os.path.splitext(split_path[-1])
            extension = extension[1:]  # remove the dot from the extension
            self.create_file(name=file_name, extension=extension, path_from_root=split_path[1:-1] or None)
            self.write_content(split_path[1:], content)
        elif os.path.isdir(real_path):
            if not is_dir:
                self.create_directory(split_path[-1], split_path[1:-1])

            # sorting files for better testability
            dir_content = list(sorted(os.listdir(real_path)))
            for path in dir_content:
                self._generate_partition_from_folder(os.path.join(smaller_path, path), folder_path=folder_path)

    def generate(self, input_directory: str) -> None:
        """
        Normalize path to folder and recursively encode folder to binary image
        """
        path_to_folder, folder_name = os.path.split(input_directory)
        self._generate_partition_from_folder(folder_name, folder_path=path_to_folder, is_dir=True)


def main() -> None:
    args = get_args_for_partition_generator('Create a FAT filesystem and populate it with directory content')
    fatfs = FATFS(sector_size=args.sector_size,
                  sectors_per_cluster=args.sectors_per_cluster,
                  size=args.partition_size,
                  root_entry_count=args.root_entry_count,
                  explicit_fat_type=args.fat_type,
                  long_names_enabled=args.long_name_support)

    fatfs.generate(args.input_directory)
    fatfs.write_filesystem(args.output_file)


if __name__ == '__main__':
    main()
