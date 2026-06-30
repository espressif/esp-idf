#!/usr/bin/env python
#
# generates an empty binary file
#
# This tool generates an empty binary file of the required size.
#
# SPDX-FileCopyrightText: 2018-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0


import sys

import rich_click as click
from esp_pylib.cli_types import AnyIntType

__version__ = '1.0'


def generate_blanked_file(size, output_path):
    output = b'\xff' * size
    try:
        stdout_binary = sys.stdout.buffer  # Python 3
    except AttributeError:
        stdout_binary = sys.stdout
    with stdout_binary if output_path == '-' else open(output_path, 'wb') as f:
        f.write(output)


@click.command(
    context_settings={'help_option_names': ['-h', '--help']},
    help='Generates an empty binary file of the required size.',
)
@click.argument('size', type=AnyIntType(), metavar='SIZE')
@click.argument('output', type=click.Path(), required=False, default='-')
def cli(size, output):
    if size > 0:
        generate_blanked_file(size, output)


def main():
    cli()


if __name__ == '__main__':
    from esp_pylib.excepthook import install_exception_reporting

    install_exception_reporting()
    main()
