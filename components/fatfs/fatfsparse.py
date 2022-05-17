# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import sys
from typing import Tuple

from fatfsgen_utils.boot_sector import BootSector
from fatfsgen_utils.cluster import Cluster
from fatfsgen_utils.entry import Entry
from fatfsgen_utils.fat import FAT
from fatfsgen_utils.fatfs_state import BootSectorState
from fatfsgen_utils.utils import PAD_CHAR, FATDefaults, read_filesystem


def get_address_and_name(obj_: dict, state_: BootSectorState) -> Tuple[int, str]:
    cluster_id_ = obj_['DIR_FstClusLO']
    obj_ext_ = obj_['DIR_Name_ext'].rstrip(chr(PAD_CHAR))
    ext_ = f'.{obj_ext_}' if len(obj_ext_) > 0 else ''
    obj_name_ = obj_['DIR_Name'].rstrip(chr(PAD_CHAR)) + ext_
    data_address_ = Cluster.compute_cluster_data_address(state_, cluster_id_)
    return data_address_, obj_name_


def traverse_folder_tree(directory_address: int, name: str, state_: BootSectorState) -> None:
    if name not in ('.', '..'):
        os.makedirs(name)
    for i in range(state_.sector_size // FATDefaults.ENTRY_SIZE):
        obj_address_ = directory_address + FATDefaults.ENTRY_SIZE * i
        obj_ = Entry.ENTRY_FORMAT_SHORT_NAME.parse(
            fs[obj_address_: obj_address_ + FATDefaults.ENTRY_SIZE])
        if obj_['DIR_Attr'] == Entry.ATTR_ARCHIVE:
            data_address_, obj_name_ = get_address_and_name(obj_, state_)
            content_ = fs[data_address_: data_address_ + state_.sector_size].rstrip(chr(0x00).encode())
            with open(os.path.join(name, obj_name_), 'wb') as new_file:
                new_file.write(content_)
        elif obj_['DIR_Attr'] == Entry.ATTR_DIRECTORY:
            data_address_, obj_name_ = get_address_and_name(obj_, state_)
            if obj_name_ in ('.', '..'):
                continue
            traverse_folder_tree(data_address_, os.path.join(name, obj_name_), state_=state_)


if __name__ == '__main__':
    fs = read_filesystem(sys.argv[1])
    parser = BootSector()
    parser.parse_boot_sector(fs)
    fat = FAT(parser.boot_sector_state, init_=False)

    traverse_folder_tree(parser.boot_sector_state.root_directory_start,
                         parser.boot_sector_state.volume_label.rstrip(chr(PAD_CHAR)),
                         parser.boot_sector_state)
