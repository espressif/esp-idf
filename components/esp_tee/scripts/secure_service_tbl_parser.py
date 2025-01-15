# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import re
from typing import List
from typing import Tuple


def parse_services(secure_service_tbl: str) -> List[Tuple[int, str, int]]:
    services, service_ids = [], set()
    pattern = re.compile(r'^([0-9A-Fa-fXx]+)\s+\S+\s+(\S+)\s+(\d+)')

    with open(secure_service_tbl, 'r') as f:
        for line in f:
            if match := pattern.match(line):
                service_id = int(match.group(1), 0)
                if service_id in service_ids:
                    raise ValueError(f'Duplicate service call ID found: 0x{service_id:X}')
                service_ids.add(service_id)
                services.append((service_id, match.group(2), int(match.group(3))))

    return sorted(services, key=lambda x: x[0])


def generate_num_header(services: List[Tuple[int, str, int]], output_file: str) -> None:
    header = '''/**
  * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT!
  */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

'''
    body = '\n'.join(f'#define SS_{name.upper()}\t{nr}' for nr, name, _ in services)
    footer = f'\n#define MAX_SECURE_SERVICES_ID\t{services[-1][0] + 1 if services else 0}\n'
    footer += f'#define SECURE_SERVICES_NUM\t{len(services)}\n\n'
    footer += '''typedef void (*secure_service_t)(void);
typedef struct { int id; secure_service_t func; int nargs; } secure_service_entry_t;
'''
    footer += '\n#ifdef __cplusplus\n}\n#endif\n'
    with open(output_file, 'w') as f:
        f.write(header + body + footer)


def generate_dec_header(services: List[Tuple[int, str, int]], output_file: str) -> None:
    header = '''/**
  * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT!
  */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
'''
    body = '\n'.join(f'void _ss_{name}(void);' for _, name, _ in services)
    footer = '\n#ifdef __cplusplus\n}\n#endif\n'
    with open(output_file, 'w') as f:
        f.write(header + body + footer)


def generate_table(services: List[Tuple[int, str, int]], output_file: str) -> None:
    header = '''/**
  * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT!
  */

#pragma once
'''
    body = '\n'.join(f'__SECURE_SERVICE({nr}, {name}, {nargs})' for nr, name, nargs in services)
    with open(output_file, 'w') as f:
        f.write(header + body)


def generate_wrap_list(secure_service_tbl: str) -> None:
    pattern = re.compile(r'^[0-9A-Fa-fXx]+\s+IDF\s+(\S+)\s+\d+')
    with open(secure_service_tbl, 'r') as f:
        wrap_list = [f'-Wl,--wrap={match.group(1)}' for line in f if (match := pattern.match(line))]
    print(' '.join(wrap_list), end='')


def main() -> None:
    parser = argparse.ArgumentParser(description='Generate secure service outputs')
    parser.add_argument('--wrap', action='store_true', help='Generate linker wrap options')
    parser.add_argument('secure_service_tbl', type=str, help='Path to secure service table file')
    parser.add_argument('output_files', nargs='*', help='Output files: [secure_service_num.h, secure_service_dec.h, secure_service.h]')

    args = parser.parse_args()

    if args.wrap:
        generate_wrap_list(args.secure_service_tbl)
    else:
        if len(args.output_files) != 3:
            parser.error('Missing output header files!')
        services = parse_services(args.secure_service_tbl)
        generate_num_header(services, args.output_files[0])
        generate_dec_header(services, args.output_files[1])
        generate_table(services, args.output_files[2])


if __name__ == '__main__':
    main()
