#!/usr/bin/env python
# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse


def gen_header_file(path: str, subtypes: str) -> None:
    HDR_MESSAGE = '/* Automatically generated file. DO NOT EDIT. */\n\n'
    PARTTOOL_USAGE = 'If you want to use parttool.py manually, please use the following as an extra argument:'
    with open(path, 'w') as f:
        f.write(HDR_MESSAGE)
        if subtypes:
            f.write('/*\n\t' + PARTTOOL_USAGE + '\n\t')
            f.write('--extra-partition-subtypes ')
            for line_no in subtypes:
                f.write(f'"{line_no}" ')
            f.write('\n*/\n\n')
        f.write('#pragma once\n\n')
        for line_no in subtypes:
            try:
                fields = [line.strip() for line in line_no.split(',')]
                fields[0] = fields[0].strip()
                fields[1] = fields[1].strip()
                fields[2] = fields[2].strip()
                f.write('ESP_PARTITION_SUBTYPE_%s_%s = %s,\n' % (fields[0].upper(), fields[1].upper(), fields[2]))
            except ValueError as err:
                raise ValueError('Error parsing custom subtypes: %s' % err)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='ESP32 extra partitions utility')
    parser.add_argument('config_dir', help='Path to config directory')
    parser.add_argument('extra_partition_subtypes', help='Extra partition subtype entries', nargs='*')
    args = parser.parse_args()

    gen_header_file(args.config_dir, args.extra_partition_subtypes)
