#!/usr/bin/env python
#
# parttool returns info about the required partition.
#
# This utility is used by the make system to get information 
# about the start addresses: partition table, factory area, phy area.
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
import re
import struct
import sys
import hashlib
import binascii
import gen_esp32part as gen

__version__ = '1.0'

quiet = False

def status(msg):
    """ Print status message to stderr """
    if not quiet:
        critical(msg)

def critical(msg):
    """ Print critical message to stderr """
    if not quiet:
        sys.stderr.write(msg)
        sys.stderr.write('\n')
               
def main():
    global quiet
    parser = argparse.ArgumentParser(description='Returns info about the required partition.')

    parser.add_argument('--quiet', '-q', help="Don't print status messages to stderr", action='store_true')

    search_type = parser.add_mutually_exclusive_group()
    search_type.add_argument('--partition-name', '-p', help='The name of the required partition', type=str, default=None)
    search_type.add_argument('--type', '-t', help='The type of the required partition', type=str, default=None)
    search_type.add_argument('--default-boot-partition', help='Select the default boot partition, '+
                             'using the same fallback logic as the IDF bootloader', action="store_true")

    parser.add_argument('--subtype', '-s', help='The subtype of the required partition', type=str, default=None)

    parser.add_argument('--offset', '-o', help='Return offset of required partition', action="store_true")
    parser.add_argument('--size', help='Return size of required partition', action="store_true")

    parser.add_argument('input', help='Path to CSV or binary file to parse. Will use stdin if omitted.',
                        type=argparse.FileType('rb'), default=sys.stdin)

    args = parser.parse_args()

    if args.type is not None and args.subtype is None:
        status("If --type is specified, --subtype is required")
        return 2
    if args.type is None and args.subtype is not None:
        status("--subtype is only used with --type")
        return 2

    quiet = args.quiet

    input = args.input.read()
    input_is_binary = input[0:2] == gen.PartitionDefinition.MAGIC_BYTES
    if input_is_binary:
        status("Parsing binary partition input...")
        table = gen.PartitionTable.from_binary(input)
    else:
        input = input.decode()
        status("Parsing CSV input...")
        table = gen.PartitionTable.from_csv(input)

    found_partition = None

    if args.default_boot_partition:
        search = [ "factory" ] + [ "ota_%d" % d for d in range(16) ]
        for subtype in search:
            found_partition = table.find_by_type("app", subtype)
            if found_partition is not None:
                break
    elif args.partition_name is not None:
        found_partition = table.find_by_name(args.partition_name)
    elif args.type is not None:
        found_partition = table.find_by_type(args.type, args.subtype)
    else:
        raise RuntimeError("invalid partition selection choice")

    if found_partition is None:
        return 1  # nothing found

    if args.offset:
        print('0x%x ' % (found_partition.offset))
    if args.size:
        print('0x%x' % (found_partition.size))

    return 0

class InputError(RuntimeError):
    def __init__(self, e):
        super(InputError, self).__init__(e)


class ValidationError(InputError):
    def __init__(self, partition, message):
        super(ValidationError, self).__init__(
            "Partition %s invalid: %s" % (partition.name, message))


if __name__ == '__main__':
    try:
        r = main()
        sys.exit(r)
    except InputError as e:
        print(e, file=sys.stderr)
        sys.exit(2)
