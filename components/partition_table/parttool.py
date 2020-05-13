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
import gen_esp32part as gen

__version__ = '1.0'

IDF_COMPONENTS_PATH = os.path.expandvars(os.path.join("$IDF_PATH", "components"))

ESPTOOL_PY = os.path.join(IDF_COMPONENTS_PATH, "esptool_py", "esptool", "esptool.py")

quiet = False


def status(msg):
    """ Print status message to stderr """
    if not quiet:
        print(msg)


def _invoke_esptool(esptool_args, args):
    m_esptool_args = [sys.executable, ESPTOOL_PY]

    if args.port != "":
        m_esptool_args.extend(["--port", args.port])

    m_esptool_args.extend(esptool_args)

    if quiet:
        with open(os.devnull, "w") as fnull:
            subprocess.check_call(m_esptool_args, stdout=fnull, stderr=fnull)
    else:
        subprocess.check_call(m_esptool_args)


def _get_partition_table(args):
    partition_table = None

    gen.offset_part_table = int(args.partition_table_offset, 0)

    if args.partition_table_file:
        status("Reading partition table from partition table file...")

        try:
            with open(args.partition_table_file, "rb") as partition_table_file:
                partition_table = gen.PartitionTable.from_binary(partition_table_file.read())
                status("Partition table read from binary file {}".format(partition_table_file.name))
        except (gen.InputError, TypeError):
            with open(args.partition_table_file, "r") as partition_table_file:
                partition_table_file.seek(0)
                partition_table = gen.PartitionTable.from_csv(partition_table_file.read())
                status("Partition table read from CSV file {}".format(partition_table_file.name))
    else:
        port_info = (" on port " + args.port if args.port else "")
        status("Reading partition table from device{}...".format(port_info))

        f_name = None
        with tempfile.NamedTemporaryFile(delete=False) as f:
            f_name = f.name

        try:
            invoke_args = ["read_flash", str(gen.offset_part_table), str(gen.MAX_PARTITION_LENGTH), f_name]
            _invoke_esptool(invoke_args, args)
            with open(f_name, "rb") as f:
                partition_table = gen.PartitionTable.from_binary(f.read())
                status("Partition table read from device" + port_info)
        finally:
            os.unlink(f_name)

    return partition_table


def _get_partition(args):
    partition_table = _get_partition_table(args)

    partition = None

    if args.partition_name:
        partition = partition_table.find_by_name(args.partition_name)
    elif args.partition_type and args.partition_subtype:
        partition = partition_table.find_by_type(args.partition_type, args.partition_subtype)
    elif args.partition_boot_default:
        search = ["factory"] + ["ota_{}".format(d) for d in range(16)]
        for subtype in search:
            partition = partition_table.find_by_type("app", subtype)
            if partition is not None:
                break
    else:
        raise RuntimeError("Invalid partition selection arguments. Specify --partition-name OR \
                            --partition-type and --partition-subtype OR --partition--boot-default.")

    if partition:
        status("Found partition {}".format(str(partition)))

    return partition


def _get_and_check_partition(args):
    partition = None

    partition = _get_partition(args)

    if not partition:
        raise RuntimeError("Unable to find specified partition.")

    return partition


def write_partition(args):
    erase_partition(args)

    partition = _get_and_check_partition(args)

    status("Checking input file size...")

    with open(args.input, "rb") as input_file:
        content_len = len(input_file.read())

        if content_len != partition.size:
            status("File size (0x{:x}) does not match partition size (0x{:x})".format(content_len, partition.size))
        else:
            status("File size matches partition size (0x{:x})".format(partition.size))

    _invoke_esptool(["write_flash", str(partition.offset),  args.input], args)

    status("Written contents of file '{}' to device at offset 0x{:x}".format(args.input, partition.offset))


def read_partition(args):
    partition = _get_and_check_partition(args)
    _invoke_esptool(["read_flash", str(partition.offset), str(partition.size), args.output], args)
    status("Read partition contents from device at offset 0x{:x} to file '{}'".format(partition.offset, args.output))


def erase_partition(args):
    partition = _get_and_check_partition(args)
    _invoke_esptool(["erase_region", str(partition.offset),  str(partition.size)], args)
    status("Erased partition at offset 0x{:x} on device".format(partition.offset))


def get_partition_info(args):
    partition = None

    if args.table:
        partition_table = _get_partition_table(args)

        if args.table.endswith(".csv"):
            partition_table = partition_table.to_csv()
        else:
            partition_table = partition_table.to_binary()

        with open(args.table, "wb") as table_file:
            table_file.write(partition_table)
            status("Partition table written to " + table_file.name)
    else:
        partition = _get_partition(args)

        if partition:
            info_dict = {
                "offset": '0x{:x}'.format(partition.offset),
                "size": '0x{:x}'.format(partition.size)
            }

            infos = []

            try:
                for info in args.info:
                    infos += [info_dict[info]]
            except KeyError:
                raise RuntimeError("Request for unknown partition info {}".format(info))

            status("Requested partition information [{}]:".format(", ".join(args.info)))
            print(" ".join(infos))
        else:
            status("Partition not found")


def generate_blank_partition_file(args):
    output = None
    stdout_binary = None

    partition = _get_and_check_partition(args)
    output = b"\xFF" * partition.size

    try:
        stdout_binary = sys.stdout.buffer  # Python 3
    except AttributeError:
        stdout_binary = sys.stdout

    with stdout_binary if args.output == "" else open(args.output, 'wb') as f:
        f.write(output)
        status("Blank partition file '{}' generated".format(args.output))


def main():
    global quiet

    parser = argparse.ArgumentParser("ESP-IDF Partitions Tool")

    parser.add_argument("--quiet", "-q", help="suppress stderr messages", action="store_true")

    # There are two possible sources for the partition table: a device attached to the host
    # or a partition table CSV/binary file. These sources are mutually exclusive.
    partition_table_info_source_args = parser.add_mutually_exclusive_group()

    partition_table_info_source_args.add_argument("--port", "-p", help="port where the device to read the partition table from is attached", default="")
    partition_table_info_source_args.add_argument("--partition-table-file", "-f", help="file (CSV/binary) to read the partition table from")

    parser.add_argument("--partition-table-offset", "-o", help="offset to read the partition table from",  default="0x8000")

    # Specify what partition to perform the operation on. This can either be specified using the
    # partition name or the first partition that matches the specified type/subtype
    partition_selection_args = parser.add_mutually_exclusive_group()

    partition_selection_args.add_argument("--partition-name", "-n", help="name of the partition")
    partition_selection_args.add_argument("--partition-type", "-t", help="type of the partition")
    partition_selection_args.add_argument('--partition-boot-default', "-d", help='select the default boot partition \
                                           using the same fallback logic as the IDF bootloader', action="store_true")

    parser.add_argument("--partition-subtype", "-s", help="subtype of the partition")

    subparsers = parser.add_subparsers(dest="operation", help="run parttool -h for additional help")

    # Specify the supported operations
    read_part_subparser = subparsers.add_parser("read_partition", help="read partition from device and dump contents into a file")
    read_part_subparser.add_argument("--output", help="file to dump the read partition contents to")

    write_part_subparser = subparsers.add_parser("write_partition", help="write contents of a binary file to partition on device")
    write_part_subparser.add_argument("--input", help="file whose contents are to be written to the partition offset")

    subparsers.add_parser("erase_partition", help="erase the contents of a partition on the device")

    print_partition_info_subparser = subparsers.add_parser("get_partition_info", help="get partition information")
    print_partition_info_subparser_info_type = print_partition_info_subparser.add_mutually_exclusive_group()
    print_partition_info_subparser_info_type.add_argument("--info", help="type of partition information to get",
                                                          choices=["offset", "size"], default=["offset", "size"], nargs="+")
    print_partition_info_subparser_info_type.add_argument("--table", help="dump the partition table to a file")

    generate_blank_subparser = subparsers.add_parser("generate_blank_partition_file", help="generate a blank (all 0xFF) partition file of \
                                                     the specified partition that can be flashed to the device")
    generate_blank_subparser.add_argument("--output", help="blank partition file filename")

    args = parser.parse_args()

    quiet = args.quiet

    # No operation specified, display help and exit
    if args.operation is None:
        if not quiet:
            parser.print_help()
        sys.exit(1)

    # Else execute the operation
    operation_func = globals()[args.operation]

    if quiet:
        # If exceptions occur, suppress and exit quietly
        try:
            operation_func(args)
        except Exception:
            sys.exit(2)
    else:
        operation_func(args)


if __name__ == '__main__':
    main()
