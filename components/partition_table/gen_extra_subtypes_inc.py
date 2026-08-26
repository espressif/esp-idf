#!/usr/bin/env python
# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import rich_click as click
from esp_pylib.logger import log


def gen_header_file(path: str, subtypes: str) -> None:
    HDR_MESSAGE = '/* Automatically generated file. DO NOT EDIT. */\n\n'
    PARTTOOL_USAGE = 'If you want to use parttool.py manually, please use the following as an extra argument:'
    with open(path, 'w', encoding='utf-8') as f:
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
                f.write(f'ESP_PARTITION_SUBTYPE_{fields[0].upper()}_{fields[1].upper()} = {fields[2]},\n')
            except ValueError as err:
                log.die(f'Error parsing custom subtypes: {err}')


@click.command(
    context_settings={'help_option_names': ['-h', '--help']},
    help='ESP32 extra partitions utility',
)
@click.argument('config_dir', type=click.Path())
@click.argument('extra_partition_subtypes', nargs=-1)
def cli(config_dir, extra_partition_subtypes):  # type: ignore[no-untyped-def]
    gen_header_file(config_dir, extra_partition_subtypes)


def main() -> None:
    cli()


if __name__ == '__main__':
    from esp_pylib.excepthook import install_exception_reporting

    install_exception_reporting()
    main()
