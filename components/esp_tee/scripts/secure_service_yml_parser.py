# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
from typing import List
from typing import Set
from typing import Tuple

import yaml

SEC_SRV_TABLE_SPLIT_ID = 30
OUTPUT_HEADERS = [
    'secure_service_num.h',
    'secure_service_dec.h',
    'secure_service_int.h',
    'secure_service_ext.h',
]


def parse_services(yml_files: List[str], excluded_fam: Set[str]) -> List[Tuple[int, str, int]]:
    services, service_ids = [], set()

    for yml_file in yml_files:
        with open(yml_file, 'r') as f:
            data = yaml.safe_load(f)
            for family in data.get('secure_services', []):
                family_name = family.get('family', '')
                if family_name in excluded_fam:
                    continue
                for entry in family.get('entries', []):
                    service_id = entry['id']
                    if service_id in service_ids:
                        raise ValueError(f'Duplicate service call ID found: 0x{service_id:X}')
                    service_ids.add(service_id)
                    services.append((service_id, entry['function'], entry['args']))

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
    footer += f'#define SECURE_SERVICES_SPLIT_ID\t{SEC_SRV_TABLE_SPLIT_ID}\n\n'
    footer += '''typedef void (*secure_service_t)(void);
typedef struct { secure_service_t func; int nargs; } secure_service_entry_t;
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


def generate_table_split(services: List[Tuple[int, str, int]], output_file_1: str, output_file_2: str) -> None:
    header = '''/**
  * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT!
  */

#pragma once
'''
    services_1 = [service for service in services if service[0] < SEC_SRV_TABLE_SPLIT_ID]
    services_2 = [service for service in services if service[0] >= SEC_SRV_TABLE_SPLIT_ID]

    body_1 = '\n'.join(f'__SECURE_SERVICE({nr}, {name}, {nargs})' for nr, name, nargs in services_1)
    body_2 = '\n'.join(f'__SECURE_SERVICE({nr}, {name}, {nargs})' for nr, name, nargs in services_2)

    with open(output_file_1, 'w') as f1, open(output_file_2, 'w') as f2:
        f1.write(header + body_1)
        f2.write(header + body_2)


def generate_wrap_list(yml_files: List[str], excluded_fam: Set[str]) -> None:
    wrap_list: list[str] = []
    for yml_file in yml_files:
        with open(yml_file, 'r') as f:
            data = yaml.safe_load(f)
            wrap_list.extend(
                f'-Wl,--wrap={entry["function"]}'
                for family in data.get('secure_services', [])
                for entry in family.get('entries', [])
                if entry['type'] == 'IDF' and family.get('family', '') not in excluded_fam
            )
    print(' '.join(wrap_list), end='')


def main() -> None:
    parser = argparse.ArgumentParser(description='Generate secure service outputs from YAML table')
    parser.add_argument('--wrap', action='store_true', help='Generate linker wrap options')
    parser.add_argument('-s', '--sec_srv', nargs='+', required=True, help='Secure service table(s) in YAML')
    parser.add_argument('--exclude', nargs='*', default=[], help='List of API families to exclude from the output')

    args = parser.parse_args()
    excluded_fam = set(args.exclude)

    if args.wrap:
        generate_wrap_list(args.sec_srv, excluded_fam)
    else:
        services = parse_services(args.sec_srv, excluded_fam)
        generate_num_header(services, OUTPUT_HEADERS[0])
        generate_dec_header(services, OUTPUT_HEADERS[1])
        generate_table_split(services, OUTPUT_HEADERS[2], OUTPUT_HEADERS[3])


if __name__ == '__main__':
    main()
