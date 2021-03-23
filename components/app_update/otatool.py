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
import tempfile
import collections
import struct

try:
    from parttool import PartitionName, PartitionType, ParttoolTarget, PARTITION_TABLE_OFFSET
except ImportError:
    COMPONENTS_PATH = os.path.expandvars(os.path.join("$IDF_PATH", "components"))
    PARTTOOL_DIR = os.path.join(COMPONENTS_PATH, "partition_table")

    sys.path.append(PARTTOOL_DIR)
    from parttool import PartitionName, PartitionType, ParttoolTarget, PARTITION_TABLE_OFFSET

__version__ = '2.0'

SPI_FLASH_SEC_SIZE = 0x2000

quiet = False


def status(msg):
    if not quiet:
        print(msg)


class OtatoolTarget():

    OTADATA_PARTITION = PartitionType("data", "ota")

    def __init__(self, port=None, baud=None, partition_table_offset=PARTITION_TABLE_OFFSET, partition_table_file=None,
                 spi_flash_sec_size=SPI_FLASH_SEC_SIZE, esptool_args=[], esptool_write_args=[],
                 esptool_read_args=[], esptool_erase_args=[]):
        self.target = ParttoolTarget(port, baud, partition_table_offset, partition_table_file, esptool_args,
                                     esptool_write_args, esptool_read_args, esptool_erase_args)
        self.spi_flash_sec_size = spi_flash_sec_size

        temp_file = tempfile.NamedTemporaryFile(delete=False)
        temp_file.close()
        try:
            self.target.read_partition(OtatoolTarget.OTADATA_PARTITION, temp_file.name)
            with open(temp_file.name, "rb") as f:
                self.otadata = f.read()
        except Exception:
            self.otadata = None
        finally:
            os.unlink(temp_file.name)

    def _check_otadata_partition(self):
        if not self.otadata:
            raise Exception("No otadata partition found")

    def erase_otadata(self):
        self._check_otadata_partition()
        self.target.erase_partition(OtatoolTarget.OTADATA_PARTITION)

    def _get_otadata_info(self):
        info = []

        otadata_info = collections.namedtuple("otadata_info", "seq crc")

        for i in range(2):
            start = i * (self.spi_flash_sec_size >> 1)

            seq = bytearray(self.otadata[start:start + 4])
            crc = bytearray(self.otadata[start + 28:start + 32])

            seq = struct.unpack('I', seq)
            crc = struct.unpack('I', crc)
            info.append(otadata_info(seq[0], crc[0]))

        return info

    def _get_partition_id_from_ota_id(self, ota_id):
        if isinstance(ota_id, int):
            return PartitionType("app", "ota_" + str(ota_id))
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
            raise Exception("No ota app partitions found")

        # Look for the app partition to switch to
        ota_partition_next = None

        try:
            if isinstance(ota_id, int):
                ota_partition_next = filter(lambda p: p.subtype - gen.MIN_PARTITION_SUBTYPE_APP_OTA  == ota_id, ota_partitions)
            else:
                ota_partition_next = filter(lambda p: p.name == ota_id, ota_partitions)

            ota_partition_next = list(ota_partition_next)[0]
        except IndexError:
            raise Exception("Partition to switch to not found")

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
        ota_seq_next = struct.pack("I", ota_seq_next)
        ota_seq_crc_next = binascii.crc32(ota_seq_next, 0xFFFFFFFF) % (1 << 32)
        ota_seq_crc_next = struct.pack("I", ota_seq_crc_next)

        temp_file = tempfile.NamedTemporaryFile(delete=False)
        temp_file.close()

        try:
            with open(temp_file.name, "wb") as otadata_next_file:
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

    def write_ota_partition(self, ota_id, input):
        self.target.write_partition(self._get_partition_id_from_ota_id(ota_id), input)

    def erase_ota_partition(self, ota_id):
        self.target.erase_partition(self._get_partition_id_from_ota_id(ota_id))


def _read_otadata(target):
    target._check_otadata_partition()

    otadata_info = target._get_otadata_info()

    print('             {:8s} \t  {:8s} | \t  {:8s} \t   {:8s}'.format('OTA_SEQ', 'CRC', 'OTA_SEQ', 'CRC'))
    print('Firmware:  0x{:08x} \t0x{:08x} | \t0x{:08x} \t 0x{:08x}'.format(otadata_info[0].seq, otadata_info[0].crc,
          otadata_info[1].seq, otadata_info[1].crc))


def _erase_otadata(target):
    target.erase_otadata()
    status("Erased ota_data partition contents")


def _switch_ota_partition(target, ota_id):
    target.switch_ota_partition(ota_id)


def _read_ota_partition(target, ota_id, output):
    target.read_ota_partition(ota_id, output)
    status("Read ota partition contents to file {}".format(output))


def _write_ota_partition(target, ota_id, input):
    target.write_ota_partition(ota_id, input)
    status("Written contents of file {} to ota partition".format(input))


def _erase_ota_partition(target, ota_id):
    target.erase_ota_partition(ota_id)
    status("Erased contents of ota partition")


def main():
    if sys.version_info[0] < 3:
        print("WARNING: Support for Python 2 is deprecated and will be removed in future versions.", file=sys.stderr)
    elif sys.version_info[0] == 3 and sys.version_info[1] < 6:
        print("WARNING: Python 3 versions older than 3.6 are not supported.", file=sys.stderr)
    global quiet

    parser = argparse.ArgumentParser("ESP-IDF OTA Partitions Tool")

    parser.add_argument("--quiet", "-q", help="suppress stderr messages", action="store_true")
    parser.add_argument("--esptool-args", help="additional main arguments for esptool", nargs="+")
    parser.add_argument("--esptool-write-args", help="additional subcommand arguments for esptool write_flash", nargs="+")
    parser.add_argument("--esptool-read-args", help="additional subcommand arguments for esptool read_flash", nargs="+")
    parser.add_argument("--esptool-erase-args", help="additional subcommand arguments for esptool erase_region", nargs="+")

    # There are two possible sources for the partition table: a device attached to the host
    # or a partition table CSV/binary file. These sources are mutually exclusive.
    parser.add_argument("--port", "-p", help="port where the device to read the partition table from is attached")

    parser.add_argument("--baud", "-b", help="baudrate to use", type=int)

    parser.add_argument("--partition-table-offset", "-o", help="offset to read the partition table from",  type=str)

    parser.add_argument("--partition-table-file", "-f", help="file (CSV/binary) to read the partition table from; \
                                                            overrides device attached to specified port as the partition table source when defined")

    subparsers = parser.add_subparsers(dest="operation", help="run otatool -h for additional help")

    spi_flash_sec_size = argparse.ArgumentParser(add_help=False)
    spi_flash_sec_size.add_argument("--spi-flash-sec-size", help="value of SPI_FLASH_SEC_SIZE macro", type=str)

    # Specify the supported operations
    subparsers.add_parser("read_otadata", help="read otadata partition", parents=[spi_flash_sec_size])
    subparsers.add_parser("erase_otadata", help="erase otadata partition")

    slot_or_name_parser = argparse.ArgumentParser(add_help=False)
    slot_or_name_parser_args = slot_or_name_parser.add_mutually_exclusive_group()
    slot_or_name_parser_args.add_argument("--slot", help="slot number of the ota partition", type=int)
    slot_or_name_parser_args.add_argument("--name", help="name of the ota partition")

    subparsers.add_parser("switch_ota_partition", help="switch otadata partition", parents=[slot_or_name_parser, spi_flash_sec_size])

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

    target_args = {}

    if args.port:
        target_args["port"] = args.port

    if args.partition_table_file:
        target_args["partition_table_file"] = args.partition_table_file

    if args.partition_table_offset:
        target_args["partition_table_offset"] = int(args.partition_table_offset, 0)

    try:
        if args.spi_flash_sec_size:
            target_args["spi_flash_sec_size"] = int(args.spi_flash_sec_size, 0)
    except AttributeError:
        pass

    if args.esptool_args:
        target_args["esptool_args"] = args.esptool_args

    if args.esptool_write_args:
        target_args["esptool_write_args"] = args.esptool_write_args

    if args.esptool_read_args:
        target_args["esptool_read_args"] = args.esptool_read_args

    if args.esptool_erase_args:
        target_args["esptool_erase_args"] = args.esptool_erase_args

    if args.baud:
        target_args["baud"] = args.baud

    target = OtatoolTarget(**target_args)

    # Create the operation table and execute the operation
    common_args = {'target':target}

    ota_id = []

    try:
        if args.name is not None:
            ota_id = ["name"]
        else:
            if args.slot is not None:
                ota_id = ["slot"]
    except AttributeError:
        pass

    otatool_ops = {
        'read_otadata':(_read_otadata, []),
        'erase_otadata':(_erase_otadata, []),
        'switch_ota_partition':(_switch_ota_partition, ota_id),
        'read_ota_partition':(_read_ota_partition, ["output"] + ota_id),
        'write_ota_partition':(_write_ota_partition, ["input"] + ota_id),
        'erase_ota_partition':(_erase_ota_partition, ota_id)
    }

    (op, op_args) = otatool_ops[args.operation]

    for op_arg in op_args:
        common_args.update({op_arg:vars(args)[op_arg]})

    try:
        common_args['ota_id'] = common_args.pop('name')
    except KeyError:
        try:
            common_args['ota_id'] = common_args.pop('slot')
        except KeyError:
            pass

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
