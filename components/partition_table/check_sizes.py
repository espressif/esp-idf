#!/usr/bin/env python
#
# check_sizes.py is a tool run by the ESP-IDF build system
# to check a particular binary fits in the available partitions of
# a particular type/subtype. Can be used to check if the app binary fits in
# all available app partitions, for example.
#
# (Can also check if the bootloader binary fits before the partition table.)
#
# SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import division, print_function, unicode_literals

import argparse
import io  # noqa: F401 # pylint: disable=unused-import
import os
import sys

try:
    from typing import IO  # noqa: F401 # pylint: disable=unused-import
except ImportError:
    pass  # used for type hinting only

import gen_esp32part
from gen_esp32part import PartitionTable, get_ptype_as_int, get_subtype_as_int

allow_failures = False


def _file_size(f):   # type: (IO) -> int
    before = f.tell()
    f.seek(0, 2)  # seek to end
    result = f.tell()
    f.seek(before)
    return result


def _fail(msg):   # type: (str) -> None
    if allow_failures:
        print('Warning: {}'.format(msg))
    else:
        raise SystemExit('Error: {}'.format(msg))


def check_bootloader(partition_table_offset, bootloader_offset, binary_file):  # type: (int, int, IO) -> None
    max_size = partition_table_offset - bootloader_offset
    bootloader_size = _file_size(binary_file)
    if bootloader_size > max_size:
        msg = ('Bootloader binary size {:#x} bytes is too large for partition table offset {:#02x}. ' +
               'Bootloader binary can be maximum {:#x} ({}) bytes unless the partition table offset ' +
               'is increased in the Partition Table section of the project configuration menu.').format(
                   bootloader_size, partition_table_offset, max_size, max_size)
        _fail(msg)
    free_size = max_size - bootloader_size
    print('Bootloader binary size {:#x} bytes. {:#x} bytes ({}%) free.'.format(
          bootloader_size, free_size, round(free_size * 100 / max_size)))


def check_partition(ptype, subtype, partition_table_file, bin_file):  # type: (str, str, io.IOBase, IO) -> None
    table, _ = PartitionTable.from_file(partition_table_file)
    ptype_str = str(ptype)
    ptype = get_ptype_as_int(ptype)
    partitions = [p for p in table if p.type == ptype]

    if subtype is not None:
        ptype_str += ' ({})'.format(subtype)
        subtype = get_subtype_as_int(ptype, subtype)
        partitions = [p for p in partitions if p.subtype == subtype]

    if len(partitions) == 0:
        print('WARNING: Partition table does not contain any partitions matching {}'.format(ptype_str))
        return

    bin_name = os.path.basename(bin_file.name)
    bin_size = _file_size(bin_file)
    smallest_size = min(p.size for p in partitions)
    if smallest_size >= bin_size:
        free_size = smallest_size - bin_size
        free_size_relative = free_size / smallest_size
        print('{} binary size {:#x} bytes. Smallest {} partition is {:#x} bytes. {:#x} bytes ({:.0%}) free.'.format(
            bin_name, bin_size, ptype_str, smallest_size, free_size, free_size_relative))
        free_size_relative_critical = 0.05
        if free_size_relative < free_size_relative_critical:
            print('Warning: The smallest {} partition is nearly full ({:.0%} free space left)!'.format(ptype_str, free_size_relative))
        return

    too_small_partitions = [p for p in partitions if p.size < bin_size]
    if len(partitions) == 1:
        msg = '{} partition is'.format(ptype_str)
    elif len(partitions) == len(too_small_partitions):
        msg = 'All {} partitions are'.format(ptype_str)
    else:
        msg = '{}/{} {} partitions are'.format(len(too_small_partitions), len(partitions), ptype_str)
    msg += ' too small for binary {} size {:#x}:'.format(bin_name, bin_size)
    for p in too_small_partitions:
        msg += '\n  - {} (overflow {:#x})'.format(p, bin_size - p.size)
    if not allow_failures and len(partitions) == len(too_small_partitions):
        # if some partitions can fit the binary then just print a warning
        raise SystemExit('Error: ' + msg)
    else:
        print('Warning: ' + msg)


def main():  # type: () -> None
    global allow_failures  # pylint: disable=global-statement

    parser = argparse.ArgumentParser(description='Check binary sizes against partition table entries')
    parser.add_argument('--target', choices=['esp32', 'esp32s2'])
    parser.add_argument('--allow_failures', action='store_true', help='If true, failures will print warnings but not exit with an error')
    parser.add_argument('--offset', '-o', help='Set partition table offset', default='0x8000')

    subparsers = parser.add_subparsers(dest='check_target',
                                       help='Type of binary to check against partition table layout')
    sp_bootloader = subparsers.add_parser('bootloader')
    sp_bootloader.add_argument('bootloader_offset', help='Hex offset of bootloader in flash')
    sp_bootloader.add_argument('bootloader_binary', type=argparse.FileType('rb'), help='Bootloader binary (.bin) file from build output')

    sp_part = subparsers.add_parser('partition')
    sp_part.add_argument('--type', type=str, help='Check the file size against all partitions of this type.', required=True)
    sp_part.add_argument('--subtype', type=str, help='Optional, only check the file size against all partitions of this subtype.')
    sp_part.add_argument('partition_table', type=argparse.FileType('rb'), help='Partition table file')
    sp_part.add_argument('binary', type=argparse.FileType('rb'), help='Binary file which will have the size checked')

    args = parser.parse_args()

    gen_esp32part.quiet = True

    args.offset = int(args.offset, 0)
    gen_esp32part.offset_part_table = args.offset

    if args.check_target is None:  # add_subparsers only has a 'required' argument since Python 3
        parser.print_help()
        sys.exit(1)
    if args.check_target == 'bootloader':
        check_bootloader(args.offset, int(args.bootloader_offset, 0), args.bootloader_binary)
    else:
        check_partition(args.type, args.subtype, args.partition_table, args.binary)


if __name__ == '__main__':
    main()
