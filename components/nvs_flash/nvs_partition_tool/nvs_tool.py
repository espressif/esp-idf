#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
from collections.abc import Callable
from typing import Any

import nvs_check
import nvs_logger
import nvs_parser
from esp_pylib.logger import log
from nvs_logger import nvs_log
from nvs_parser import nvs_const

_DUMP_CHOICES = [
    'all',
    'written',
    'minimal',
    'blobs',
    'namespaces',
    'storage_info',
    'none',
]
_DUMP_HELP = {
    'all': 'print written, erased and empty entries',
    'written': 'print only currently written entries',
    'minimal': 'print only namespace:key=value pairs',
    'blobs': 'print all blobs and strings',
    'namespaces': 'list all written namespaces',
    'storage_info': 'print storage related information (free/used entries, etc)',
    'none': 'do not print anything (if you only want to do integrity check)',
}
_FORMAT_CHOICES = ['text', 'json']


def _run(file: str, integrity_check: bool, dump: str, color: str, out_format: str) -> None:
    nvs_log.set_color(color)
    nvs_log.set_format(out_format)

    if nvs_const.entry_size != 32:
        log.die('Entry size is not 32B! This is currently non negotiable.')

    try:
        with open(file, 'rb') as f:
            partition = f.read()
    except FileNotFoundError:
        log.die(f'File not found: {file}')
    except OSError as e:
        log.die(f'Cannot read file {file}: {e}')

    try:
        nvs = nvs_parser.NVS_Partition(os.path.basename(file), bytearray(partition))
    except nvs_parser.NotAlignedError as e:
        log.die(str(e))
    except nvs_parser.NVS_Constants.ConstantError as e:
        log.die(str(e))

    def noop(_: nvs_parser.NVS_Partition) -> None:
        pass

    cmds: dict[str, Callable[[nvs_parser.NVS_Partition], None]] = {}
    if out_format == 'text':
        cmds = {
            'all': nvs_logger.dump_everything,
            'written': nvs_logger.dump_written_entries,
            'minimal': nvs_logger.dump_key_value_pairs,
            'namespaces': nvs_logger.list_namespaces,
            'blobs': nvs_logger.dump_written_blobs,
            'storage_info': nvs_logger.storage_stats,
            'none': noop,
        }
    elif out_format == 'json':
        cmds = {
            'all': nvs_logger.print_json,
            'minimal': nvs_logger.print_minimal_json,
            'none': noop,
        }
    else:
        log.die(f'{out_format} is not implemented')

    handler = cmds.get(dump)
    if handler is None:
        log.die(f'{dump} is not implemented for format {out_format}')
        return
    handler(nvs)

    if integrity_check:
        nvs_log.info()
        nvs_check.integrity_check(nvs, nvs_log)


def _build_cli() -> Any:
    import rich_click as click

    dump_help = os.linesep.join(f'{opt} - {_DUMP_HELP[opt]}' for opt in _DUMP_CHOICES)

    @click.command(
        context_settings={'help_option_names': ['-h', '--help']},
        help='Parse NVS partition',
    )
    @click.argument('file', type=click.Path(exists=True, dir_okay=False))
    @click.option(
        '-i',
        '--integrity-check',
        is_flag=True,
        help='Check partition for potential errors',
    )
    @click.option(
        '-d',
        '--dump',
        type=click.Choice(_DUMP_CHOICES),
        default='all',
        show_default=True,
        help=f'Dump type:{os.linesep}{dump_help}',
    )
    @click.option(
        '--color',
        type=click.Choice(['never', 'auto', 'always']),
        default='auto',
        show_default=True,
        help='Enable color output',
    )
    @click.option(
        '-f',
        '--format',
        'out_format',
        type=click.Choice(_FORMAT_CHOICES),
        default='text',
        show_default=True,
        help='Output format',
    )
    def cli(file: str, integrity_check: bool, dump: str, color: str, out_format: str) -> None:
        _run(file, integrity_check, dump, color, out_format)

    return cli


def main() -> None:
    _build_cli()()


if __name__ == '__main__':
    from esp_pylib.excepthook import install_exception_reporting

    install_exception_reporting()
    main()
