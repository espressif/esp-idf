#!/usr/bin/env python
#
# check_sizes.py is a tool run by the ESP-IDF build system
# to check a particular binary fits in the available partitions of
# a particular type/subtype. Can be used to check if the app binary fits in
# all available app partitions, for example.
#
# (Can also check if the bootloader binary fits before the partition table.)
#
# SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0


import io  # noqa: F401 # pylint: disable=unused-import
import os

try:
    from typing import IO  # noqa: F401 # pylint: disable=unused-import
except ImportError:
    pass  # used for type hinting only

import gen_esp32part
import rich_click as click
from esp_pylib.cli_types import AnyIntType
from esp_pylib.logger import log
from gen_esp32part import APP_TYPE
from gen_esp32part import MIN_PARTITION_SUBTYPE_APP_TEE
from gen_esp32part import NUM_PARTITION_SUBTYPE_APP_TEE
from gen_esp32part import PartitionTable
from gen_esp32part import get_ptype_as_int
from gen_esp32part import get_subtype_as_int

allow_failures = False


def _file_size(f):  # type: (IO) -> int
    before = f.tell()
    f.seek(0, 2)  # seek to end
    result = f.tell()
    f.seek(before)
    return result


def _fail(msg):  # type: (str) -> None
    if allow_failures:
        log.warn(msg)
    else:
        log.err(msg)
        raise SystemExit(msg)


def check_bootloader(partition_table_offset, bootloader_offset, binary_file):  # type: (int, int, IO) -> None
    max_size = partition_table_offset - bootloader_offset
    bootloader_size = _file_size(binary_file)
    if bootloader_size > max_size:
        msg = (
            'Bootloader binary size {:#x} bytes is too large for partition table offset {:#02x}. '
            + 'Bootloader binary can be maximum {:#x} ({}) bytes unless the partition table offset '
            + 'is increased in the Partition Table section of the project configuration menu.'
        ).format(bootloader_size, partition_table_offset, max_size, max_size)
        _fail(msg)
    free_size = max_size - bootloader_size
    pct = round(free_size * 100 / max_size)
    log.print(f'Bootloader binary size {bootloader_size:#x} bytes. {free_size:#x} bytes ({pct}%) free.')


def check_partition(ptype, subtype, partition_table_file, bin_file):  # type: (str, str, io.IOBase, IO) -> None
    table, _ = PartitionTable.from_file(partition_table_file)
    ptype_str = str(ptype)
    ptype = get_ptype_as_int(ptype)
    partitions = [p for p in table if p.type == ptype]
    tee_subtypes = range(MIN_PARTITION_SUBTYPE_APP_TEE, MIN_PARTITION_SUBTYPE_APP_TEE + NUM_PARTITION_SUBTYPE_APP_TEE)

    if subtype is not None:
        ptype_str += f' ({subtype})'
        subtype = get_subtype_as_int(ptype, subtype)
        partitions = [p for p in partitions if p.subtype == subtype]
    elif ptype == APP_TYPE:
        partitions = [p for p in partitions if p.subtype not in tee_subtypes]

    if len(partitions) == 0:
        log.warn(f'Partition table does not contain any partitions matching {ptype_str}')
        return

    bin_name = os.path.basename(bin_file.name)
    bin_size = _file_size(bin_file)
    smallest_size = min(p.size for p in partitions)
    if smallest_size >= bin_size:
        free_size = smallest_size - bin_size
        free_size_relative = free_size / smallest_size
        log.print(
            f'{bin_name} binary size {bin_size:#x} bytes. Smallest {ptype_str} partition is '
            f'{smallest_size:#x} bytes. {free_size:#x} bytes ({free_size_relative:.0%}) free.'
        )
        free_size_relative_critical = 0.05
        if free_size_relative < free_size_relative_critical:
            log.warn(f'The smallest {ptype_str} partition is nearly full ({free_size_relative:.0%} free space left)!')
        return

    too_small_partitions = [p for p in partitions if p.size < bin_size]
    if len(partitions) == 1:
        msg = f'{ptype_str} partition is'
    elif len(partitions) == len(too_small_partitions):
        msg = f'All {ptype_str} partitions are'
    else:
        msg = f'{len(too_small_partitions)}/{len(partitions)} {ptype_str} partitions are'
    msg += f' too small for binary {bin_name} size {bin_size:#x}:'
    for p in too_small_partitions:
        msg += f'\n  - {p} (overflow {bin_size - p.size:#x})'
    if not allow_failures and len(partitions) == len(too_small_partitions):
        # if some partitions can fit the binary then just print a warning
        _fail(msg)
    else:
        log.warn(msg)


def _configure_globals(offset: int, allow_failures_flag: bool) -> None:
    global allow_failures  # pylint: disable=global-statement

    gen_esp32part.quiet = True
    gen_esp32part.offset_part_table = offset
    allow_failures = allow_failures_flag


@click.group(
    context_settings={'help_option_names': ['-h', '--help']},
    help='Check binary sizes against partition table entries',
)
@click.option('--target', type=click.Choice(['esp32', 'esp32s2']))
@click.option(
    '--allow-failures',
    'allow_failures_flag',
    is_flag=True,
    help='If true, failures will print warnings but not exit with an error',
)
@click.option('--offset', '-o', type=AnyIntType(), default=0x8000, show_default=True, help='Set partition table offset')
@click.pass_context
def cli(ctx, target, allow_failures_flag, offset):  # type: ignore[no-untyped-def]
    ctx.ensure_object(dict)
    ctx.obj['offset'] = offset
    ctx.obj['allow_failures'] = allow_failures_flag


@cli.command('bootloader', help='Check bootloader binary size against partition table offset')
@click.argument('bootloader_offset', type=AnyIntType())
@click.argument('bootloader_binary', type=click.File('rb'))
@click.pass_context
def bootloader_cmd(ctx, bootloader_offset, bootloader_binary):  # type: ignore[no-untyped-def]
    _configure_globals(ctx.obj['offset'], ctx.obj['allow_failures'])
    check_bootloader(ctx.obj['offset'], bootloader_offset, bootloader_binary)


@cli.command('partition', help='Check binary size against partition table entries')
@click.option(
    '--type', 'partition_type', required=True, help='Check the file size against all partitions of this type.'
)
@click.option('--subtype', help='Optional, only check the file size against all partitions of this subtype.')
@click.argument('partition_table', type=click.File('rb'))
@click.argument('binary', type=click.File('rb'))
@click.pass_context
def partition_cmd(ctx, partition_type, subtype, partition_table, binary):  # type: ignore[no-untyped-def]
    _configure_globals(ctx.obj['offset'], ctx.obj['allow_failures'])
    check_partition(partition_type, subtype, partition_table, binary)


def main():  # type: () -> None
    cli()


if __name__ == '__main__':
    from esp_pylib.excepthook import install_exception_reporting

    install_exception_reporting()
    main()
