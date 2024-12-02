#!/usr/bin/env python
# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import re
import sys
from typing import Dict
from typing import Tuple


def emit(nr: int, entry: str, nargs: str) -> str:
    return f'__SECURE_SERVICE({nr}, {entry}, {nargs})'


def main() -> None:
    parser = argparse.ArgumentParser(description='Generate secure service table')
    parser.add_argument('input_file', type=str, help='Path to input file')
    parser.add_argument('output_file', type=str, help='Path to output file')
    args = parser.parse_args()

    services: Dict[int, Tuple[str, str, str]] = {}
    pattern: re.Pattern = re.compile(r'^([0-9A-Fa-fXx]+)\s+(\S+)\s+(\S+)\s+(\d+)')

    # Single pass through file to collect services and check duplicates
    with open(args.input_file, 'r') as f:
        for line in f:
            if match := pattern.match(line):
                nr = int(match.group(1))
                if nr in services:
                    print('ERROR: Found duplicate secure service numbers, exiting...')
                    sys.exit(1)
                services[nr] = (match.group(2), match.group(3), match.group(4))

    # Generate output
    with open(args.output_file, 'w') as f:
        f.write('''/**
  * This header file is used to define secure services.
  *
  * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT!
  */

#pragma once
''')
        for nr in sorted(services):
            _, name, nargs = services[nr]
            f.write(emit(nr, name, nargs) + '\n')


if __name__ == '__main__':
    main()
