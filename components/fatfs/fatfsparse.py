#!/usr/bin/env python
# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os

import construct
from fatfs_utils.boot_sector import BootSector
from fatfs_utils.entry import Entry
from fatfs_utils.fat import FAT
from fatfs_utils.fatfs_state import BootSectorState
from fatfs_utils.utils import FULL_BYTE, LONG_NAMES_ENCODING, PAD_CHAR, FATDefaults, lfn_checksum, read_filesystem
from wl_fatfsgen import remove_wl


def build_file_name(name1: bytes, name2: bytes, name3: bytes) -> str:
    full_name_ = name1 + name2 + name3
    # need to strip empty bytes and null-terminating char ('\x00')
    return full_name_.rstrip(FULL_BYTE).decode(LONG_NAMES_ENCODING).rstrip('\x00')


def get_obj_name(obj_: dict, directory_bytes_: bytes, entry_position_: int, lfn_checksum_: int) -> str:
    obj_ext_ = obj_['DIR_Name_ext'].rstrip(chr(PAD_CHAR))
    ext_ = f'.{obj_ext_}' if len(obj_ext_) > 0 else ''
    obj_name_: str = obj_['DIR_Name'].rstrip(chr(PAD_CHAR)) + ext_  # short entry name

    if not args.long_name_support:
        return obj_name_

    full_name = {}

    for pos in range(entry_position_ - 1, -1, -1):  # loop from the current entry back to the start
        obj_address_: int = FATDefaults.ENTRY_SIZE * pos
        entry_bytes_: bytes = directory_bytes_[obj_address_: obj_address_ + FATDefaults.ENTRY_SIZE]
        struct_ = Entry.parse_entry_long(entry_bytes_, lfn_checksum_)
        if len(struct_.items()) > 0:
            full_name[struct_['order']] = build_file_name(struct_['name1'], struct_['name2'], struct_['name3'])
            if struct_['is_last']:
                break
    return ''.join(map(lambda x: x[1], sorted(full_name.items()))) or obj_name_


def traverse_folder_tree(directory_bytes_: bytes,
                         name: str,
                         state_: BootSectorState,
                         fat_: FAT,
                         binary_array_: bytes) -> None:
    os.makedirs(name)

    assert len(directory_bytes_) % FATDefaults.ENTRY_SIZE == 0
    entries_count_: int = len(directory_bytes_) // FATDefaults.ENTRY_SIZE

    for i in range(entries_count_):
        obj_address_: int = FATDefaults.ENTRY_SIZE * i
        try:
            obj_: dict = Entry.ENTRY_FORMAT_SHORT_NAME.parse(
                directory_bytes_[obj_address_: obj_address_ + FATDefaults.ENTRY_SIZE])
        except (construct.core.ConstError, UnicodeDecodeError) as e:
            if not args.long_name_support:
                raise e
            continue

        if obj_['DIR_Attr'] == 0:  # empty entry
            continue

        obj_name_: str = get_obj_name(obj_,
                                      directory_bytes_,
                                      entry_position_=i,
                                      lfn_checksum_=lfn_checksum(obj_['DIR_Name'] + obj_['DIR_Name_ext']))
        if obj_['DIR_Attr'] == Entry.ATTR_ARCHIVE:
            content_ = fat_.chain_content(cluster_id_=Entry.get_cluster_id(obj_)).rstrip(chr(0x00).encode())
            with open(os.path.join(name, obj_name_), 'wb') as new_file:
                new_file.write(content_)
        elif obj_['DIR_Attr'] == Entry.ATTR_DIRECTORY:
            # avoid creating symlinks to itself and parent folder
            if obj_name_ in ('.', '..'):
                continue
            child_directory_bytes_ = fat_.chain_content(cluster_id_=obj_['DIR_FstClusLO'])
            traverse_folder_tree(directory_bytes_=child_directory_bytes_,
                                 name=os.path.join(name, obj_name_),
                                 state_=state_,
                                 fat_=fat_,
                                 binary_array_=binary_array_)


if __name__ == '__main__':
    desc = 'Tool for parsing fatfs image and extracting directory structure on host.'
    argument_parser: argparse.ArgumentParser = argparse.ArgumentParser(description=desc)
    argument_parser.add_argument('input_image',
                                 help='Path to the image that will be parsed and extracted.')
    argument_parser.add_argument('--long-name-support',
                                 action='store_true',
                                 help='Set flag to enable long names support.')

    argument_parser.add_argument('--wear-leveling',
                                 action='store_true',
                                 help='Set flag to parse an image encoded using wear levelling.')

    args = argument_parser.parse_args()

    fs = read_filesystem(args.input_image)

    # An algorithm for removing wear levelling:
    # 1. find an remove dummy sector:
    #    a) dummy sector is at the position defined by the number of records in the state sector
    #    b) dummy may not be placed in state nor cfg sectors
    #    c) first (boot) sector position (boot_s_pos) is calculated using value of move count
    #    boot_s_pos = - mc
    # 2. remove state sectors (trivial)
    # 3. remove cfg sector (trivial)
    # 4. valid fs is then old_fs[-mc:] + old_fs[:-mc]
    if args.wear_leveling:
        fs = remove_wl(fs)
    boot_sector_ = BootSector()
    boot_sector_.parse_boot_sector(fs)
    fat = FAT(boot_sector_.boot_sector_state, init_=False)

    boot_dir_start_ = boot_sector_.boot_sector_state.root_directory_start
    boot_dir_sectors = boot_sector_.boot_sector_state.root_dir_sectors_cnt
    full_ = fs[boot_dir_start_: boot_dir_start_ + boot_dir_sectors * boot_sector_.boot_sector_state.sector_size]
    traverse_folder_tree(full_,
                         boot_sector_.boot_sector_state.volume_label.rstrip(chr(PAD_CHAR)),
                         boot_sector_.boot_sector_state, fat, fs)
