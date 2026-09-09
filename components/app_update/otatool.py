#!/usr/bin/env python
#
# otatool is used to perform ota-level operations - flashing ota partition
# erasing ota partition and switching ota partition
#
# SPDX-FileCopyrightText: 2018-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import binascii
import collections
import os
import struct
import sys
import tempfile

import rich_click as click
from esp_pylib.cli_options import EspRichGroup
from esp_pylib.cli_options import MutuallyExclusiveOption
from esp_pylib.cli_options import OptionEatAll
from esp_pylib.cli_types import AnyIntType
from esp_pylib.logger import Verbosity
from esp_pylib.logger import log
from rich.markup import escape

try:
    from parttool import PARTITION_TABLE_OFFSET
    from parttool import PartitionName
    from parttool import PartitionType
    from parttool import ParttoolTarget
except ImportError:
    COMPONENTS_PATH = os.path.expandvars(os.path.join('$IDF_PATH', 'components'))
    PARTTOOL_DIR = os.path.join(COMPONENTS_PATH, 'partition_table')
    sys.path.append(PARTTOOL_DIR)
    from parttool import PARTITION_TABLE_OFFSET
    from parttool import PartitionName
    from parttool import PartitionType
    from parttool import ParttoolTarget

__version__ = '2.0'

SPI_FLASH_SEC_SIZE = 0x2000


class OtatoolTarget:
    OTADATA_PARTITION = PartitionType('data', 'ota')

    def __init__(
        self,
        port=None,
        baud=None,
        partition_table_offset=PARTITION_TABLE_OFFSET,
        partition_table_file=None,
        spi_flash_sec_size=SPI_FLASH_SEC_SIZE,
        esptool_args=[],
        esptool_write_args=[],
        esptool_read_args=[],
        esptool_erase_args=[],
    ):
        self.target = ParttoolTarget(
            port,
            baud,
            partition_table_offset,
            partition_table_file,
            esptool_args,
            esptool_write_args,
            esptool_read_args,
            esptool_erase_args,
        )
        self.spi_flash_sec_size = spi_flash_sec_size

        temp_file = tempfile.NamedTemporaryFile(delete=False)
        temp_file.close()
        try:
            self.target.read_partition(OtatoolTarget.OTADATA_PARTITION, temp_file.name)
            with open(temp_file.name, 'rb') as f:
                self.otadata = f.read()
        finally:
            os.unlink(temp_file.name)

    def _check_otadata_partition(self):
        if not self.otadata:
            raise Exception('No otadata partition found')

    def erase_otadata(self):
        self._check_otadata_partition()
        self.target.erase_partition(OtatoolTarget.OTADATA_PARTITION)

    def _get_otadata_info(self):
        info = []

        otadata_info = collections.namedtuple('otadata_info', 'seq crc')

        for i in range(2):
            start = i * (self.spi_flash_sec_size >> 1)

            seq = bytearray(self.otadata[start : start + 4])
            crc = bytearray(self.otadata[start + 28 : start + 32])

            seq = struct.unpack('I', seq)
            crc = struct.unpack('I', crc)
            info.append(otadata_info(seq[0], crc[0]))

        return info

    def _get_partition_id_from_ota_id(self, ota_id):
        if isinstance(ota_id, int):
            return PartitionType('app', 'ota_' + str(ota_id))
        else:
            return PartitionName(ota_id)

    def switch_ota_partition(self, ota_id):
        self._check_otadata_partition()

        import gen_esp32part as gen

        def is_otadata_info_valid(status):
            seq = status.seq % (1 << 32)
            crc = binascii.crc32(struct.pack('I', seq), 0xFFFFFFFF) % (1 << 32)
            return seq < (int('0xFFFFFFFF', 16) % (1 << 32)) and status.crc == crc

        partition_table = self.target.partition_table

        ota_partitions = list()

        for i in range(gen.NUM_PARTITION_SUBTYPE_APP_OTA):
            ota_partition = filter(lambda p: p.subtype == (gen.MIN_PARTITION_SUBTYPE_APP_OTA + i), partition_table)

            try:
                ota_partitions.append(list(ota_partition)[0])
            except IndexError:
                break

        ota_partitions = sorted(ota_partitions, key=lambda p: p.subtype)

        if not ota_partitions:
            raise Exception('No ota app partitions found')

        # Look for the app partition to switch to
        ota_partition_next = None

        try:
            if isinstance(ota_id, int):
                ota_partition_next = filter(
                    lambda p: p.subtype - gen.MIN_PARTITION_SUBTYPE_APP_OTA == ota_id, ota_partitions
                )
            else:
                ota_partition_next = filter(lambda p: p.name == ota_id, ota_partitions)

            ota_partition_next = list(ota_partition_next)[0]
        except IndexError:
            raise Exception('Partition to switch to not found')

        otadata_info = self._get_otadata_info()

        # Find the copy to base the computation for ota sequence number on
        otadata_compute_base = -1

        # Both are valid, take the max as computation base
        if is_otadata_info_valid(otadata_info[0]) and is_otadata_info_valid(otadata_info[1]):
            if otadata_info[0].seq >= otadata_info[1].seq:
                otadata_compute_base = 0
            else:
                otadata_compute_base = 1
        # Only one copy is valid, use that
        elif is_otadata_info_valid(otadata_info[0]):
            otadata_compute_base = 0
        elif is_otadata_info_valid(otadata_info[1]):
            otadata_compute_base = 1
        # Both are invalid (could be initial state - all 0xFF's)
        else:
            pass

        ota_seq_next = 0
        ota_partitions_num = len(ota_partitions)

        target_seq = (ota_partition_next.subtype & 0x0F) + 1

        # Find the next ota sequence number
        if otadata_compute_base == 0 or otadata_compute_base == 1:
            base_seq = otadata_info[otadata_compute_base].seq % (1 << 32)

            i = 0
            while base_seq > target_seq % ota_partitions_num + i * ota_partitions_num:
                i += 1

            ota_seq_next = target_seq % ota_partitions_num + i * ota_partitions_num
        else:
            ota_seq_next = target_seq

        # Create binary data from computed values
        ota_seq_next = struct.pack('I', ota_seq_next)
        ota_seq_crc_next = binascii.crc32(ota_seq_next, 0xFFFFFFFF) % (1 << 32)
        ota_seq_crc_next = struct.pack('I', ota_seq_crc_next)

        temp_file = tempfile.NamedTemporaryFile(delete=False)
        temp_file.close()

        try:
            with open(temp_file.name, 'wb') as otadata_next_file:
                start = (1 if otadata_compute_base == 0 else 0) * (self.spi_flash_sec_size >> 1)

                otadata_next_file.write(self.otadata)

                otadata_next_file.seek(start)
                otadata_next_file.write(ota_seq_next)

                otadata_next_file.seek(start + 28)
                otadata_next_file.write(ota_seq_crc_next)

                otadata_next_file.flush()

            self.target.write_partition(OtatoolTarget.OTADATA_PARTITION, temp_file.name)
        finally:
            os.unlink(temp_file.name)

    def read_ota_partition(self, ota_id, output):
        self.target.read_partition(self._get_partition_id_from_ota_id(ota_id), output)

    def write_ota_partition(self, ota_id, input_file):
        self.target.write_partition(self._get_partition_id_from_ota_id(ota_id), input_file)

    def erase_ota_partition(self, ota_id):
        self.target.erase_partition(self._get_partition_id_from_ota_id(ota_id))


def _read_otadata(target):
    target._check_otadata_partition()

    otadata_info = target._get_otadata_info()

    # Regular tool output (same stream as pre-pylib status()/print).
    log.print('             {:8s} \t  {:8s} | \t  {:8s} \t   {:8s}'.format('OTA_SEQ', 'CRC', 'OTA_SEQ', 'CRC'))
    log.print(
        f'Firmware:  {otadata_info[0].seq:#08x} \t{otadata_info[0].crc:#08x} | '
        f'\t{otadata_info[1].seq:#08x} \t {otadata_info[1].crc:#08x}'
    )


def _erase_otadata(target):
    target.erase_otadata()
    log.print('Erased ota_data partition contents')


def _switch_ota_partition(target, ota_id):
    target.switch_ota_partition(ota_id)


def _read_ota_partition(target, ota_id, output):
    target.read_ota_partition(ota_id, output)
    log.print(f'Read ota partition contents to file {escape(str(output))}')


def _write_ota_partition(target, ota_id, input_file):
    target.write_ota_partition(ota_id, input_file)
    log.print(f'Written contents of file {escape(str(input_file))} to ota partition')


def _erase_ota_partition(target, ota_id):
    target.erase_ota_partition(ota_id)
    log.print('Erased contents of ota partition')


def _target_kwargs_from_ctx(ctx_obj, spi_flash_sec_size=None):
    kwargs = {}
    for key, value in (
        ('port', ctx_obj.get('port')),
        ('baud', ctx_obj.get('baud')),
        ('partition_table_offset', ctx_obj.get('partition_table_offset')),
        ('partition_table_file', ctx_obj.get('partition_table_file')),
        ('esptool_args', ctx_obj.get('esptool_args')),
        ('esptool_write_args', ctx_obj.get('esptool_write_args')),
        ('esptool_read_args', ctx_obj.get('esptool_read_args')),
        ('esptool_erase_args', ctx_obj.get('esptool_erase_args')),
    ):
        if value is not None and value != ():
            kwargs[key] = value
    if spi_flash_sec_size is not None:
        kwargs['spi_flash_sec_size'] = spi_flash_sec_size
    return kwargs


def _resolve_ota_id(slot, name):
    if name is not None:
        return name
    if slot is not None:
        return slot
    return None


def _run_operation(operation, target, quiet=False, **op_kwargs):
    otatool_ops = {
        'read_otadata': (_read_otadata, []),
        'erase_otadata': (_erase_otadata, []),
        'switch_ota_partition': (_switch_ota_partition, ['ota_id']),
        'read_ota_partition': (_read_ota_partition, ['ota_id', 'output']),
        'write_ota_partition': (_write_ota_partition, ['ota_id', 'input_file']),
        'erase_ota_partition': (_erase_ota_partition, ['ota_id']),
    }

    op, op_arg_names = otatool_ops[operation]
    common_args = {'target': target}
    for op_arg in op_arg_names:
        common_args[op_arg] = op_kwargs[op_arg]

    if quiet:
        try:
            op(**common_args)
        except Exception:
            sys.exit(2)
    else:
        op(**common_args)


def _slot_or_name_options(func):
    decorators = [
        click.option(
            '--slot',
            type=int,
            cls=MutuallyExclusiveOption,
            exclusive_with=['name'],
            help='slot number of the ota partition',
        ),
        click.option(
            '--name',
            cls=MutuallyExclusiveOption,
            exclusive_with=['slot'],
            help='name of the ota partition',
        ),
    ]
    for decorator in reversed(decorators):
        func = decorator(func)
    return func


@click.group(
    cls=EspRichGroup,
    invoke_without_command=True,
    context_settings={'help_option_names': ['-h', '--help']},
    help='ESP-IDF OTA Partitions Tool',
)
@click.option('--quiet', '-q', is_flag=True, help='suppress status messages')
@click.option(
    '--esptool-args',
    multiple=True,
    cls=OptionEatAll,
    type=str,
    help='additional main arguments for esptool',
)
@click.option(
    '--esptool-write-args',
    multiple=True,
    cls=OptionEatAll,
    type=str,
    help='additional subcommand arguments for esptool write-flash',
)
@click.option(
    '--esptool-read-args',
    multiple=True,
    cls=OptionEatAll,
    type=str,
    help='additional subcommand arguments for esptool read-flash',
)
@click.option(
    '--esptool-erase-args',
    multiple=True,
    cls=OptionEatAll,
    type=str,
    help='additional subcommand arguments for esptool erase-region',
)
@click.option('--port', '-p', help='port where the device to read the partition table from is attached')
@click.option('--baud', '-b', type=int, help='baudrate to use')
@click.option('--partition-table-offset', '-o', type=AnyIntType(), help='offset to read the partition table from')
@click.option(
    '--partition-table-file',
    '-f',
    type=click.Path(),
    help='file (CSV/binary) to read the partition table from; '
    'overrides device attached to specified port as the partition table source when defined',
)
@click.pass_context
def cli(
    ctx,
    quiet,
    esptool_args,
    esptool_write_args,
    esptool_read_args,
    esptool_erase_args,
    port,
    baud,
    partition_table_offset,
    partition_table_file,
):
    if quiet:
        log.set_verbosity(Verbosity.SILENT)

    ctx.ensure_object(dict)
    ctx.obj.update(
        {
            'quiet': quiet,
            'esptool_args': esptool_args,
            'esptool_write_args': esptool_write_args,
            'esptool_read_args': esptool_read_args,
            'esptool_erase_args': esptool_erase_args,
            'port': port,
            'baud': baud,
            'partition_table_offset': partition_table_offset,
            'partition_table_file': partition_table_file,
        }
    )

    # Match pre-click argparse: no subcommand → help (unless quiet) and exit 1.
    if ctx.invoked_subcommand is None:
        if not quiet:
            click.echo(ctx.get_help())
        sys.exit(1)


@cli.command('read_otadata', help='read otadata partition')
@click.option('--spi-flash-sec-size', type=AnyIntType(), help='value of SPI_FLASH_SEC_SIZE macro')
@click.pass_context
def read_otadata_cmd(ctx, spi_flash_sec_size):
    target = OtatoolTarget(**_target_kwargs_from_ctx(ctx.obj, spi_flash_sec_size))
    _run_operation('read_otadata', target, quiet=ctx.obj.get('quiet'))


@cli.command('erase_otadata', help='erase otadata partition')
@click.pass_context
def erase_otadata_cmd(ctx):
    target = OtatoolTarget(**_target_kwargs_from_ctx(ctx.obj))
    _run_operation('erase_otadata', target, quiet=ctx.obj.get('quiet'))


@cli.command('switch_ota_partition', help='switch otadata partition')
@_slot_or_name_options
@click.option('--spi-flash-sec-size', type=AnyIntType(), help='value of SPI_FLASH_SEC_SIZE macro')
@click.pass_context
def switch_ota_partition_cmd(ctx, slot, name, spi_flash_sec_size):
    ota_id = _resolve_ota_id(slot, name)
    if ota_id is None:
        # Under --quiet, match pre-click: silent exit 2 (exception was swallowed).
        if ctx.obj.get('quiet'):
            sys.exit(2)
        log.die('Partition to switch to should be defined using --slot OR --name')
    target = OtatoolTarget(**_target_kwargs_from_ctx(ctx.obj, spi_flash_sec_size))
    _run_operation('switch_ota_partition', target, quiet=ctx.obj.get('quiet'), ota_id=ota_id)


@cli.command('read_ota_partition', help='read contents of an ota partition')
@_slot_or_name_options
@click.option('--output', help='file to write the contents of the ota partition to', required=True)
@click.pass_context
def read_ota_partition_cmd(ctx, slot, name, output):
    ota_id = _resolve_ota_id(slot, name)
    if ota_id is None:
        if ctx.obj.get('quiet'):
            sys.exit(2)
        log.die('OTA partition should be defined using --slot OR --name')
    target = OtatoolTarget(**_target_kwargs_from_ctx(ctx.obj))
    _run_operation('read_ota_partition', target, quiet=ctx.obj.get('quiet'), ota_id=ota_id, output=output)


@cli.command('write_ota_partition', help='write contents to an ota partition')
@_slot_or_name_options
@click.option('--input', 'input_file', help='file whose contents to write to the ota partition')
@click.pass_context
def write_ota_partition_cmd(ctx, slot, name, input_file):
    ota_id = _resolve_ota_id(slot, name)
    if ota_id is None:
        if ctx.obj.get('quiet'):
            sys.exit(2)
        log.die('OTA partition should be defined using --slot OR --name')
    target = OtatoolTarget(**_target_kwargs_from_ctx(ctx.obj))
    _run_operation('write_ota_partition', target, quiet=ctx.obj.get('quiet'), ota_id=ota_id, input_file=input_file)


@cli.command('erase_ota_partition', help='erase contents of an ota partition')
@_slot_or_name_options
@click.pass_context
def erase_ota_partition_cmd(ctx, slot, name):
    ota_id = _resolve_ota_id(slot, name)
    if ota_id is None:
        if ctx.obj.get('quiet'):
            sys.exit(2)
        log.die('OTA partition should be defined using --slot OR --name')
    target = OtatoolTarget(**_target_kwargs_from_ctx(ctx.obj))
    _run_operation('erase_ota_partition', target, quiet=ctx.obj.get('quiet'), ota_id=ota_id)


def main():
    cli()


if __name__ == '__main__':
    from esp_pylib.excepthook import install_exception_reporting

    install_exception_reporting()
    main()
