#!/usr/bin/env python
#
# parttool is used to perform partition level operations - reading,
# writing, erasing and getting info about the partition.
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
from __future__ import print_function, division
import argparse
import os
import sys
import subprocess
import tempfile
import re
import gen_esp32part as gen


__version__ = '2.0'

COMPONENTS_PATH = os.path.expandvars(os.path.join("$IDF_PATH", "components"))
ESPTOOL_PY = os.path.join(COMPONENTS_PATH, "esptool_py", "esptool", "esptool.py")

PARTITION_TABLE_OFFSET = 0x8000


quiet = False


def status(msg):
    if not quiet:
        print(msg)


class _PartitionId():

    def __init__(self, name=None, type=None, subtype=None):
        self.name = name
        self.type = type
        self.subtype = subtype


class PartitionName(_PartitionId):

    def __init__(self, name):
        _PartitionId.__init__(self, name=name)


class PartitionType(_PartitionId):

    def __init__(self, type, subtype):
        _PartitionId.__init__(self, type=type, subtype=subtype)


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
                pattern = re.compile(r"(.+)=(.+)")
                result = pattern.match(arg)
                try:
                    key = result.group(1)
                    value = result.group(2)
                    results.extend(["--" + key, value])
                except AttributeError:
                    results.extend(["--" + arg])
            return results

        self.esptool_args = parse_esptool_args(esptool_args)
        self.esptool_write_args = parse_esptool_args(esptool_write_args)
        self.esptool_read_args = parse_esptool_args(esptool_read_args)
        self.esptool_erase_args = parse_esptool_args(esptool_erase_args)

        if partition_table_file:
            try:
                with open(partition_table_file, "rb") as f:
                    partition_table = gen.PartitionTable.from_binary(f.read())
            except (gen.InputError, IOError, TypeError):
                with open(partition_table_file, "r") as f:
                    f.seek(0)
                    partition_table = gen.PartitionTable.from_csv(f.read())
        else:
            temp_file = tempfile.NamedTemporaryFile(delete=False)
            temp_file.close()

            try:
                self._call_esptool(["read_flash", str(partition_table_offset), str(gen.MAX_PARTITION_LENGTH), temp_file.name])
                with open(temp_file.name, "rb") as f:
                    partition_table = gen.PartitionTable.from_binary(f.read())
            finally:
                os.unlink(temp_file.name)

        self.partition_table = partition_table

    def _call_esptool(self, args, out=None):
        esptool_args = [sys.executable, ESPTOOL_PY] + self.esptool_args

        if self.port:
            esptool_args += ["--port", self.port]

        if self.baud:
            esptool_args += ["--baud", str(self.baud)]

        esptool_args += args

        with open(os.devnull, "w") as null_file:
            subprocess.check_call(esptool_args, stdout=null_file, stderr=null_file)

    def get_partition_info(self, partition_id):
        partition = None

        if partition_id.name:
            partition = self.partition_table.find_by_name(partition_id.name)
        elif partition_id.type and partition_id.subtype:
            partition = self.partition_table.find_by_type(partition_id.type, partition_id.subtype)
        else:  # default boot partition
            search = ["factory"] + ["ota_{}".format(d) for d in range(16)]
            for subtype in search:
                partition = self.partition_table.find_by_type("app", subtype)
                if partition:
                    break

        if not partition:
            raise Exception("Partition does not exist")

        return partition

    def erase_partition(self, partition_id):
        partition = self.get_partition_info(partition_id)
        self._call_esptool(["erase_region", str(partition.offset),  str(partition.size)] + self.esptool_erase_args)

    def read_partition(self, partition_id, output):
        partition = self.get_partition_info(partition_id)
        self._call_esptool(["read_flash", str(partition.offset), str(partition.size), output] + self.esptool_read_args)

    def write_partition(self, partition_id, input):
        self.erase_partition(partition_id)

        partition = self.get_partition_info(partition_id)

        with open(input, "rb") as input_file:
            content_len = len(input_file.read())

            if content_len > partition.size:
                raise Exception("Input file size exceeds partition size")

        self._call_esptool(["write_flash", str(partition.offset), input] + self.esptool_write_args)


def _write_partition(target, partition_id, input):
    target.write_partition(partition_id, input)
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
        partition = target.get_partition_info(partition_id)
    except Exception:
        return

    info_dict = {
        "offset": '0x{:x}'.format(partition.offset),
        "size": '0x{:x}'.format(partition.size)
    }

    infos = []

    try:
        for i in info:
            infos += [info_dict[i]]
    except KeyError:
        raise RuntimeError("Request for unknown partition info {}".format(i))

    print(" ".join(infos))


def main():
    global quiet

    parser = argparse.ArgumentParser("ESP-IDF Partitions Tool")

    parser.add_argument("--quiet", "-q", help="suppress stderr messages", action="store_true")
    parser.add_argument("--esptool-args", help="additional main arguments for esptool", nargs="+")
    parser.add_argument("--esptool-write-args", help="additional subcommand arguments when writing to flash", nargs="+")
    parser.add_argument("--esptool-read-args", help="additional subcommand arguments when reading flash", nargs="+")
    parser.add_argument("--esptool-erase-args", help="additional subcommand arguments when erasing regions of flash", nargs="+")

    # By default the device attached to the specified port is queried for the partition table. If a partition table file
    # is specified, that is used instead.
    parser.add_argument("--port", "-p", help="port where the target device of the command is connected to; the partition table is sourced from this device \
                                            when the partition table file is not defined")
    parser.add_argument("--baud", "-b", help="baudrate to use", type=int)

    parser.add_argument("--partition-table-offset", "-o", help="offset to read the partition table from", type=str)
    parser.add_argument("--partition-table-file", "-f", help="file (CSV/binary) to read the partition table from; \
                                                            overrides device attached to specified port as the partition table source when defined")

    partition_selection_parser = argparse.ArgumentParser(add_help=False)

    # Specify what partition to perform the operation on. This can either be specified using the
    # partition name or the first partition that matches the specified type/subtype
    partition_selection_args = partition_selection_parser.add_mutually_exclusive_group()

    partition_selection_args.add_argument("--partition-name", "-n", help="name of the partition")
    partition_selection_args.add_argument("--partition-type", "-t", help="type of the partition")
    partition_selection_args.add_argument('--partition-boot-default', "-d", help='select the default boot partition \
                                           using the same fallback logic as the IDF bootloader', action="store_true")

    partition_selection_parser.add_argument("--partition-subtype", "-s", help="subtype of the partition")

    subparsers = parser.add_subparsers(dest="operation", help="run parttool -h for additional help")

    # Specify the supported operations
    read_part_subparser = subparsers.add_parser("read_partition", help="read partition from device and dump contents into a file",
                                                parents=[partition_selection_parser])
    read_part_subparser.add_argument("--output", help="file to dump the read partition contents to")

    write_part_subparser = subparsers.add_parser("write_partition", help="write contents of a binary file to partition on device",
                                                 parents=[partition_selection_parser])
    write_part_subparser.add_argument("--input", help="file whose contents are to be written to the partition offset")

    subparsers.add_parser("erase_partition", help="erase the contents of a partition on the device", parents=[partition_selection_parser])

    print_partition_info_subparser = subparsers.add_parser("get_partition_info", help="get partition information", parents=[partition_selection_parser])
    print_partition_info_subparser.add_argument("--info", help="type of partition information to get", nargs="+")

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
            raise RuntimeError("--partition-subtype should be defined when --partition-type is defined")
        partition_id = PartitionType(args.partition_type, args.partition_subtype)
    elif args.partition_boot_default:
        partition_id = PARTITION_BOOT_DEFAULT
    else:
        raise RuntimeError("Partition to operate on should be defined using --partition-name OR \
                            partition-type,--partition-subtype OR partition-boot-default")

    # Prepare the device to perform operation on
    target_args = {}

    if args.port:
        target_args["port"] = args.port

    if args.baud:
        target_args["baud"] = args.baud

    if args.partition_table_file:
        target_args["partition_table_file"] = args.partition_table_file

    if args.partition_table_offset:
        target_args["partition_table_offset"] = int(args.partition_table_offset, 0)

    if args.esptool_args:
        target_args["esptool_args"] = args.esptool_args

    if args.esptool_write_args:
        target_args["esptool_write_args"] = args.esptool_write_args

    if args.esptool_read_args:
        target_args["esptool_read_args"] = args.esptool_read_args

    if args.esptool_erase_args:
        target_args["esptool_erase_args"] = args.esptool_erase_args

    target = ParttoolTarget(**target_args)

    # Create the operation table and execute the operation
    common_args = {'target':target, 'partition_id':partition_id}
    parttool_ops = {
        'erase_partition':(_erase_partition, []),
        'read_partition':(_read_partition, ["output"]),
        'write_partition':(_write_partition, ["input"]),
        'get_partition_info':(_get_partition_info, ["info"])
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
        op(**common_args)


if __name__ == '__main__':
    main()
