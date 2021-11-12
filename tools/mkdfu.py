#!/usr/bin/env python
#
# Copyright 2020 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# This program creates archives compatible with ESP32-S* ROM DFU implementation.
#
# The archives are in CPIO format. Each file which needs to be flashed is added to the archive
# as a separate file. In addition to that, a special index file, 'dfuinfo0.dat', is created.
# This file must be the first one in the archive. It contains binary structures describing each
# subsequent file (for example, where the file needs to be flashed/loaded).

import argparse
import hashlib
import json
import os
import struct
import zlib
from collections import namedtuple

from future.utils import iteritems

try:
    import typing
except ImportError:
    # Only used for type annotations
    pass

try:
    from itertools import izip as zip
except ImportError:
    # Python 3
    pass

# CPIO ("new ASCII") format related things
CPIO_MAGIC = b'070701'
CPIO_STRUCT = b'=6s' + b'8s' * 13
CPIOHeader = namedtuple(
    'CPIOHeader',
    [
        'magic',
        'ino',
        'mode',
        'uid',
        'gid',
        'nlink',
        'mtime',
        'filesize',
        'devmajor',
        'devminor',
        'rdevmajor',
        'rdevminor',
        'namesize',
        'check',
    ],
)
CPIO_TRAILER = 'TRAILER!!!'


def make_cpio_header(
    filename_len, file_len, is_trailer=False
):  # type: (int, int, bool) -> CPIOHeader
    """ Returns CPIOHeader for the given file name and file size """

    def as_hex(val):  # type: (int) -> bytes
        return '{:08x}'.format(val).encode('ascii')

    hex_0 = as_hex(0)
    mode = hex_0 if is_trailer else as_hex(0o0100644)
    nlink = as_hex(1) if is_trailer else hex_0
    return CPIOHeader(
        magic=CPIO_MAGIC,
        ino=hex_0,
        mode=mode,
        uid=hex_0,
        gid=hex_0,
        nlink=nlink,
        mtime=hex_0,
        filesize=as_hex(file_len),
        devmajor=hex_0,
        devminor=hex_0,
        rdevmajor=hex_0,
        rdevminor=hex_0,
        namesize=as_hex(filename_len),
        check=hex_0,
    )


# DFU format related things
# Structure of one entry in dfuinfo0.dat
DFUINFO_STRUCT = b'<I I 64s 16s'
DFUInfo = namedtuple('DFUInfo', ['address', 'flags', 'name', 'md5'])
DFUINFO_FILE = 'dfuinfo0.dat'
# Structure which gets added at the end of the entire DFU file
DFUSUFFIX_STRUCT = b'<H H H H 3s B'
DFUSuffix = namedtuple(
    'DFUSuffix', ['bcd_device', 'pid', 'vid', 'bcd_dfu', 'sig', 'len']
)
ESPRESSIF_VID = 12346
# This CRC32 gets added after DFUSUFFIX_STRUCT
DFUCRC_STRUCT = b'<I'


def dfu_crc(data, crc=0):  # type: (bytes, int) -> int
    """ Calculate CRC32/JAMCRC of data, with an optional initial value """
    uint32_max = 0xFFFFFFFF
    return uint32_max - (zlib.crc32(data, crc) & uint32_max)


def pad_bytes(b, multiple, padding=b'\x00'):  # type: (bytes, int, bytes) -> bytes
    """ Pad 'b' to a length divisible by 'multiple' """
    padded_len = (len(b) + multiple - 1) // multiple * multiple
    return b + padding * (padded_len - len(b))


class EspDfuWriter(object):
    def __init__(self, dest_file, pid):  # type: (typing.BinaryIO) -> None
        self.dest = dest_file
        self.pid = pid
        self.entries = []  # type: typing.List[bytes]
        self.index = []  # type: typing.List[DFUInfo]

    def add_file(self, flash_addr, path):  # type: (int, str) -> None
        """ Add file to be written into flash at given address """
        with open(path, 'rb') as f:
            self._add_cpio_flash_entry(os.path.basename(path), flash_addr, f.read())

    def finish(self):  # type: () -> None
        """ Write DFU file """
        # Prepare and add dfuinfo0.dat file
        dfuinfo = b''.join([struct.pack(DFUINFO_STRUCT, *item) for item in self.index])
        self._add_cpio_entry(DFUINFO_FILE, dfuinfo, first=True)

        # Add CPIO archive trailer
        self._add_cpio_entry(CPIO_TRAILER, b'', trailer=True)

        # Combine all the entries and pad the file
        out_data = b''.join(self.entries)
        cpio_block_size = 10240
        out_data = pad_bytes(out_data, cpio_block_size)

        # Add DFU suffix and CRC
        dfu_suffix = DFUSuffix(0xFFFF, self.pid, ESPRESSIF_VID, 0x0100, b'UFD', 16)
        out_data += struct.pack(DFUSUFFIX_STRUCT, *dfu_suffix)
        out_data += struct.pack(DFUCRC_STRUCT, dfu_crc(out_data))

        # Finally write the entire binary
        self.dest.write(out_data)

    def _add_cpio_flash_entry(
        self, filename, flash_addr, data
    ):  # type: (str, int, bytes) -> None
        md5 = hashlib.md5()
        md5.update(data)
        self.index.append(
            DFUInfo(
                address=flash_addr,
                flags=0,
                name=filename.encode('utf-8'),
                md5=md5.digest(),
            )
        )
        self._add_cpio_entry(filename, data)

    def _add_cpio_entry(
        self, filename, data, first=False, trailer=False
    ):  # type: (str, bytes, bool, bool) -> None
        filename_b = filename.encode('utf-8') + b'\x00'
        cpio_header = make_cpio_header(len(filename_b), len(data), is_trailer=trailer)
        entry = pad_bytes(
            struct.pack(CPIO_STRUCT, *cpio_header) + filename_b, 4
        ) + pad_bytes(data, 4)
        if not first:
            self.entries.append(entry)
        else:
            self.entries.insert(0, entry)


def action_write(args):
    writer = EspDfuWriter(args['output_file'], args['pid'])
    for addr, f in args['files']:
        print('Adding {} at {:#x}'.format(f, addr))
        writer.add_file(addr, f)
    writer.finish()
    print('"{}" has been written. You may proceed with DFU flashing.'.format(args['output_file'].name))


def main():
    parser = argparse.ArgumentParser()

    # Provision to add "info" command
    subparsers = parser.add_subparsers(dest='command')
    write_parser = subparsers.add_parser('write')
    write_parser.add_argument('-o', '--output-file',
                              help='Filename for storing the output DFU image',
                              required=True,
                              type=argparse.FileType('wb'))
    write_parser.add_argument('--pid',
                              required=True,
                              type=lambda h: int(h, 16),
                              help='Hexa-decimal product indentificator')
    write_parser.add_argument('--json',
                              help='Optional file for loading "flash_files" dictionary with <address> <file> items')
    write_parser.add_argument('files',
                              metavar='<address> <file>', help='Add <file> at <address>',
                              nargs='*')

    args = parser.parse_args()

    def check_file(file_name):
        if not os.path.isfile(file_name):
            raise RuntimeError('{} is not a regular file!'.format(file_name))
        return file_name

    files = []
    if args.files:
        files += [(int(addr, 0), check_file(f_name)) for addr, f_name in zip(args.files[::2], args.files[1::2])]

    if args.json:
        json_dir = os.path.dirname(os.path.abspath(args.json))

        def process_json_file(path):
            '''
            The input path is relative to json_dir. This function makes it relative to the current working
            directory.
            '''
            return check_file(os.path.relpath(os.path.join(json_dir, path), start=os.curdir))

        with open(args.json) as f:
            files += [(int(addr, 0),
                       process_json_file(f_name)) for addr, f_name in iteritems(json.load(f)['flash_files'])]

    files = sorted([(addr, f_name) for addr, f_name in iteritems(dict(files))],
                   key=lambda x: x[0])  # remove possible duplicates and sort based on the address

    cmd_args = {'output_file': args.output_file,
                'files': files,
                'pid': args.pid,
                }

    {'write': action_write
     }[args.command](cmd_args)


if __name__ == '__main__':
    main()
