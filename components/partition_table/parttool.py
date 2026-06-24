#!/usr/bin/env python
#
# parttool is used to perform partition level operations - reading,
# writing, erasing and getting info about the partition.
#
# SPDX-FileCopyrightText: 2018-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import re
import subprocess
import sys
import tempfile

import gen_esp32part as gen
import rich_click as click
from esp_pylib.cli_options import EspRichGroup
from esp_pylib.cli_options import MutuallyExclusiveOption
from esp_pylib.cli_options import OptionEatAll
from esp_pylib.cli_types import AnyIntType
from esp_pylib.logger import log

__version__ = '2.2'

COMPONENTS_PATH = os.path.expandvars(os.path.join('$IDF_PATH', 'components'))

PARTITION_TABLE_OFFSET = 0x8000


class _StringListParamType(click.ParamType):
    """ParamType for `OptionEatAll` without ``multiple=True`` (argparse ``nargs='+'``)."""

    name = 'text_list'

    def convert(self, value, param, ctx):
        if not value:
            return ()
        if isinstance(value, (list, tuple)):  # noqa: UP038
            return tuple(value)
        return (value,)


_ESPTOOL_ARGS_TYPE = _StringListParamType()


quiet = False


def status(msg):
    if not quiet:
        log.print(msg, file=sys.stderr)


class _PartitionId:
    def __init__(self, name=None, p_type=None, subtype=None, part_list=None):
        self.name = name
        self.type = p_type
        self.subtype = subtype
        self.part_list = part_list


class PartitionName(_PartitionId):
    def __init__(self, name):
        _PartitionId.__init__(self, name=name)


class PartitionType(_PartitionId):
    def __init__(self, p_type, subtype, part_list=None):
        _PartitionId.__init__(self, p_type=p_type, subtype=subtype, part_list=part_list)


PARTITION_BOOT_DEFAULT = _PartitionId()


class ParttoolTarget:
    def __init__(
        self,
        port=None,
        baud=None,
        partition_table_offset=PARTITION_TABLE_OFFSET,
        primary_bootloader_offset=None,
        recovery_bootloader_offset=None,
        partition_table_file=None,
        esptool_args=[],
        esptool_write_args=[],
        esptool_read_args=[],
        esptool_erase_args=[],
    ):
        self.port = port
        self.baud = baud

        gen.offset_part_table = partition_table_offset
        gen.primary_bootloader_offset = primary_bootloader_offset
        gen.recovery_bootloader_offset = recovery_bootloader_offset
        gen.quiet = True

        def parse_esptool_args(esptool_args):
            results = list()
            for arg in esptool_args:
                pattern = re.compile(r'(.+)=(.+)')
                result = pattern.match(arg)
                try:
                    key = result.group(1)
                    value = result.group(2)
                    results.extend(['--' + key, value])
                except AttributeError:
                    results.extend(['--' + arg])
            return results

        self.esptool_args = parse_esptool_args(esptool_args)
        self.esptool_write_args = parse_esptool_args(esptool_write_args)
        self.esptool_read_args = parse_esptool_args(esptool_read_args)
        self.esptool_erase_args = parse_esptool_args(esptool_erase_args)

        if partition_table_file:
            with open(partition_table_file, 'rb') as f:
                partition_table, _ = gen.PartitionTable.from_file(f)
        else:
            temp_file = tempfile.NamedTemporaryFile(delete=False)
            temp_file.close()

            try:
                self._call_esptool(
                    ['read-flash', str(partition_table_offset), str(gen.MAX_PARTITION_LENGTH), temp_file.name]
                )
                with open(temp_file.name, 'rb') as f:
                    partition_table = gen.PartitionTable.from_binary(f.read())
            finally:
                os.unlink(temp_file.name)

        self.partition_table = partition_table

    # set `out` to None to redirect the output to the STDOUT
    # otherwise set `out` to file descriptor
    # beware that the method does not close the file descriptor
    def _call_esptool(self, args, out=None):
        esptool_args = [sys.executable, '-m', 'esptool'] + self.esptool_args

        if self.port:
            esptool_args += ['--port', self.port]

        if self.baud:
            esptool_args += ['--baud', str(self.baud)]

        esptool_args += args

        log.print(f'Running {" ".join(esptool_args)}...', file=sys.stderr)
        try:
            subprocess.check_call(esptool_args, stdout=out, stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError as e:
            log.print(f'An exception: ** {e} ** occurred in _call_esptool.', file=out or sys.stderr)
            raise e

    def get_partition_info(self, partition_id):
        partition = None

        if partition_id.name:
            partition = self.partition_table.find_by_name(partition_id.name)
        elif partition_id.type and partition_id.subtype:
            partition = list(self.partition_table.find_by_type(partition_id.type, partition_id.subtype))
            if not partition_id.part_list:
                partition = partition[0]
        else:  # default boot partition
            search = ['factory'] + [f'ota_{d}' for d in range(16)]
            for subtype in search:
                partition = next(self.partition_table.find_by_type('app', subtype), None)
                if partition:
                    break

        if not partition:
            raise Exception('Partition does not exist')

        return partition

    def erase_partition(self, partition_id):
        partition = self.get_partition_info(partition_id)
        self._call_esptool(['erase-region', str(partition.offset), str(partition.size)] + self.esptool_erase_args)

    def read_partition(self, partition_id, output):
        partition = self.get_partition_info(partition_id)
        self._call_esptool(['read-flash', str(partition.offset), str(partition.size), output] + self.esptool_read_args)

    def write_partition(self, partition_id, input, ignore_readonly=False):  # noqa: A002
        partition = self.get_partition_info(partition_id)

        if partition.readonly and not ignore_readonly:
            log.die(f'"{partition.name}" partition is read-only, (use the --ignore-readonly flag to skip it)')

        self.erase_partition(partition_id)

        with open(input, 'rb') as f:
            content_len = len(f.read())

            if content_len > partition.size:
                raise Exception('Input file size exceeds partition size')

        self._call_esptool(['write-flash', str(partition.offset), input] + self.esptool_write_args)


def _write_partition(target, partition_id, input, ignore_readonly=False):  # noqa: A002
    target.write_partition(partition_id, input, ignore_readonly)
    partition = target.get_partition_info(partition_id)
    status(f"Written contents of file '{input}' at offset 0x{partition.offset:x}")


def _read_partition(target, partition_id, output):
    target.read_partition(partition_id, output)
    partition = target.get_partition_info(partition_id)
    status(
        f"Read partition '{partition.name}' contents from device at offset 0x{partition.offset:x} to file '{output}'"
    )


def _erase_partition(target, partition_id):
    target.erase_partition(partition_id)
    partition = target.get_partition_info(partition_id)
    status(f"Erased partition '{partition.name}' at offset 0x{partition.offset:x}")


def _get_partition_info(target, partition_id, info):
    try:
        partitions = target.get_partition_info(partition_id)
        if not isinstance(partitions, list):
            partitions = [partitions]
    except Exception:
        return

    infos = []

    try:
        for p in partitions:
            info_dict = {
                'name': f'{p.name}',
                'type': f'{p.type}',
                'subtype': f'{p.subtype}',
                'offset': f'0x{p.offset:x}',
                'size': f'0x{p.size:x}',
                'encrypted': f'{p.encrypted}',
                'readonly': f'{p.readonly}',
            }
            for i in info:
                infos += [info_dict[i]]
    except KeyError:
        log.die(f'Request for unknown partition info {i}')

    print(' '.join(infos))


def _partition_selection_options(func):
    decorators = [
        click.option(
            '--partition-name',
            '-n',
            cls=MutuallyExclusiveOption,
            exclusive_with=['partition_type', 'partition_boot_default'],
            help='name of the partition',
        ),
        click.option(
            '--partition-type',
            '-t',
            cls=MutuallyExclusiveOption,
            exclusive_with=['partition_name', 'partition_boot_default'],
            help='type of the partition',
        ),
        click.option(
            '--partition-boot-default',
            '-d',
            is_flag=True,
            cls=MutuallyExclusiveOption,
            exclusive_with=['partition_name', 'partition_type'],
            help='select the default boot partition using the same fallback logic as the IDF bootloader',
        ),
        click.option('--partition-subtype', '-s', help='subtype of the partition'),
        click.option('--extra-partition-subtypes', multiple=True, help='Extra partition subtype entries'),
    ]
    for decorator in reversed(decorators):
        func = decorator(func)
    return func


def _build_partition_id(partition_name, partition_type, partition_subtype, partition_boot_default, part_list=False):
    if partition_name:
        return PartitionName(partition_name)
    if partition_type:
        if not partition_subtype:
            log.die('--partition-subtype should be defined when --partition-type is defined')
        return PartitionType(partition_type, partition_subtype, part_list)
    if partition_boot_default:
        return PARTITION_BOOT_DEFAULT
    log.die(
        'Partition to operate on should be defined using --partition-name OR '
        'partition-type,--partition-subtype OR partition-boot-default'
    )


def _target_kwargs_from_ctx(ctx_obj, extra_partition_subtypes=None):
    kwargs = {}
    for key, value in (
        ('port', ctx_obj.get('port')),
        ('baud', ctx_obj.get('baud')),
        ('partition_table_offset', ctx_obj.get('partition_table_offset')),
        ('primary_bootloader_offset', ctx_obj.get('primary_bootloader_offset')),
        ('recovery_bootloader_offset', ctx_obj.get('recovery_bootloader_offset')),
        ('partition_table_file', ctx_obj.get('partition_table_file')),
        ('esptool_args', ctx_obj.get('esptool_args')),
        ('esptool_write_args', ctx_obj.get('esptool_write_args')),
        ('esptool_read_args', ctx_obj.get('esptool_read_args')),
        ('esptool_erase_args', ctx_obj.get('esptool_erase_args')),
    ):
        if value is not None and value != ():
            kwargs[key] = value
    if extra_partition_subtypes:
        gen.add_extra_subtypes(extra_partition_subtypes)
    return kwargs


def _run_operation(operation, quiet_flag, target, partition_id, **op_kwargs):
    global quiet

    quiet = quiet_flag

    parttool_ops = {
        'erase_partition': (_erase_partition, []),
        'read_partition': (_read_partition, ['output']),
        'write_partition': (_write_partition, ['input', 'ignore_readonly']),
        'get_partition_info': (_get_partition_info, ['info']),
    }

    op, op_arg_names = parttool_ops[operation]
    common_args = {'target': target, 'partition_id': partition_id}
    for op_arg in op_arg_names:
        common_args[op_arg] = op_kwargs[op_arg]

    if quiet:
        try:
            op(**common_args)
        except Exception:
            sys.exit(2)
    else:
        try:
            op(**common_args)
        except gen.InputError as e:
            log.print(str(e), file=sys.stderr)
            sys.exit(2)


@click.group(
    cls=EspRichGroup,
    context_settings={'help_option_names': ['-h', '--help']},
    help='ESP-IDF Partitions Tool',
)
@click.option('--quiet', '-q', is_flag=True, help='suppress stderr messages')
@click.option(
    '--esptool-args',
    cls=OptionEatAll,
    type=_ESPTOOL_ARGS_TYPE,
    default=(),
    help='additional main arguments for esptool',
)
@click.option(
    '--esptool-write-args',
    cls=OptionEatAll,
    type=_ESPTOOL_ARGS_TYPE,
    default=(),
    help='additional subcommand arguments when writing to flash',
)
@click.option(
    '--esptool-read-args',
    cls=OptionEatAll,
    type=_ESPTOOL_ARGS_TYPE,
    default=(),
    help='additional subcommand arguments when reading flash',
)
@click.option(
    '--esptool-erase-args',
    cls=OptionEatAll,
    type=_ESPTOOL_ARGS_TYPE,
    default=(),
    help='additional subcommand arguments when erasing regions of flash',
)
@click.option(
    '--port',
    '-p',
    help='port where the target device of the command is connected to; the partition table is sourced from '
    'this device when the partition table file is not defined',
)
@click.option('--baud', '-b', type=int, help='baudrate to use')
@click.option('--partition-table-offset', '-o', type=AnyIntType(), help='offset to read the partition table from')
@click.option('--primary-bootloader-offset', type=AnyIntType(), help='offset for primary bootloader')
@click.option('--recovery-bootloader-offset', type=AnyIntType(), help='offset for recovery bootloader')
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
    primary_bootloader_offset,
    recovery_bootloader_offset,
    partition_table_file,
):
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
            'primary_bootloader_offset': primary_bootloader_offset,
            'recovery_bootloader_offset': recovery_bootloader_offset,
            'partition_table_file': partition_table_file,
        }
    )


def _shared_subcommand_options(func):
    func = click.pass_context(func)
    return func


@cli.command('read_partition', help='read partition from device and dump contents into a file')
@_partition_selection_options
@click.option('--output', help='file to dump the read partition contents to')
@_shared_subcommand_options
def read_partition_cmd(
    ctx,
    partition_name,
    partition_type,
    partition_boot_default,
    partition_subtype,
    extra_partition_subtypes,
    output,
):
    partition_id = _build_partition_id(partition_name, partition_type, partition_subtype, partition_boot_default)
    target = ParttoolTarget(**_target_kwargs_from_ctx(ctx.obj, extra_partition_subtypes))
    _run_operation('read_partition', ctx.obj['quiet'], target, partition_id, output=output)


@cli.command('write_partition', help='write contents of a binary file to partition on device')
@_partition_selection_options
@click.option('--input', 'input_file', help='file whose contents are to be written to the partition offset')
@click.option('--ignore-readonly', is_flag=True, help='Ignore read-only attribute')
@_shared_subcommand_options
def write_partition_cmd(
    ctx,
    partition_name,
    partition_type,
    partition_boot_default,
    partition_subtype,
    extra_partition_subtypes,
    input_file,
    ignore_readonly,
):
    partition_id = _build_partition_id(partition_name, partition_type, partition_subtype, partition_boot_default)
    target = ParttoolTarget(**_target_kwargs_from_ctx(ctx.obj, extra_partition_subtypes))
    _run_operation(
        'write_partition',
        ctx.obj['quiet'],
        target,
        partition_id,
        input=input_file,
        ignore_readonly=ignore_readonly,
    )


@cli.command('erase_partition', help='erase the contents of a partition on the device')
@_partition_selection_options
@_shared_subcommand_options
def erase_partition_cmd(
    ctx,
    partition_name,
    partition_type,
    partition_boot_default,
    partition_subtype,
    extra_partition_subtypes,
):
    partition_id = _build_partition_id(partition_name, partition_type, partition_subtype, partition_boot_default)
    target = ParttoolTarget(**_target_kwargs_from_ctx(ctx.obj, extra_partition_subtypes))
    _run_operation('erase_partition', ctx.obj['quiet'], target, partition_id)


@cli.command('get_partition_info', help='get partition information')
@_partition_selection_options
@click.option(
    '--info',
    type=click.Choice(['name', 'type', 'subtype', 'offset', 'size', 'encrypted', 'readonly']),
    multiple=True,
    cls=OptionEatAll,
    default=['offset', 'size'],
    show_default=True,
    help='type of partition information to get',
)
@click.option('--part_list', 'part_list', is_flag=True, help='Get a list of partitions suitable for a given type')
@_shared_subcommand_options
def get_partition_info_cmd(
    ctx,
    partition_name,
    partition_type,
    partition_boot_default,
    partition_subtype,
    extra_partition_subtypes,
    info,
    part_list,
):
    partition_id = _build_partition_id(
        partition_name, partition_type, partition_subtype, partition_boot_default, part_list=part_list
    )
    target = ParttoolTarget(**_target_kwargs_from_ctx(ctx.obj, extra_partition_subtypes))
    _run_operation('get_partition_info', ctx.obj['quiet'], target, partition_id, info=info)


def main():
    cli()


if __name__ == '__main__':
    from esp_pylib.excepthook import install_exception_reporting

    install_exception_reporting()
    main()
