#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
# Module was moved to the esptool in ESP-IDF v5.2 and relicensed under GPL v2.0 license.

from __future__ import division

import argparse
import json
import os
import subprocess
import sys


def main() -> None:
    parser = argparse.ArgumentParser()

    def parse_chip_id(string: str) -> str:
        # compatibility layer with old script
        print("DEPRECATED option '--chip-id'. Please consider using '--chip' instead")
        # DO NOT add new IDs; they are now maintained in esptool.
        ids = {
            0x1c5f21b0: 'esp32',
            0xbfdd4eee: 'esp32s2',
            0xd42ba06c: 'esp32c3',
            0xc47e5767: 'esp32s3',
            0x332726f6: 'esp32h2',
            0x2b88d29c: 'esp32c2',
            0x540ddf62: 'esp32c6',
            0x3d308e94: 'esp32p4',
        }
        try:
            return ids[int(string, 16)]
        except KeyError:
            raise argparse.ArgumentTypeError('Unknown Chip ID')

    # Provision to add "info" command
    subparsers = parser.add_subparsers(dest='command')
    write_parser = subparsers.add_parser('write')
    write_parser.add_argument('-o', '--output-file',
                              help='Filename for storing the output UF2 image',
                              required=True)
    group = write_parser.add_mutually_exclusive_group(required=True)
    # chip-id used just for backwards compatibility, UF2 family IDs are now stored in esptool
    group.add_argument('--chip-id',
                       type=parse_chip_id,
                       help=argparse.SUPPRESS)
    group.add_argument('--chip',
                       type=str,
                       help='Target chip type')
    write_parser.add_argument('--chunk-size',
                              required=False,
                              type=int,
                              default=None,
                              help='Specify the used data part of the 512 byte UF2 block. A common value is 256. By '
                                   'default the largest possible value will be used.')
    write_parser.add_argument('--json',
                              help='Optional file for loading "flash_files" dictionary with <address> <file> items')
    write_parser.add_argument('--bin',
                              help='Use only a subset of binaries from the JSON file, e.g. "partition_table '
                                   'bootloader app"',
                              nargs='*')
    write_parser.add_argument('--md5-disable',
                              help='Disable MD5 checksum. Useful for compatibility with e.g. TinyUF2',
                              action='store_true')
    write_parser.add_argument('files',
                              metavar='<address> <file>', help='Add <file> at <address>',
                              nargs='*')

    args = parser.parse_args()

    def check_file(file_name: str) -> str:
        if not os.path.isfile(file_name):
            raise RuntimeError('{} is not a regular file!'.format(file_name))
        return file_name

    files = []
    if args.files:
        files += [(addr, check_file(f_name)) for addr, f_name in zip(args.files[::2], args.files[1::2])]

    if args.json:
        json_dir = os.path.dirname(os.path.abspath(args.json))

        def process_json_file(path: str) -> str:
            '''
            The input path is relative to json_dir. This function makes it relative to the current working
            directory.
            '''
            return check_file(os.path.abspath(os.path.join(json_dir, path)))

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

        files += [(addr, process_json_file(f_name)) for addr, f_name in flash_dic.items()]

    # remove possible duplicates and sort based on the address
    files = sorted([(addr, f_name) for addr, f_name in dict(files).items()], key=lambda x: x[0])  # type: ignore

    # list of tuples to simple list
    files = [item for t in files for item in t]

    cmd = [
        sys.executable, '-m', 'esptool',
        '--chip', args.chip_id or args.chip,
        'merge_bin',
        '--format', 'uf2',
        '-o', args.output_file,
    ]
    if args.chunk_size:
        cmd.extend(['--chunk_size', args.chunk_size])

    if args.md5_disable:
        cmd.append('--md5-disable')

    cmd_str = ' '.join(cmd + files)
    print(f'Executing: {cmd_str}')

    sys.exit(subprocess.run(cmd + files).returncode)


if __name__ == '__main__':
    main()
