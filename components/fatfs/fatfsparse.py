# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import sys
from typing import Tuple

from fatfs_utils.boot_sector import BootSector
from fatfs_utils.cluster import Cluster
from fatfs_utils.entry import Entry
from fatfs_utils.fat import FAT
from fatfs_utils.fatfs_state import BootSectorState
from fatfs_utils.utils import PAD_CHAR, FATDefaults, read_filesystem


def get_chained_full_content(cluster_id_: int,
                             fat_: FAT,
                             state_: BootSectorState,
                             binary_array_: bytearray) -> bytearray:
    if fat_.is_cluster_last(cluster_id_):
        data_address_ = Cluster.compute_cluster_data_address(state_, cluster_id_)
        content_: bytearray = binary_array_[data_address_: data_address_ + state_.sector_size]
        return content_
    fat_value_: int = fat_.get_cluster_value(cluster_id_)
    data_address_ = Cluster.compute_cluster_data_address(state_, cluster_id_)
    content_ = binary_array_[data_address_: data_address_ + state_.sector_size]

    while not fat_.is_cluster_last(cluster_id_):
        cluster_id_ = fat_value_
        fat_value_ = fat_.get_cluster_value(cluster_id_)
        data_address_ = Cluster.compute_cluster_data_address(state_, cluster_id_)
        content_ += binary_array_[data_address_: data_address_ + state_.sector_size]
    return content_


def get_name_and_id(obj_: dict) -> Tuple[str, int]:
    cluster_id_ = obj_['DIR_FstClusLO']
    obj_ext_ = obj_['DIR_Name_ext'].rstrip(chr(PAD_CHAR))
    ext_ = f'.{obj_ext_}' if len(obj_ext_) > 0 else ''
    obj_name_ = obj_['DIR_Name'].rstrip(chr(PAD_CHAR)) + ext_
    return obj_name_, cluster_id_


def traverse_folder_tree(directory_bytes_: bytes,
                         name: str,
                         state_: BootSectorState, fat_: FAT,
                         binary_array_: bytearray) -> None:
    if name not in ('.', '..'):
        os.makedirs(name)
    for i in range(len(directory_bytes_) // FATDefaults.ENTRY_SIZE):
        obj_address_ = FATDefaults.ENTRY_SIZE * i
        obj_ = Entry.ENTRY_FORMAT_SHORT_NAME.parse(
            directory_bytes_[obj_address_: obj_address_ + FATDefaults.ENTRY_SIZE])
        if obj_['DIR_Attr'] == Entry.ATTR_ARCHIVE:
            obj_name_, cluster_id_ = get_name_and_id(obj_)
            content_ = get_chained_full_content(
                cluster_id_=cluster_id_,
                fat_=fat_,
                state_=state_,
                binary_array_=binary_array_
            ).rstrip(chr(0x00).encode())
            with open(os.path.join(name, obj_name_), 'wb') as new_file:
                new_file.write(content_)
        elif obj_['DIR_Attr'] == Entry.ATTR_DIRECTORY:
            obj_name_, cluster_id_ = get_name_and_id(obj_)
            if obj_name_ in ('.', '..'):
                continue
            child_directory_bytes_ = get_chained_full_content(
                cluster_id_=obj_['DIR_FstClusLO'],
                fat_=fat_,
                state_=state_,
                binary_array_=binary_array_
            )
            traverse_folder_tree(directory_bytes_=child_directory_bytes_,
                                 name=os.path.join(name, obj_name_),
                                 state_=state_,
                                 fat_=fat_,
                                 binary_array_=binary_array_)


if __name__ == '__main__':
    fs = read_filesystem(sys.argv[1])
    parser = BootSector()
    parser.parse_boot_sector(fs)
    fat = FAT(parser.boot_sector_state, init_=False)

    boot_dir_start_ = parser.boot_sector_state.root_directory_start
    boot_dir_sectors = parser.boot_sector_state.root_dir_sectors_cnt
    full_ = fs[boot_dir_start_: boot_dir_start_ + boot_dir_sectors * parser.boot_sector_state.sector_size]
    traverse_folder_tree(full_,
                         parser.boot_sector_state.volume_label.rstrip(chr(PAD_CHAR)),
                         parser.boot_sector_state, fat, fs)
