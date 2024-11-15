#!/usr/bin/env python
# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import re
from typing import List
from typing import Tuple


def parse_services(secure_service_tbl: str) -> List[Tuple[str, str, str]]:
    services: List[Tuple[str, str, str]] = []
    pattern: re.Pattern = re.compile(r'^([0-9A-Fa-fXx]+)\s+\S+\s+(\S+)\s+(\d+)')
    with open(secure_service_tbl, 'r') as f:
        for line in f:
            if match := pattern.match(line):
                services.append((match.group(1), match.group(2), match.group(3)))
    return sorted(services, key=lambda x: int(x[0]))


def generate_num_header(services: List[Tuple[str, str, str]], output_file: str) -> None:
    header_text: str = '''/**
  * This header file is used to generate secure service number macros.
  *
  * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT!
  */

#pragma once
'''
    with open(output_file, 'w') as f:
        f.write(header_text)
        for nr, name, _ in services:
            f.write(f'#define SS_{name.upper()}\t{nr}\n')
        total: int = int(services[-1][0]) + 1 if services else 0
        f.write(f'\n#define MAX_SECURE_SERVICES\t{total}\n\n')


def generate_dec_header(services: List[Tuple[str, str, str]], output_file: str) -> None:
    header_text: str = '''/**
  * This header file is used to provide function declarations
  * for compiling secure_service_table.c source file. Please do not
  * use it in application.
  *
  * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT!
  */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
'''
    with open(output_file, 'w') as f:
        f.write(header_text)
        for _, name, _ in services:
            f.write(f'void _ss_{name}(void);\n')
        f.write('''
#ifdef __cplusplus
}
#endif
''')


def main() -> None:
    parser = argparse.ArgumentParser(description='Generate secure service headers')
    parser.add_argument('secure_service_tbl', type=str, help='Path to secure_service.tbl generated in build directory')
    parser.add_argument('secure_service_num_h', type=str, help='Path to secure_service_num.h header file')
    parser.add_argument('secure_service_dec_h', type=str, help='Path to secure_service_dec.h header file')
    args = parser.parse_args()

    services: List[Tuple[str, str, str]] = parse_services(args.secure_service_tbl)
    generate_num_header(services, args.secure_service_num_h)
    generate_dec_header(services, args.secure_service_dec_h)


if __name__ == '__main__':
    main()
