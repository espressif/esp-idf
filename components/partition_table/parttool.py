#!/usr/bin/env python
#
# parttool is used to perform partition level operations - reading,
# writing, erasing and getting info about the partition.
#
# SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os
import re
import subprocess
import sys
import tempfile

import gen_esp32part as gen

__version__ = '2.1'

COMPONENTS_PATH = os.path.expandvars(os.path.join('$IDF_PATH', 'components'))
ESPTOOL_PY = os.path.join(COMPONENTS_PATH, 'esptool_py', 'esptool', 'esptool.py')

PARTITION_TABLE_OFFSET = 0x8000


quiet = False


def status(msg):
    if not quiet:
        print(msg)


class _PartitionId():

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


class ParttoolTarget():

    def __init__(self, port=None, baud=None, partition_table_offset=PARTITION_TABLE_OFFSET, partition_table_file=None,
                 esptool_args=[], esptool_write_args=[], esptool_read_args=[], esptool_erase_args=[]):
        self.port = port
        self.baud = baud

        gen.offset_part_table = partition_table_offset

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
            partition_table = None
            with open(partition_table_file, 'rb') as f:
                input_is_binary = (f.read(2) == gen.PartitionDefinition.MAGIC_BYTES)
                f.seek(0)
                if input_is_binary:
                    partition_table = gen.PartitionTable.from_binary(f.read())

            if partition_table is None:
                with open(partition_table_file, 'r') as f:
                    f.seek(0)
                    partition_table = gen.PartitionTable.from_csv(f.read())
        else:
            temp_file = tempfile.NamedTemporaryFile(delete=False)
            temp_file.close()

            try:
                self._call_esptool(['read_flash', str(partition_table_offset), str(gen.MAX_PARTITION_LENGTH), temp_file.name])
                with open(temp_file.name, 'rb') as f:
                    partition_table = gen.PartitionTable.from_binary(f.read())
            finally:
                os.unlink(temp_file.name)

        self.partition_table = partition_table

    # set `out` to None to redirect the output to the STDOUT
    # otherwise set `out` to file descriptor
    # beware that the method does not close the file descriptor
    def _call_esptool(self, args, out=None):
        esptool_args = [sys.executable, ESPTOOL_PY] + self.esptool_args

        if self.port:
            esptool_args += ['--port', self.port]

        if self.baud:
            esptool_args += ['--baud', str(self.baud)]

        esptool_args += args

        print('Running %s...' % (' '.join(esptool_args)))
        try:
            subprocess.check_call(esptool_args, stdout=out, stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError as e:
            print('An exception: **', str(e), '** occurred in _call_esptool.', file=out)
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
            search = ['factory'] + ['ota_{}'.format(d) for d in range(16)]
            for subtype in search:
                partition = next(self.partition_table.find_by_type('app', subtype), None)
                if partition:
                    break

        if not partition:
            raise Exception('Partition does not exist')

        return partition

    def erase_partition(self, partition_id):
        partition = self.get_partition_info(partition_id)
        self._call_esptool(['erase_region', str(partition.offset),  str(partition.size)] + self.esptool_erase_args)

    def read_partition(self, partition_id, output):
        partition = self.get_partition_info(partition_id)
        self._call_esptool(['read_flash', str(partition.offset), str(partition.size), output] + self.esptool_read_args)

    def write_partition(self, partition_id, input, ignore_readonly=False):
        partition = self.get_partition_info(partition_id)

        if partition.readonly and not ignore_readonly:
            raise SystemExit(f'"{partition.name}" partition is read-only, (use the --ignore-readonly flag to skip it)')

        self.erase_partition(partition_id)

        with open(input, 'rb') as input_file:
            content_len = len(input_file.read())

            if content_len > partition.size:
                raise Exception('Input file size exceeds partition size')

        self._call_esptool(['write_flash', str(partition.offset), input] + self.esptool_write_args)


def _write_partition(target, partition_id, input, ignore_readonly=False):
    target.write_partition(partition_id, input, ignore_readonly)
    partition = target.get_partition_info(partition_id)
    status("Written contents of file '{}' at offset 0x{:x}".format(input, partition.offset))


def _read_partition(target, partition_id, output):
    target.read_partition(partition_id, output)
    partition = target.get_partition_info(partition_id)
    status("Read partition '{}' contents from device at offset 0x{:x} to file '{}'"
           .format(partition.name, partition.offset, output))


def _erase_partition(target, partition_id):
    target.erase_partition(partition_id)
    partition = target.get_partition_info(partition_id)
    status("Erased partition '{}' at offset 0x{:x}".format(partition.name, partition.offset))


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
                'name': '{}'.format(p.name),
                'type': '{}'.format(p.type),
                'subtype': '{}'.format(p.subtype),
                'offset': '0x{:x}'.format(p.offset),
                'size': '0x{:x}'.format(p.size),
                'encrypted': '{}'.format(p.encrypted),
                'readonly': '{}'.format(p.readonly)
            }
            for i in info:
                infos += [info_dict[i]]
    except KeyError:
        raise RuntimeError('Request for unknown partition info {}'.format(i))

    print(' '.join(infos))


def main():
    global quiet

    parser = argparse.ArgumentParser('ESP-IDF Partitions Tool')

    parser.add_argument('--quiet', '-q', help='suppress stderr messages', action='store_true')
    parser.add_argument('--esptool-args', help='additional main arguments for esptool', nargs='+')
    parser.add_argument('--esptool-write-args', help='additional subcommand arguments when writing to flash', nargs='+')
    parser.add_argument('--esptool-read-args', help='additional subcommand arguments when reading flash', nargs='+')
    parser.add_argument('--esptool-erase-args', help='additional subcommand arguments when erasing regions of flash', nargs='+')

    # By default the device attached to the specified port is queried for the partition table. If a partition table file
    # is specified, that is used instead.
    parser.add_argument('--port', '-p', help='port where the target device of the command is connected to; the partition table is sourced from this device \
                                            when the partition table file is not defined')
    parser.add_argument('--baud', '-b', help='baudrate to use', type=int)

    parser.add_argument('--partition-table-offset', '-o', help='offset to read the partition table from', type=str)
    parser.add_argument('--partition-table-file', '-f', help='file (CSV/binary) to read the partition table from; \
                                                            overrides device attached to specified port as the partition table source when defined')

    partition_selection_parser = argparse.ArgumentParser(add_help=False)

    # Specify what partition to perform the operation on. This can either be specified using the
    # partition name or the first partition that matches the specified type/subtype
    partition_selection_args = partition_selection_parser.add_mutually_exclusive_group()

    partition_selection_args.add_argument('--partition-name', '-n', help='name of the partition')
    partition_selection_args.add_argument('--partition-type', '-t', help='type of the partition')
    partition_selection_args.add_argument('--partition-boot-default', '-d', help='select the default boot partition \
                                           using the same fallback logic as the IDF bootloader', action='store_true')

    partition_selection_parser.add_argument('--partition-subtype', '-s', help='subtype of the partition')
    partition_selection_parser.add_argument('--extra-partition-subtypes', help='Extra partition subtype entries', nargs='*')

    subparsers = parser.add_subparsers(dest='operation', help='run parttool -h for additional help')

    # Specify the supported operations
    read_part_subparser = subparsers.add_parser('read_partition', help='read partition from device and dump contents into a file',
                                                parents=[partition_selection_parser])
    read_part_subparser.add_argument('--output', help='file to dump the read partition contents to')

    write_part_subparser = subparsers.add_parser('write_partition', help='write contents of a binary file to partition on device',
                                                 parents=[partition_selection_parser])
    write_part_subparser.add_argument('--input', help='file whose contents are to be written to the partition offset')
    write_part_subparser.add_argument('--ignore-readonly', help='Ignore read-only attribute', action='store_true')

    subparsers.add_parser('erase_partition', help='erase the contents of a partition on the device', parents=[partition_selection_parser])

    print_partition_info_subparser = subparsers.add_parser('get_partition_info', help='get partition information', parents=[partition_selection_parser])
    print_partition_info_subparser.add_argument('--info', help='type of partition information to get',
                                                choices=['name', 'type', 'subtype', 'offset', 'size', 'encrypted', 'readonly'],
                                                default=['offset', 'size'], nargs='+')
    print_partition_info_subparser.add_argument('--part_list', help='Get a list of partitions suitable for a given type', action='store_true')

    args = parser.parse_args()
    quiet = args.quiet

    # No operation specified, display help and exit
    if args.operation is None:
        if not quiet:
            parser.print_help()
        sys.exit(1)

    # Prepare the partition to perform operation on
    if args.partition_name:
        partition_id = PartitionName(args.partition_name)
    elif args.partition_type:
        if not args.partition_subtype:
            raise RuntimeError('--partition-subtype should be defined when --partition-type is defined')
        partition_id = PartitionType(args.partition_type, args.partition_subtype, getattr(args, 'part_list', None))
    elif args.partition_boot_default:
        partition_id = PARTITION_BOOT_DEFAULT
    else:
        raise RuntimeError('Partition to operate on should be defined using --partition-name OR \
                            partition-type,--partition-subtype OR partition-boot-default')

    # Prepare the device to perform operation on
    target_args = {}

    if args.port:
        target_args['port'] = args.port

    if args.baud:
        target_args['baud'] = args.baud

    if args.partition_table_file:
        target_args['partition_table_file'] = args.partition_table_file

    if args.partition_table_offset:
        target_args['partition_table_offset'] = int(args.partition_table_offset, 0)

    if args.esptool_args:
        target_args['esptool_args'] = args.esptool_args

    if args.esptool_write_args:
        target_args['esptool_write_args'] = args.esptool_write_args

    if args.esptool_read_args:
        target_args['esptool_read_args'] = args.esptool_read_args

    if args.esptool_erase_args:
        target_args['esptool_erase_args'] = args.esptool_erase_args

    if args.extra_partition_subtypes:
        gen.add_extra_subtypes(args.extra_partition_subtypes)

    target = ParttoolTarget(**target_args)

    # Create the operation table and execute the operation
    common_args = {'target':target, 'partition_id':partition_id}
    parttool_ops = {
        'erase_partition': (_erase_partition, []),
        'read_partition': (_read_partition, ['output']),
        'write_partition': (_write_partition, ['input', 'ignore_readonly']),
        'get_partition_info': (_get_partition_info, ['info'])
    }

    (op, op_args) = parttool_ops[args.operation]

    for op_arg in op_args:
        common_args.update({op_arg:vars(args)[op_arg]})

    if quiet:
        # If exceptions occur, suppress and exit quietly
        try:
            op(**common_args)
        except Exception:
            sys.exit(2)
    else:
        try:
            op(**common_args)
        except gen.InputError as e:
            print(e, file=sys.stderr)
            sys.exit(2)


if __name__ == '__main__':
    main()
