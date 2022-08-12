#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import division

import argparse
import hashlib
import json
import os
import struct
from functools import partial
from typing import Dict, List


def round_up_int_div(n: int, d: int) -> int:
    # equivalent to math.ceil(n / d)
    return (n + d - 1) // d


class UF2Writer(object):

    # The UF2 format is described here: https://github.com/microsoft/uf2
    UF2_BLOCK_SIZE = 512
    UF2_DATA_SIZE = 476  # max value of CHUNK_SIZE reduced by optional parts. Currently, MD5_PART only.
    UF2_MD5_PART_SIZE = 24
    UF2_FIRST_MAGIC = 0x0A324655
    UF2_SECOND_MAGIC = 0x9E5D5157
    UF2_FINAL_MAGIC = 0x0AB16F30
    UF2_FLAG_FAMILYID_PRESENT = 0x00002000
    UF2_FLAG_MD5_PRESENT = 0x00004000

    def __init__(self, chip_id: int, output_file: os.PathLike, chunk_size: int) -> None:
        self.chip_id = chip_id
        self.CHUNK_SIZE = self.UF2_DATA_SIZE - self.UF2_MD5_PART_SIZE if chunk_size is None else chunk_size
        self.f = open(output_file, 'wb')

    def __enter__(self) -> 'UF2Writer':
        return self

    def __exit__(self, exc_type: str, exc_val: int, exc_tb: List) -> None:
        if self.f:
            self.f.close()

    @staticmethod
    def _to_uint32(num: int) -> bytes:
        return struct.pack('<I', num)

    def _write_block(self, addr: int, chunk: bytes, len_chunk: int, block_no: int, blocks: int) -> None:
        assert len_chunk > 0
        assert len_chunk <= self.CHUNK_SIZE
        assert block_no < blocks
        block = self._to_uint32(self.UF2_FIRST_MAGIC)
        block += self._to_uint32(self.UF2_SECOND_MAGIC)
        block += self._to_uint32(self.UF2_FLAG_FAMILYID_PRESENT | self.UF2_FLAG_MD5_PRESENT)
        block += self._to_uint32(addr)
        block += self._to_uint32(len_chunk)
        block += self._to_uint32(block_no)
        block += self._to_uint32(blocks)
        block += self._to_uint32(self.chip_id)
        block += chunk

        md5_part = self._to_uint32(addr)
        md5_part += self._to_uint32(len_chunk)
        md5_part += hashlib.md5(chunk).digest()
        assert len(md5_part) == self.UF2_MD5_PART_SIZE

        block += md5_part
        block += b'\x00' * (self.UF2_DATA_SIZE - self.UF2_MD5_PART_SIZE - len_chunk)
        block += self._to_uint32(self.UF2_FINAL_MAGIC)
        assert len(block) == self.UF2_BLOCK_SIZE
        self.f.write(block)

    def add_file(self, addr: int, f_path: os.PathLike) -> None:
        blocks = round_up_int_div(os.path.getsize(f_path), self.CHUNK_SIZE)
        with open(f_path, 'rb') as fin:
            a = addr
            for i, chunk in enumerate(iter(partial(fin.read, self.CHUNK_SIZE), b'')):
                len_chunk = len(chunk)
                self._write_block(a, chunk, len_chunk, i, blocks)
                a += len_chunk


def action_write(args: Dict) -> None:
    with UF2Writer(args['chip_id'], args['output_file'], args['chunk_size']) as writer:
        for addr, f in args['files']:
            print('Adding {} at {:#x}'.format(f, addr))
            writer.add_file(addr, f)
    print('"{}" has been written.'.format(args['output_file']))


def main() -> None:
    parser = argparse.ArgumentParser()

    def four_byte_aligned(integer: int) -> bool:
        return integer & 3 == 0

    def parse_chunk_size(string: str) -> int:
        num = int(string, 0)
        if not four_byte_aligned(num):
            raise argparse.ArgumentTypeError('Chunk size should be a 4-byte aligned number')
        return num

    def parse_chip_id(string: str) -> int:
        num = int(string, 16)
        if num < 0 or num > 0xFFFFFFFF:
            raise argparse.ArgumentTypeError('Chip ID should be a 4-byte unsigned integer')
        return num

    # Provision to add "info" command
    subparsers = parser.add_subparsers(dest='command')
    write_parser = subparsers.add_parser('write')
    write_parser.add_argument('-o', '--output-file',
                              help='Filename for storing the output UF2 image',
                              required=True)
    write_parser.add_argument('--chip-id',
                              required=True,
                              type=parse_chip_id,
                              help='Hexa-decimal chip identificator')
    write_parser.add_argument('--chunk-size',
                              required=False,
                              type=parse_chunk_size,
                              default=None,
                              help='Specify the used data part of the 512 byte UF2 block. A common value is 256. By '
                                   'default the largest possible value will be used.')
    write_parser.add_argument('--json',
                              help='Optional file for loading "flash_files" dictionary with <address> <file> items')
    write_parser.add_argument('--bin',
                              help='Use only a subset of binaries from the JSON file, e.g. "partition_table '
                                   'bootloader app"',
                              nargs='*')
    write_parser.add_argument('files',
                              metavar='<address> <file>', help='Add <file> at <address>',
                              nargs='*')

    args = parser.parse_args()

    def check_file(file_name: str) -> str:
        if not os.path.isfile(file_name):
            raise RuntimeError('{} is not a regular file!'.format(file_name))
        return file_name

    def parse_addr(string: str) -> int:
        num = int(string, 0)
        if not four_byte_aligned(num):
            raise RuntimeError('{} is not a 4-byte aligned valid address'.format(string))
        return num

    files = []
    if args.files:
        files += [(parse_addr(addr), check_file(f_name)) for addr, f_name in zip(args.files[::2], args.files[1::2])]

    if args.json:
        json_dir = os.path.dirname(os.path.abspath(args.json))

        def process_json_file(path: str) -> str:
            '''
            The input path is relative to json_dir. This function makes it relative to the current working
            directory.
            '''
            return check_file(os.path.relpath(os.path.join(json_dir, path), start=os.curdir))

        with open(args.json) as f:
            json_content = json.load(f)

        if args.bin:
            try:
                bin_selection = [json_content[b] for b in args.bin]
                flash_dic = dict((x['offset'], x['file']) for x in bin_selection)
            except KeyError:
                print('Invalid binary was selected.')
                valid = [k if all(x in v for x in ('offset', 'file')) else None for k, v in json_content.items()]
                print('Valid ones:', ' '.join(x for x in valid if x))
                exit(1)
        else:
            flash_dic = json_content['flash_files']

        files += [(parse_addr(addr), process_json_file(f_name)) for addr, f_name in flash_dic.items()]

    files = sorted([(addr, f_name) for addr, f_name in dict(files).items()],
                   key=lambda x: x[0])  # remove possible duplicates and sort based on the address

    cmd_args = {'output_file': args.output_file,
                'files': files,
                'chip_id': args.chip_id,
                'chunk_size': args.chunk_size,
                }

    {'write': action_write
     }[args.command](cmd_args)


if __name__ == '__main__':
    main()
