#!/usr/bin/env python
#
# check_sizes.py is a tool run by the ESP-IDF build system
# to check a particular binary fits in the available partitions of
# a particular type/subtype. Can be used to check if the app binary fits in
# all available app partitions, for example.
#
# (Can also check if the bootloader binary fits before the partition table.)
#
# SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0


import argparse
import io  # noqa: F401 # pylint: disable=unused-import
import os
import sys

try:
    from typing import IO  # noqa: F401 # pylint: disable=unused-import
except ImportError:
    pass  # used for type hinting only

import gen_esp32part
from gen_esp32part import PartitionTable
from gen_esp32part import get_ptype_as_int
from gen_esp32part import get_subtype_as_int

allow_failures = False


def _file_size(f):  # type: (IO) -> int
    before = f.tell()
    f.seek(0, 2)  # seek to end
    result = f.tell()
    f.seek(before)
    return result


def _fail(msg):  # type: (str) -> None
    if allow_failures:
        print(f'Warning: {msg}')
    else:
        raise SystemExit(f'Error: {msg}')


def check_bootloader(partition_table_offset, bootloader_offset, binary_file):  # type: (int, int, IO) -> None
    max_size = partition_table_offset - bootloader_offset
    bootloader_size = _file_size(binary_file)
    if bootloader_size > max_size:
        msg = (
            'Bootloader binary size {:#x} bytes is too large for partition table offset {:#02x}. '
            + 'Bootloader binary can be maximum {:#x} ({}) bytes unless the partition table offset '
            + 'is increased in the Partition Table section of the project configuration menu.'
        ).format(bootloader_size, partition_table_offset, max_size, max_size)
        _fail(msg)
    free_size = max_size - bootloader_size
    print(
        f'Bootloader binary size {bootloader_size:#x} bytes. '
        f'{free_size:#x} bytes ({round(free_size * 100 / max_size)}%) free.'
    )


def check_partition(ptype, subtype, partition_table_file, bin_file):  # type: (str, str, io.IOBase, IO) -> None
    table, _ = PartitionTable.from_file(partition_table_file)
    ptype_str = str(ptype)
    ptype = get_ptype_as_int(ptype)
    partitions = [p for p in table if p.type == ptype]

    if subtype is not None:
        ptype_str += f' ({subtype})'
        subtype = get_subtype_as_int(ptype, subtype)
        partitions = [p for p in partitions if p.subtype == subtype]

    if len(partitions) == 0:
        print(f'WARNING: Partition table does not contain any partitions matching {ptype_str}')
        return

    bin_name = os.path.basename(bin_file.name)
    bin_size = _file_size(bin_file)
    smallest_size = min(p.size for p in partitions)
    if smallest_size >= bin_size:
        free_size = smallest_size - bin_size
        free_size_relative = free_size / smallest_size
        print(
            f'{bin_name} binary size {bin_size:#x} bytes. Smallest {ptype_str} partition is '
            f'{smallest_size:#x} bytes. {free_size:#x} bytes ({free_size_relative:.0%}) free.'
        )
        free_size_relative_critical = 0.05
        if free_size_relative < free_size_relative_critical:
            print(
                f'Warning: The smallest {ptype_str} partition is nearly '
                f'full ({free_size_relative:.0%} free space left)!'
            )
        return

    too_small_partitions = [p for p in partitions if p.size < bin_size]
    if len(partitions) == 1:
        msg = f'{ptype_str} partition is'
    elif len(partitions) == len(too_small_partitions):
        msg = f'All {ptype_str} partitions are'
    else:
        msg = f'{len(too_small_partitions)}/{len(partitions)} {ptype_str} partitions are'
    msg += f' too small for binary {bin_name} size {bin_size:#x}:'
    for p in too_small_partitions:
        msg += f'\n  - {p} (overflow {bin_size - p.size:#x})'
    if not allow_failures and len(partitions) == len(too_small_partitions):
        # if some partitions can fit the binary then just print a warning
        raise SystemExit('Error: ' + msg)
    else:
        print('Warning: ' + msg)


def _file_type(mode: str):  # type: ignore
    """Open files for argparse arguments (argparse's own file factory is deprecated in Python 3.15)."""

    def _open(path: str):  # type: ignore
        if path == '-':
            stream = sys.stdin if 'r' in mode else sys.stdout
            return stream.buffer if 'b' in mode else stream
        try:
            return open(path, mode, encoding=None if 'b' in mode else 'utf-8')
        except OSError as e:
            raise argparse.ArgumentTypeError(f"can't open '{path}': {e}")

    return _open


def main():  # type: () -> None
    global allow_failures  # pylint: disable=global-statement

    parser = argparse.ArgumentParser(description='Check binary sizes against partition table entries')
    parser.add_argument('--target', choices=['esp32', 'esp32s2'])
    parser.add_argument(
        '--allow_failures', action='store_true', help='If true, failures will print warnings but not exit with an error'
    )
    parser.add_argument('--offset', '-o', help='Set partition table offset', default='0x8000')

    subparsers = parser.add_subparsers(
        dest='check_target', help='Type of binary to check against partition table layout'
    )
    sp_bootloader = subparsers.add_parser('bootloader')
    sp_bootloader.add_argument('bootloader_offset', help='Hex offset of bootloader in flash')
    sp_bootloader.add_argument(
        'bootloader_binary', type=_file_type('rb'), help='Bootloader binary (.bin) file from build output'
    )

    sp_part = subparsers.add_parser('partition')
    sp_part.add_argument(
        '--type', type=str, help='Check the file size against all partitions of this type.', required=True
    )
    sp_part.add_argument(
        '--subtype', type=str, help='Optional, only check the file size against all partitions of this subtype.'
    )
    sp_part.add_argument('partition_table', type=_file_type('rb'), help='Partition table file')
    sp_part.add_argument('binary', type=_file_type('rb'), help='Binary file which will have the size checked')

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
