#!/usr/bin/env python
#
# gen_otadata prints info about the otadata partition.
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

def little_endian(buff, offset):
    data = buff[offset:offset+4]
    data.reverse()
    data = ''.join(data)
    return data
               
def main():
    global quiet
    parser = argparse.ArgumentParser(description='Prints otadata partition in human readable form.')

    parser.add_argument('--quiet', '-q', help="Don't print status messages to stderr", action='store_true')

    search_type = parser.add_mutually_exclusive_group()

    parser.add_argument('input', help='Path to binary file containing otadata partition to parse.',
                        type=argparse.FileType('rb'))

    args = parser.parse_args()

    quiet = args.quiet

    input = args.input.read()
    
    hex_input_0 = binascii.hexlify(input)
    hex_input_0 = map(''.join, zip(*[iter(hex_input_0)]*2))
    hex_input_1 = binascii.hexlify(input[4096:])
    hex_input_1 = map(''.join, zip(*[iter(hex_input_1)]*2))
    
    print("\t%11s\t%8s    |\t%8s\t%8s" %("OTA_SEQ", "CRC", "OTA_SEQ", "CRC"))
    print("Firmware:  0x%s  \t 0x%s |\t0x%s \t 0x%s" % (little_endian(hex_input_0, 0), little_endian(hex_input_0, 28), \
                                                        little_endian(hex_input_1, 0), little_endian(hex_input_1, 28)))
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
