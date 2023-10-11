#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os
import sys
import traceback

import nvs_check
import nvs_logger
import nvs_parser
from nvs_logger import nvs_log
from nvs_parser import nvs_const


def program_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description='Parse NVS partition', formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument('file', help='Path to dumped NVS partition')
    parser.add_argument(
        '-i',
        '--integrity-check',
        action='store_true',
        help='check partition for potential errors',
    )
    tmp = {
        'all': 'print everything',
        'written': 'print only currently written entries',
        'minimal': 'print only namespace:key=value pairs',
        'namespaces': 'list all written namespaces',
        'blobs': 'print all blobs and strings',
        'storage_info': 'print storage related information (free/used entries, etc)',
        'none': 'do not print anything (if you only want to do integrity check)',
    }
    parser.add_argument(
        '-d',
        '--dump',
        choices=tmp,
        default='all',
        metavar='type',
        help=(
            f"""type: {str(list(tmp.keys()))[1:-1]}
{os.linesep.join([f'{opt} - {tmp[opt]}' for opt in tmp])}"""
        ),
    )
    parser.add_argument(
        '--color',
        choices=['never', 'auto', 'always'],
        default='auto',
        help='Enable color (ANSI)',
    )
    tmp = {
        'text': 'print output as a human-readable text',
        'json': 'print output as JSON and exit',
    }
    parser.add_argument(
        '-f', '--format', choices=tmp, default='text', help='Output format'
    )
    return parser.parse_args()


def main() -> None:
    args = program_args()

    if nvs_const.entry_size != 32:
        raise ValueError(f'Entry size is not 32B! This is currently non negotiable.')

    nvs_log.set_color(args.color)
    nvs_log.set_format(args.format)

    try:
        with open(args.file, 'rb') as f:
            partition = f.read()
    except IndexError:
        nvs_log.error('No file given')
        raise
    except FileNotFoundError:
        nvs_log.error('Bad filename')
        raise

    nvs = nvs_parser.NVS_Partition(args.file.split('/')[-1], bytearray(partition))

    def noop(_: nvs_parser.NVS_Partition) -> None:
        pass

    def format_not_implemented(_: nvs_parser.NVS_Partition) -> None:
        raise RuntimeError(f'{args.format} is not implemented')

    def cmd_not_implemented(_: nvs_parser.NVS_Partition) -> None:
        raise RuntimeError(f'{args.dump} is not implemented')

    formats = {
        'text': noop,
        'json': nvs_logger.print_json,
    }
    formats.get(args.format, format_not_implemented)(nvs)

    if args.format == 'text':
        cmds = {
            'all': nvs_logger.dump_everything,
            'written': nvs_logger.dump_written_entries,
            'minimal': nvs_logger.dump_key_value_pairs,
            'namespaces': nvs_logger.list_namespaces,
            'blobs': nvs_logger.dump_written_blobs,
            'storage_info': nvs_logger.storage_stats,
            'none': noop,
        }
        cmds.get(args.dump, cmd_not_implemented)(nvs)  # type: ignore

        if args.integrity_check:
            nvs_log.info()
            nvs_check.integrity_check(nvs, nvs_log)


if __name__ == '__main__':
    try:
        main()
    except ValueError:
        traceback.print_exc(file=sys.stderr)
        sys.exit(1)
    except nvs_parser.NVS_Constants.ConstantError:
        traceback.print_exc(file=sys.stderr)
        sys.exit(1)
