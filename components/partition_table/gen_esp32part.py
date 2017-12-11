#!/usr/bin/env python
#
# ESP32 partition table generation tool
#
# Converts partition tables to/from CSV and binary formats.
#
# See http://esp-idf.readthedocs.io/en/latest/api-guides/partition-tables.html
# for explanation of partition table structure and uses.
#
# Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

MAX_PARTITION_LENGTH = 0xC00   # 3K for partition data (96 entries) leaves 1K in a 4K sector for signature

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

class PartitionTable(list):
    def __init__(self):
        super(PartitionTable, self).__init__(self)

    @classmethod
    def from_csv(cls, csv_contents):
        res = PartitionTable()
        lines = csv_contents.splitlines()

        def expand_vars(f):
            f = os.path.expandvars(f)
            m = re.match(r'(?<!\\)\$([A-Za-z_][A-Za-z0-9_]*)', f)
            if m:
                raise InputError("unknown variable '%s'" % m.group(1))
            return f

        for line_no in range(len(lines)):
            line = expand_vars(lines[line_no]).strip()
            if line.startswith("#") or len(line) == 0:
                continue
            try:
                res.append(PartitionDefinition.from_csv(line))
            except InputError as e:
                raise InputError("Error at line %d: %s" % (line_no+1, e))
            except Exception:
                critical("Unexpected error parsing line %d: %s" % (line_no+1, line))
                raise

        # fix up missing offsets & negative sizes
        last_end = 0x5000 # first offset after partition table
        for e in res:
            if e.offset is None:
                pad_to = 0x10000 if e.type == PartitionDefinition.APP_TYPE else 4
                if last_end % pad_to != 0:
                    last_end += pad_to - (last_end % pad_to)
                e.offset = last_end
            if e.size < 0:
                e.size = -e.size - e.offset
            last_end = e.offset + e.size

        return res

    def __getitem__(self, item):
        """ Allow partition table access via name as well as by
        numeric index. """
        if isinstance(item, str):
            for x in self:
                if x.name == item:
                    return x
            raise ValueError("No partition entry named '%s'" % item)
        else:
            return super(PartitionTable, self).__getitem__(item)

    def verify(self):
        # verify each partition individually
        for p in self:
            p.verify()
        # check for overlaps
        last = None
        for p in sorted(self, key=lambda x:x.offset):
            if p.offset < 0x5000:
                raise InputError("Partition offset 0x%x is below 0x5000" % p.offset)
            if last is not None and p.offset < last.offset + last.size:
                raise InputError("Partition at 0x%x overlaps 0x%x-0x%x" % (p.offset, last.offset, last.offset+last.size-1))
            last = p

    @classmethod
    def from_binary(cls, b):
        result = cls()
        for o in range(0,len(b),32):
            data = b[o:o+32]
            if len(data) != 32:
                raise InputError("Partition table length must be a multiple of 32 bytes")
            if data == b'\xFF'*32:
                return result  # got end marker
            result.append(PartitionDefinition.from_binary(data))
        raise InputError("Partition table is missing an end-of-table marker")

    def to_binary(self):
        result = b"".join(e.to_binary() for e in self)
        if len(result )>= MAX_PARTITION_LENGTH:
            raise InputError("Binary partition table length (%d) longer than max" % len(result))
        result += b"\xFF" * (MAX_PARTITION_LENGTH - len(result))  # pad the sector, for signing
        return result

    def to_csv(self, simple_formatting=False):
        rows = [ "# Espressif ESP32 Partition Table",
                 "# Name, Type, SubType, Offset, Size, Flags" ]
        rows += [ x.to_csv(simple_formatting) for x in self ]
        return "\n".join(rows) + "\n"

class PartitionDefinition(object):
    APP_TYPE = 0x00
    DATA_TYPE = 0x01
    TYPES = {
        "app" : APP_TYPE,
        "data" : DATA_TYPE,
    }

    # Keep this map in sync with esp_partition_subtype_t enum in esp_partition.h 
    SUBTYPES = {
        APP_TYPE : {
            "factory" : 0x00,
            "test" : 0x20,
            },
        DATA_TYPE : {
            "ota" : 0x00,
            "phy" : 0x01,
            "nvs" : 0x02,
            "coredump" : 0x03,
            "esphttpd" : 0x80,
            "fat" : 0x81,
            "spiffs" : 0x82,
            },
    }

    MAGIC_BYTES = b"\xAA\x50"

    ALIGNMENT = {
        APP_TYPE : 0x10000,
        DATA_TYPE : 0x04,
    }

    # dictionary maps flag name (as used in CSV flags list, property name)
    # to bit set in flags words in binary format
    FLAGS = {
        "encrypted" : 0
    }

    # add subtypes for the 16 OTA slot values ("ota_XXX, etc.")
    for ota_slot in range(16):
        SUBTYPES[TYPES["app"]]["ota_%d" % ota_slot] = 0x10 + ota_slot

    def __init__(self):
        self.name = ""
        self.type = None
        self.subtype = None
        self.offset = None
        self.size = None
        self.encrypted = False

    @classmethod
    def from_csv(cls, line):
        """ Parse a line from the CSV """
        line_w_defaults = line + ",,,,"  # lazy way to support default fields
        fields = [ f.strip() for f in line_w_defaults.split(",") ]

        res = PartitionDefinition()
        res.name = fields[0]
        res.type = res.parse_type(fields[1])
        res.subtype = res.parse_subtype(fields[2])
        res.offset = res.parse_address(fields[3])
        res.size = res.parse_address(fields[4])
        if res.size is None:
            raise InputError("Size field can't be empty")

        flags = fields[5].split(":")
        for flag in flags:
            if flag in cls.FLAGS:
                setattr(res, flag, True)
            elif len(flag) > 0:
                raise InputError("CSV flag column contains unknown flag '%s'" % (flag))

        return res

    def __eq__(self, other):
        return self.name == other.name and self.type == other.type \
            and self.subtype == other.subtype and self.offset == other.offset \
            and self.size == other.size

    def __repr__(self):
        def maybe_hex(x):
            return "0x%x" % x if x is not None else "None"
        return "PartitionDefinition('%s', 0x%x, 0x%x, %s, %s)" % (self.name, self.type, self.subtype or 0,
                                                              maybe_hex(self.offset), maybe_hex(self.size))

    def __str__(self):
        return "Part '%s' %d/%d @ 0x%x size 0x%x" % (self.name, self.type, self.subtype, self.offset or -1, self.size or -1)

    def __cmp__(self, other):
        return self.offset - other.offset

    def parse_type(self, strval):
        if strval == "":
            raise InputError("Field 'type' can't be left empty.")
        return parse_int(strval, self.TYPES)

    def parse_subtype(self, strval):
        if strval == "":
            return 0 # default
        return parse_int(strval, self.SUBTYPES.get(self.type, {}))

    def parse_address(self, strval):
        if strval == "":
            return None  # PartitionTable will fill in default
        return parse_int(strval)

    def verify(self):
        if self.type is None:
            raise ValidationError(self, "Type field is not set")
        if self.subtype is None:
            raise ValidationError(self, "Subtype field is not set")
        if self.offset is None:
            raise ValidationError(self, "Offset field is not set")
        align = self.ALIGNMENT.get(self.type, 4)
        if self.offset % align:
            raise ValidationError(self, "Offset 0x%x is not aligned to 0x%x" % (self.offset, align))
        if self.size is None:
            raise ValidationError(self, "Size field is not set")

    STRUCT_FORMAT = "<2sBBLL16sL"

    @classmethod
    def from_binary(cls, b):
        if len(b) != 32:
            raise InputError("Partition definition length must be exactly 32 bytes. Got %d bytes." % len(b))
        res = cls()
        (magic, res.type, res.subtype, res.offset,
         res.size, res.name, flags) = struct.unpack(cls.STRUCT_FORMAT, b)
        if b"\x00" in res.name: # strip null byte padding from name string
            res.name = res.name[:res.name.index(b"\x00")]
        res.name = res.name.decode()
        if magic != cls.MAGIC_BYTES:
            raise InputError("Invalid magic bytes (%r) for partition definition" % magic)
        for flag,bit in cls.FLAGS.items():
            if flags & (1<<bit):
                setattr(res, flag, True)
                flags &= ~(1<<bit)
        if flags != 0:
            critical("WARNING: Partition definition had unknown flag(s) 0x%08x. Newer binary format?" % flags)
        return res

    def get_flags_list(self):
        return [ flag for flag in self.FLAGS.keys() if getattr(self, flag) ]

    def to_binary(self):
        flags = sum((1 << self.FLAGS[flag]) for flag in self.get_flags_list())
        return struct.pack(self.STRUCT_FORMAT,
                           self.MAGIC_BYTES,
                           self.type, self.subtype,
                           self.offset, self.size,
                           self.name.encode(),
                           flags)

    def to_csv(self, simple_formatting=False):
        def addr_format(a, include_sizes):
            if not simple_formatting and include_sizes:
                for (val, suffix) in [ (0x100000, "M"), (0x400, "K") ]:
                    if a % val == 0:
                        return "%d%s" % (a // val, suffix)
            return "0x%x" % a

        def lookup_keyword(t, keywords):
            for k,v in keywords.items():
                if simple_formatting == False and t == v:
                    return k
            return "%d" % t

        def generate_text_flags():
            """ colon-delimited list of flags """
            return ":".join(self.get_flags_list())

        return ",".join([ self.name,
                          lookup_keyword(self.type, self.TYPES),
                          lookup_keyword(self.subtype, self.SUBTYPES.get(self.type, {})),
                          addr_format(self.offset, False),
                          addr_format(self.size, True),
                          generate_text_flags()])


def parse_int(v, keywords={}):
    """Generic parser for integer fields - int(x,0) with provision for
    k/m/K/M suffixes and 'keyword' value lookup.
    """
    try:
        for letter, multiplier in [ ("k",1024), ("m",1024*1024) ]:
            if v.lower().endswith(letter):
                return parse_int(v[:-1], keywords) * multiplier
        return int(v, 0)
    except ValueError:
        if len(keywords) == 0:
            raise InputError("Invalid field value %s" % v)
        try:
            return keywords[v.lower()]
        except KeyError:
            raise InputError("Value '%s' is not valid. Known keywords: %s" % (v, ", ".join(keywords)))

def main():
    global quiet
    parser = argparse.ArgumentParser(description='ESP32 partition table utility')

    parser.add_argument('--verify', '-v', help='Verify partition table fields', default=True, action='store_false')
    parser.add_argument('--quiet', '-q', help="Don't print status messages to stderr", action='store_true')

    parser.add_argument('input', help='Path to CSV or binary file to parse. Will use stdin if omitted.', type=argparse.FileType('rb'), default=sys.stdin)
    parser.add_argument('output', help='Path to output converted binary or CSV file. Will use stdout if omitted, unless the --display argument is also passed (in which case only the summary is printed.)',
                        nargs='?',
                        default='-')

    args = parser.parse_args()

    quiet = args.quiet
    input = args.input.read()
    input_is_binary = input[0:2] == PartitionDefinition.MAGIC_BYTES
    if input_is_binary:
        status("Parsing binary partition input...")
        table = PartitionTable.from_binary(input)
    else:
        input = input.decode()
        status("Parsing CSV input...")
        table = PartitionTable.from_csv(input)

    if args.verify:
        status("Verifying table...")
        table.verify()

    if input_is_binary:
        output = table.to_csv()
        with sys.stdout if args.output == '-' else open(args.output, 'w') as f:
            f.write(output)
    else:
        output = table.to_binary()
        with sys.stdout.buffer if args.output == '-' else open(args.output, 'wb') as f:
            f.write(output)


class InputError(RuntimeError):
    def __init__(self, e):
        super(InputError, self).__init__(e)


class ValidationError(InputError):
    def __init__(self, partition, message):
        super(ValidationError, self).__init__(
            "Partition %s invalid: %s" % (partition.name, message))


if __name__ == '__main__':
    try:
        main()
    except InputError as e:
        print(e, file=sys.stderr)
        sys.exit(2)
