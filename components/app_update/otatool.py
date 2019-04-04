#!/usr/bin/env python
#
# otatool is used to perform ota-level operations - flashing ota partition
# erasing ota partition and switching ota partition
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
import binascii
import subprocess
import tempfile
import collections
import struct

__version__ = '1.0'

IDF_COMPONENTS_PATH = os.path.expandvars(os.path.join("$IDF_PATH", "components"))

PARTTOOL_PY = os.path.join(IDF_COMPONENTS_PATH, "partition_table", "parttool.py")

SPI_FLASH_SEC_SIZE = 0x2000

quiet = False


def status(msg):
    if not quiet:
        print(msg)


def _invoke_parttool(parttool_args, args, output=False, partition=None):
    invoke_args = []

    if partition:
        invoke_args += [sys.executable, PARTTOOL_PY] + partition
    else:
        invoke_args += [sys.executable, PARTTOOL_PY, "--partition-type", "data", "--partition-subtype", "ota"]

    if quiet:
        invoke_args += ["-q"]

    if args.port != "":
        invoke_args += ["--port", args.port]

    if args.partition_table_file:
        invoke_args += ["--partition-table-file", args.partition_table_file]

    if args.partition_table_offset:
        invoke_args += ["--partition-table-offset", args.partition_table_offset]

    invoke_args += parttool_args

    if output:
        return subprocess.check_output(invoke_args)
    else:
        return subprocess.check_call(invoke_args)


def _get_otadata_contents(args, check=True):
    global quiet

    if check:
        check_args = ["get_partition_info", "--info", "offset", "size"]

        quiet = True
        output = _invoke_parttool(check_args, args, True).split(b" ")
        quiet = args.quiet

        if not output:
            raise RuntimeError("No ota_data partition found")

    with tempfile.NamedTemporaryFile(delete=False) as f:
        f_name = f.name

    try:
        invoke_args = ["read_partition", "--output", f_name]
        _invoke_parttool(invoke_args, args)
        with open(f_name, "rb") as f:
            contents = f.read()
    finally:
        os.unlink(f_name)

    return contents


def _get_otadata_status(otadata_contents):
    status = []

    otadata_status = collections.namedtuple("otadata_status", "seq crc")

    for i in range(2):
        start = i * (SPI_FLASH_SEC_SIZE >> 1)

        seq = bytearray(otadata_contents[start:start + 4])
        crc = bytearray(otadata_contents[start + 28:start + 32])

        seq = struct.unpack('>I', seq)
        crc = struct.unpack('>I', crc)

        status.append(otadata_status(seq[0], crc[0]))

    return status


def read_otadata(args):
    status("Reading ota_data partition contents...")
    otadata_info = _get_otadata_contents(args)
    otadata_info = _get_otadata_status(otadata_info)

    print(otadata_info)

    print("\t\t{:11}\t{:8s}|\t{:8s}\t{:8s}".format("OTA_SEQ", "CRC", "OTA_SEQ", "CRC"))
    print("Firmware:  0x{:8x}  \t 0x{:8x} |\t0x{:8x} \t 0x{:8x}".format(otadata_info[0].seq, otadata_info[0].crc,
                                                                        otadata_info[1].seq, otadata_info[1].crc))


def erase_otadata(args):
    status("Erasing ota_data partition contents...")
    _invoke_parttool(["erase_partition"], args)
    status("Erased ota_data partition contents")


def switch_otadata(args):
    sys.path.append(os.path.join(IDF_COMPONENTS_PATH, "partition_table"))
    import gen_esp32part as gen

    with tempfile.NamedTemporaryFile(delete=False) as f:
        f_name = f.name

    try:
        def is_otadata_status_valid(status):
            seq = status.seq % (1 << 32)
            crc = hex(binascii.crc32(struct.pack("I", seq), 0xFFFFFFFF) % (1 << 32))
            return seq < (int('0xFFFFFFFF', 16) % (1 << 32)) and status.crc == crc

        status("Looking for ota app partitions...")

        # In order to get the number of ota app partitions, we need the partition table
        partition_table = None
        invoke_args = ["get_partition_info", "--table", f_name]

        _invoke_parttool(invoke_args, args)

        partition_table = open(f_name, "rb").read()
        partition_table = gen.PartitionTable.from_binary(partition_table)

        ota_partitions = list()

        for i in range(gen.NUM_PARTITION_SUBTYPE_APP_OTA):
            ota_partition = filter(lambda p: p.subtype == (gen.MIN_PARTITION_SUBTYPE_APP_OTA + i), partition_table)

            try:
                ota_partitions.append(list(ota_partition)[0])
            except IndexError:
                break

        ota_partitions = sorted(ota_partitions, key=lambda p: p.subtype)

        if not ota_partitions:
            raise RuntimeError("No ota app partitions found")

        status("Verifying partition to switch to exists...")

        # Look for the app partition to switch to
        ota_partition_next = None

        try:
            if args.name:
                ota_partition_next = filter(lambda p: p.name == args.name, ota_partitions)
            else:
                ota_partition_next = filter(lambda p: p.subtype - gen.MIN_PARTITION_SUBTYPE_APP_OTA  == args.slot, ota_partitions)

            ota_partition_next = list(ota_partition_next)[0]
        except IndexError:
            raise RuntimeError("Partition to switch to not found")

        otadata_contents = _get_otadata_contents(args)
        otadata_status = _get_otadata_status(otadata_contents)

        # Find the copy to base the computation for ota sequence number on
        otadata_compute_base = -1

        # Both are valid, take the max as computation base
        if is_otadata_status_valid(otadata_status[0]) and is_otadata_status_valid(otadata_status[1]):
            if otadata_status[0].seq >= otadata_status[1].seq:
                otadata_compute_base = 0
            else:
                otadata_compute_base = 1
        # Only one copy is valid, use that
        elif is_otadata_status_valid(otadata_status[0]):
            otadata_compute_base = 0
        elif is_otadata_status_valid(otadata_status[1]):
            otadata_compute_base = 1
        # Both are invalid (could be initial state - all 0xFF's)
        else:
            pass

        ota_seq_next = 0
        ota_partitions_num = len(ota_partitions)

        target_seq = (ota_partition_next.subtype & 0x0F) + 1

        # Find the next ota sequence number
        if otadata_compute_base == 0 or otadata_compute_base == 1:
            base_seq = otadata_status[otadata_compute_base].seq % (1 << 32)

            i = 0
            while base_seq > target_seq % ota_partitions_num + i * ota_partitions_num:
                i += 1

            ota_seq_next = target_seq % ota_partitions_num + i * ota_partitions_num
        else:
            ota_seq_next = target_seq

        # Create binary data from computed values
        ota_seq_next = struct.pack("I", ota_seq_next)
        ota_seq_crc_next = binascii.crc32(ota_seq_next, 0xFFFFFFFF) % (1 << 32)
        ota_seq_crc_next = struct.pack("I", ota_seq_crc_next)

        with open(f_name, "wb") as otadata_next_file:
            start = (1 if otadata_compute_base == 0 else 0) * (SPI_FLASH_SEC_SIZE >> 1)

            otadata_next_file.write(otadata_contents)

            otadata_next_file.seek(start)
            otadata_next_file.write(ota_seq_next)

            otadata_next_file.seek(start + 28)
            otadata_next_file.write(ota_seq_crc_next)

            otadata_next_file.flush()

        _invoke_parttool(["write_partition", "--input", f_name], args)
        status("Updated ota_data partition")
    finally:
        os.unlink(f_name)


def _get_partition_specifier(args):
    if args.name:
        return ["--partition-name", args.name]
    else:
        return ["--partition-type", "app", "--partition-subtype", "ota_" + str(args.slot)]


def read_ota_partition(args):
    invoke_args = ["read_partition", "--output", args.output]
    _invoke_parttool(invoke_args, args, partition=_get_partition_specifier(args))
    status("Read ota partition contents to file {}".format(args.output))


def write_ota_partition(args):
    invoke_args = ["write_partition", "--input", args.input]
    _invoke_parttool(invoke_args, args, partition=_get_partition_specifier(args))
    status("Written contents of file {} to ota partition".format(args.input))


def erase_ota_partition(args):
    invoke_args = ["erase_partition"]
    _invoke_parttool(invoke_args, args, partition=_get_partition_specifier(args))
    status("Erased contents of ota partition")


def main():
    global quiet

    parser = argparse.ArgumentParser("ESP-IDF OTA Partitions Tool")

    parser.add_argument("--quiet", "-q", help="suppress stderr messages", action="store_true")

    # There are two possible sources for the partition table: a device attached to the host
    # or a partition table CSV/binary file. These sources are mutually exclusive.
    partition_table_info_source_args = parser.add_mutually_exclusive_group()

    partition_table_info_source_args.add_argument("--port", "-p", help="port where the device to read the partition table from is attached", default="")
    partition_table_info_source_args.add_argument("--partition-table-file", "-f", help="file (CSV/binary) to read the partition table from", default="")

    parser.add_argument("--partition-table-offset", "-o", help="offset to read the partition table from", default="0x8000")

    subparsers = parser.add_subparsers(dest="operation", help="run otatool -h for additional help")

    # Specify the supported operations
    subparsers.add_parser("read_otadata", help="read otadata partition")
    subparsers.add_parser("erase_otadata", help="erase otadata partition")

    slot_or_name_parser = argparse.ArgumentParser(add_help=False)
    slot_or_name_parser_args = slot_or_name_parser.add_mutually_exclusive_group()
    slot_or_name_parser_args.add_argument("--slot", help="slot number of the ota partition", type=int)
    slot_or_name_parser_args.add_argument("--name", help="name of the ota partition")

    subparsers.add_parser("switch_otadata", help="switch otadata partition", parents=[slot_or_name_parser])

    read_ota_partition_subparser = subparsers.add_parser("read_ota_partition", help="read contents of an ota partition", parents=[slot_or_name_parser])
    read_ota_partition_subparser.add_argument("--output", help="file to write the contents of the ota partition to")

    write_ota_partition_subparser = subparsers.add_parser("write_ota_partition", help="write contents to an ota partition", parents=[slot_or_name_parser])
    write_ota_partition_subparser.add_argument("--input", help="file whose contents to write to the ota partition")

    subparsers.add_parser("erase_ota_partition", help="erase contents of an ota partition", parents=[slot_or_name_parser])

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
