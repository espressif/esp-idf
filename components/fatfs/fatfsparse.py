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

    # if LFN was detected, the record is considered as single SFN record only if DIR_NTRes == 0x18 (LDIR_DIR_NTRES)
    # if LFN was not detected, the record cannot be part of the LFN, no matter the value of DIR_NTRes
    if not args.long_name_support or obj_['DIR_NTRes'] == Entry.LDIR_DIR_NTRES:
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
        except (construct.core.ConstError, UnicodeDecodeError, construct.core.StringError):
            args.long_name_support = True
            continue

        if obj_['DIR_Attr'] == 0:  # empty entry
            continue

        obj_name_: str = get_obj_name(obj_,
                                      directory_bytes_,
                                      entry_position_=i,
                                      lfn_checksum_=lfn_checksum(obj_['DIR_Name'] + obj_['DIR_Name_ext']))
        if obj_['DIR_Attr'] == Entry.ATTR_ARCHIVE:
            content_ = b''
            if obj_['DIR_FileSize'] > 0:
                content_ = fat_.get_chained_content(cluster_id_=Entry.get_cluster_id(obj_),
                                                    size=obj_['DIR_FileSize'])
            with open(os.path.join(name, obj_name_), 'wb') as new_file:
                new_file.write(content_)
        elif obj_['DIR_Attr'] == Entry.ATTR_DIRECTORY:
            # avoid creating symlinks to itself and parent folder
            if obj_name_ in ('.', '..'):
                continue
            child_directory_bytes_ = fat_.get_chained_content(cluster_id_=obj_['DIR_FstClusLO'])
            traverse_folder_tree(directory_bytes_=child_directory_bytes_,
                                 name=os.path.join(name, obj_name_),
                                 state_=state_,
                                 fat_=fat_,
                                 binary_array_=binary_array_)


def remove_wear_levelling_if_exists(fs_: bytes) -> bytes:
    """
    Detection of the wear levelling layer is performed in two steps:
    1) check if the first sector is a valid boot sector
    2) check if the size defined in the boot sector is the same as the partition size:
        - if it is, there is no wear levelling layer
        - otherwise, we need to remove wl for further processing
    """
    try:
        boot_sector__ = BootSector()
        boot_sector__.parse_boot_sector(fs_)
        if boot_sector__.boot_sector_state.size == len(fs_):
            return fs_
    except construct.core.ConstError:
        pass
    plain_fs: bytes = remove_wl(fs_)
    return plain_fs


if __name__ == '__main__':
    desc = 'Tool for parsing fatfs image and extracting directory structure on host.'
    argument_parser: argparse.ArgumentParser = argparse.ArgumentParser(description=desc)
    argument_parser.add_argument('input_image',
                                 help='Path to the image that will be parsed and extracted.')
    argument_parser.add_argument('--long-name-support',
                                 action='store_true',
                                 help=argparse.SUPPRESS)

    # ensures backward compatibility
    argument_parser.add_argument('--wear-leveling',
                                 action='store_true',
                                 help=argparse.SUPPRESS)
    argument_parser.add_argument('--wl-layer',
                                 choices=['detect', 'enabled', 'disabled'],
                                 default=None,
                                 help="If detection doesn't work correctly, "
                                      'you can force analyzer to or not to assume WL.')

    args = argument_parser.parse_args()

    # if wear levelling is detected or user explicitly sets the parameter `--wl_layer enabled`
    # the partition with wear levelling is transformed to partition without WL for convenient parsing
    # in some cases the partitions with and without wear levelling can be 100% equivalent
    # and only user can break this tie by explicitly setting
    # the parameter --wl-layer to enabled, respectively disabled
    if args.wear_leveling and args.wl_layer:
        raise NotImplementedError('Argument --wear-leveling cannot be combined with --wl-layer!')
    if args.wear_leveling:
        args.wl_layer = 'enabled'
    args.wl_layer = args.wl_layer or 'detect'

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
    if args.wl_layer == 'enabled':
        fs = remove_wl(fs)
    elif args.wl_layer != 'disabled':
        # wear levelling is removed to enable parsing using common algorithm
        fs = remove_wear_levelling_if_exists(fs)

    boot_sector_ = BootSector()
    boot_sector_.parse_boot_sector(fs)
    fat = FAT(boot_sector_.boot_sector_state, init_=False)

    boot_dir_start_ = boot_sector_.boot_sector_state.root_directory_start
    boot_dir_sectors = boot_sector_.boot_sector_state.root_dir_sectors_cnt
    full_ = fs[boot_dir_start_: boot_dir_start_ + boot_dir_sectors * boot_sector_.boot_sector_state.sector_size]
    traverse_folder_tree(full_,
                         boot_sector_.boot_sector_state.volume_label.rstrip(chr(PAD_CHAR)),
                         boot_sector_.boot_sector_state, fat, fs)
