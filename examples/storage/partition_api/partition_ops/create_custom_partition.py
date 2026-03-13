#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
"""Create a 4kiB binary file with 'abcdef123456' at the start, rest filled with zeros."""

import argparse
import os

FILE_NAME = 'custom_partition.bin'
FILE_SIZE = 4 * 1024  # 4 kiB
HEADER = b'abcdef123456'


def main() -> None:
    parser = argparse.ArgumentParser(description='Generate a custom partition binary file.')
    parser.add_argument(
        'output_path',
        nargs='?',
        default=None,
        help='Path for the output binary file. Defaults to custom_partition.bin in the script directory.',
    )
    args = parser.parse_args()

    if args.output_path:
        output_path = args.output_path
    else:
        script_dir = os.path.dirname(os.path.abspath(__file__))
        output_path = os.path.join(script_dir, FILE_NAME)

    data = HEADER + b'\x00' * (FILE_SIZE - len(HEADER))

    with open(output_path, 'wb') as f:
        f.write(data)

    print(f'Created {output_path} ({FILE_SIZE} bytes)')


if __name__ == '__main__':
    main()
