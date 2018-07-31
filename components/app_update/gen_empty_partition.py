#!/usr/bin/env python
#
# generates an empty binary file
# 
# This tool generates an empty binary file of the required size.
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

def generate_blanked_file(size, output_path):
    output = ""
    for i in range(size):
        output += b"\xFF"
    try:
        stdout_binary = sys.stdout.buffer  # Python 3
    except AttributeError:
        stdout_binary = sys.stdout
    with stdout_binary if output_path == '-' else open(output_path, 'wb') as f:
        f.write(output)

def main():
    global quiet
    parser = argparse.ArgumentParser(description='Generates an empty binary file of the required size.')

    parser.add_argument('--quiet', '-q', help="Don't print status messages to stderr", action='store_true')

    parser.add_argument('--size', help='Size of generated the file',  type=str, required=True)
    
    parser.add_argument('output', help='Path for binary file.', nargs='?', default='-')
    args = parser.parse_args()

    quiet = args.quiet

    size = int(args.size, 0)
    if size > 0 :
        generate_blanked_file(size, args.output)
    return 0
    
class InputError(RuntimeError):
    def __init__(self, e):
        super(InputError, self).__init__(e)

if __name__ == '__main__':
    try:
        r = main()
        sys.exit(r)
    except InputError as e:
        print(e, file=sys.stderr)
        sys.exit(2)
