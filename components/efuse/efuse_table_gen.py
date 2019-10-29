#!/usr/bin/env python
#
# ESP32 efuse table generation tool
#
# Converts efuse table to header file efuse_table.h.
#
# Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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
import sys
import hashlib

__version__ = '1.0'

quiet = False
max_blk_len = 256
idf_target = "esp32"

copyright = '''// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at",
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License
'''


def status(msg):
    """ Print status message to stderr """
    if not quiet:
        critical(msg)


def critical(msg):
    """ Print critical message to stderr """
    sys.stderr.write(msg)
    sys.stderr.write('\n')


class FuseTable(list):
    def __init__(self):
        super(FuseTable, self).__init__(self)
        self.md5_digest_table = ""

    @classmethod
    def from_csv(cls, csv_contents):
        res = FuseTable()
        lines = csv_contents.splitlines()

        def expand_vars(f):
            f = os.path.expandvars(f)
            m = re.match(r'(?<!\\)\$([A-Za-z_][A-Za-z0-9_]*)', f)
            if m:
                raise InputError("unknown variable '%s'" % (m.group(1)))
            return f

        for line_no in range(len(lines)):
            line = expand_vars(lines[line_no]).strip()
            if line.startswith("#") or len(line) == 0:
                continue
            try:
                res.append(FuseDefinition.from_csv(line))
            except InputError as e:
                raise InputError("Error at line %d: %s" % (line_no + 1, e))
            except Exception:
                critical("Unexpected error parsing line %d: %s" % (line_no + 1, line))
                raise

        # fix up missing bit_start
        last_efuse_block = None
        for e in res:
            if last_efuse_block != e.efuse_block:
                last_end = 0
            if e.bit_start is None:
                e.bit_start = last_end
            last_end = e.bit_start + e.bit_count
            last_efuse_block = e.efuse_block

        res.verify_duplicate_name()

        # fix up missing field_name
        last_field = None
        for e in res:
            if e.field_name == "" and last_field is None:
                raise InputError("Error at line %d: %s missing field name" % (line_no + 1, e))
            elif e.field_name == "" and last_field is not None:
                e.field_name = last_field.field_name
            last_field = e

        # fill group
        names = [p.field_name for p in res]
        duplicates = set(n for n in names if names.count(n) > 1)
        if len(duplicates) != 0:
            i_count = 0
            for p in res:
                if len(duplicates.intersection([p.field_name])) != 0:
                    p.group = str(i_count)
                    i_count += 1
                else:
                    i_count = 0
        res.verify_duplicate_name()

        # clac md5 for table
        res.calc_md5()

        return res

    def verify_duplicate_name(self):
        # check on duplicate name
        names = [p.field_name for p in self]
        duplicates = set(n for n in names if names.count(n) > 1)

        # print sorted duplicate partitions by name
        if len(duplicates) != 0:
            fl_error = False
            for p in self:
                field_name = p.field_name + p.group
                if field_name != "" and len(duplicates.intersection([field_name])) != 0:
                    fl_error = True
                    print("Field at %s, %s, %s, %s have dublicate field_name" %
                          (p.field_name, p.efuse_block, p.bit_start, p.bit_count))
            if fl_error is True:
                raise InputError("Field names must be unique")

    def verify(self, type_table=None):
        for p in self:
            p.verify(type_table)

        self.verify_duplicate_name()

        # check for overlaps
        last = None
        for p in sorted(self, key=lambda x:(x.efuse_block, x.bit_start)):
            if last is not None and last.efuse_block == p.efuse_block and p.bit_start < last.bit_start + last.bit_count:
                raise InputError("Field at %s, %s, %s, %s overlaps %s, %s, %s, %s" %
                                 (p.field_name, p.efuse_block, p.bit_start, p.bit_count,
                                  last.field_name, last.efuse_block, last.bit_start, last.bit_count))
            last = p

    def calc_md5(self):
        txt_table = ''
        for p in self:
            txt_table += "%s %s %d %s %s" % (p.field_name, p.efuse_block, p.bit_start, str(p.get_bit_count()), p.comment) + "\n"
        self.md5_digest_table = hashlib.md5(txt_table.encode('utf-8')).hexdigest()

    def show_range_used_bits(self):
        # print used and free bits
        rows = ''
        rows += 'Sorted efuse table:\n'
        num = 1
        rows += "{0} \t{1:<30} \t{2} \t{3} \t{4}".format("#", "field_name", "efuse_block", "bit_start", "bit_count") + "\n"
        for p in sorted(self, key=lambda x:(x.efuse_block, x.bit_start)):
            rows += "{0} \t{1:<30} \t{2} \t{3:^8} \t{4:^8}".format(num, p.field_name, p.efuse_block, p.bit_start, p.bit_count) + "\n"
            num += 1

        rows += '\nUsed bits in efuse table:\n'
        last = None
        for p in sorted(self, key=lambda x:(x.efuse_block, x.bit_start)):
            if last is None:
                rows += '%s \n[%d ' % (p.efuse_block, p.bit_start)
            if last is not None:
                if last.efuse_block != p.efuse_block:
                    rows += '%d] \n\n%s \n[%d ' % (last.bit_start + last.bit_count - 1, p.efuse_block, p.bit_start)
                elif last.bit_start + last.bit_count != p.bit_start:
                    rows += '%d] [%d ' % (last.bit_start + last.bit_count - 1, p.bit_start)
            last = p
        rows += '%d] \n' % (last.bit_start + last.bit_count - 1)
        rows += '\nNote: Not printed ranges are free for using. (bits in EFUSE_BLK0 are reserved for Espressif)\n'
        return rows

    def get_str_position_last_free_bit_in_blk(self, blk):
        last_used_bit = 0
        for p in self:
            if p.efuse_block == blk:
                if p.define is not None:
                    return p.get_bit_count()
                else:
                    if last_used_bit < p.bit_start + p.bit_count:
                        last_used_bit = p.bit_start + p.bit_count
        if last_used_bit == 0:
            return None
        return str(last_used_bit)

    def to_header(self, file_name):
        rows = [copyright]
        rows += ["#ifdef __cplusplus",
                 'extern "C" {',
                 "#endif",
                 "",
                 "",
                 "// md5_digest_table " + self.md5_digest_table,
                 "// This file was generated from the file " + file_name + ".csv. DO NOT CHANGE THIS FILE MANUALLY.",
                 "// If you want to change some fields, you need to change " + file_name + ".csv file",
                 "// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.",
                 "// To show efuse_table run the command 'show_efuse_table'.",
                 "",
                 ""]

        last_field_name = ''
        for p in self:
            if (p.field_name != last_field_name):
                rows += ["extern const esp_efuse_desc_t* " + "ESP_EFUSE_" + p.field_name + "[];"]
                last_field_name = p.field_name

        rows += ["",
                 "#ifdef __cplusplus",
                 "}",
                 "#endif",
                 ""]
        return '\n'.join(rows) + "\n"

    def to_c_file(self, file_name, debug):
        rows = [copyright]
        rows += ['#include "sdkconfig.h"',
                 '#include "esp_efuse.h"',
                 '#include <assert.h>',
                 '#include "' + file_name + '.h"',
                 "",
                 "// md5_digest_table " + self.md5_digest_table,
                 "// This file was generated from the file " + file_name + ".csv. DO NOT CHANGE THIS FILE MANUALLY.",
                 "// If you want to change some fields, you need to change " + file_name + ".csv file",
                 "// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.",
                 "// To show efuse_table run the command 'show_efuse_table'."]

        rows += [""]

        if idf_target == "esp32":
            rows += ["#define MAX_BLK_LEN CONFIG_EFUSE_MAX_BLK_LEN"]

            rows += [""]

            last_free_bit_blk1 = self.get_str_position_last_free_bit_in_blk("EFUSE_BLK1")
            last_free_bit_blk2 = self.get_str_position_last_free_bit_in_blk("EFUSE_BLK2")
            last_free_bit_blk3 = self.get_str_position_last_free_bit_in_blk("EFUSE_BLK3")

            rows += ["// The last free bit in the block is counted over the entire file."]
            if last_free_bit_blk1 is not None:
                rows += ["#define LAST_FREE_BIT_BLK1 " + last_free_bit_blk1]
            if last_free_bit_blk2 is not None:
                rows += ["#define LAST_FREE_BIT_BLK2 " + last_free_bit_blk2]
            if last_free_bit_blk3 is not None:
                rows += ["#define LAST_FREE_BIT_BLK3 " + last_free_bit_blk3]

            rows += [""]

            if last_free_bit_blk1 is not None:
                rows += ['_Static_assert(LAST_FREE_BIT_BLK1 <= MAX_BLK_LEN, "The eFuse table does not match the coding scheme. '
                         'Edit the table and restart the efuse_common_table or efuse_custom_table command to regenerate the new files.");']
            if last_free_bit_blk2 is not None:
                rows += ['_Static_assert(LAST_FREE_BIT_BLK2 <= MAX_BLK_LEN, "The eFuse table does not match the coding scheme. '
                         'Edit the table and restart the efuse_common_table or efuse_custom_table command to regenerate the new files.");']
            if last_free_bit_blk3 is not None:
                rows += ['_Static_assert(LAST_FREE_BIT_BLK3 <= MAX_BLK_LEN, "The eFuse table does not match the coding scheme. '
                         'Edit the table and restart the efuse_common_table or efuse_custom_table command to regenerate the new files.");']

            rows += [""]

        last_name = ''
        for p in self:
            if (p.field_name != last_name):
                if last_name != '':
                    rows += ["};\n"]
                rows += ["static const esp_efuse_desc_t " + p.field_name + "[] = {"]
                last_name = p.field_name
            rows += [p.to_struct(debug) + ","]
        rows += ["};\n"]
        rows += ["\n\n\n"]

        last_name = ''
        for p in self:
            if (p.field_name != last_name):
                if last_name != '':
                    rows += ["    NULL",
                             "};\n"]
                rows += ["const esp_efuse_desc_t* " + "ESP_EFUSE_" + p.field_name + "[] = {"]
            last_name = p.field_name
            index = str(0) if str(p.group) == "" else str(p.group)
            rows += ["    &" + p.field_name + "[" + index + "],    \t\t// " + p.comment]
        rows += ["    NULL",
                 "};\n"]

        return '\n'.join(rows) + "\n"


class FuseDefinition(object):
    def __init__(self):
        self.field_name = ""
        self.group = ""
        self.efuse_block = ""
        self.bit_start = None
        self.bit_count = None
        self.define = None
        self.comment = ""

    @classmethod
    def from_csv(cls, line):
        """ Parse a line from the CSV """
        line_w_defaults = line + ",,,,"  # lazy way to support default fields
        fields = [f.strip() for f in line_w_defaults.split(",")]

        res = FuseDefinition()
        res.field_name = fields[0]
        res.efuse_block = res.parse_block(fields[1])
        res.bit_start = res.parse_num(fields[2])
        res.bit_count = res.parse_bit_count(fields[3])
        if res.bit_count is None or res.bit_count == 0:
            raise InputError("Field bit_count can't be empty")
        res.comment = fields[4]
        return res

    def parse_num(self, strval):
        if strval == "":
            return None  # Field will fill in default
        return self.parse_int(strval)

    def parse_bit_count(self, strval):
        if strval == "MAX_BLK_LEN":
            self.define = strval
            return self.get_max_bits_of_block()
        else:
            return self.parse_num(strval)

    def parse_int(self, v):
        try:
            return int(v, 0)
        except ValueError:
            raise InputError("Invalid field value %s" % v)

    def parse_block(self, strval):
        if strval == "":
            raise InputError("Field 'efuse_block' can't be left empty.")
        if idf_target == "esp32":
            if strval not in ["EFUSE_BLK0", "EFUSE_BLK1", "EFUSE_BLK2", "EFUSE_BLK3"]:
                raise InputError("Field 'efuse_block' should be one of EFUSE_BLK0..EFUSE_BLK3")

        if idf_target == "esp32s2beta":
            if strval not in ["EFUSE_BLK0", "EFUSE_BLK1", "EFUSE_BLK2", "EFUSE_BLK3", "EFUSE_BLK4",
                              "EFUSE_BLK5", "EFUSE_BLK6", "EFUSE_BLK7", "EFUSE_BLK8", "EFUSE_BLK9",
                              "EFUSE_BLK10"]:
                raise InputError("Field 'efuse_block' should be one of EFUSE_BLK0..EFUSE_BLK10")

        return strval

    def get_max_bits_of_block(self):
        '''common_table: EFUSE_BLK0, EFUSE_BLK1, EFUSE_BLK2, EFUSE_BLK3
           custom_table: ----------, ----------, ----------, EFUSE_BLK3(some reserved in common_table)
        '''
        if self.efuse_block == "EFUSE_BLK0":
            return 256
        else:
            return max_blk_len

    def verify(self, type_table):
        if self.efuse_block is None:
            raise ValidationError(self, "efuse_block field is not set")
        if self.bit_count is None:
            raise ValidationError(self, "bit_count field is not set")

        if type_table is not None:
            if type_table == "custom_table":
                if self.efuse_block != "EFUSE_BLK3":
                    raise ValidationError(self, "custom_table should use only EFUSE_BLK3")

        max_bits = self.get_max_bits_of_block()

        if self.bit_start + self.bit_count > max_bits:
            raise ValidationError(self, "The field is outside the boundaries(max_bits = %d) of the %s block" % (max_bits, self.efuse_block))

    def get_full_name(self):
        def get_postfix(group):
            postfix = ""
            if group != "":
                postfix = "_PART_" + group
            return postfix

        return self.field_name + get_postfix(self.group)

    def get_bit_count(self, check_define=True):
        if check_define is True and self.define is not None:
            return self.define
        else:
            return self.bit_count

    def to_struct(self, debug):
        start = "    {"
        if debug is True:
            start = "    {" + '"' + self.field_name + '" ,'
        return ", ".join([start + self.efuse_block,
                         str(self.bit_start),
                         str(self.get_bit_count()) + "}, \t // " + self.comment])


def process_input_file(file, type_table):
    status("Parsing efuse CSV input file " + file.name + " ...")
    input = file.read()
    table = FuseTable.from_csv(input)
    status("Verifying efuse table...")
    table.verify(type_table)
    return table


def ckeck_md5_in_file(md5, filename):
    if os.path.exists(filename):
        with open(filename, 'r') as f:
            for line in f:
                if md5 in line:
                    return True
    return False


def create_output_files(name, output_table, debug):
    file_name = os.path.splitext(os.path.basename(name))[0]
    gen_dir = os.path.dirname(name)

    dir_for_file_h = gen_dir + "/include"
    try:
        os.stat(dir_for_file_h)
    except Exception:
        os.mkdir(dir_for_file_h)

    file_h_path = os.path.join(dir_for_file_h, file_name + ".h")
    file_c_path = os.path.join(gen_dir, file_name + ".c")

    # src files are the same
    if ckeck_md5_in_file(output_table.md5_digest_table, file_c_path) is False:
        status("Creating efuse *.h file " + file_h_path + " ...")
        output = output_table.to_header(file_name)
        with open(file_h_path, 'w') as f:
            f.write(output)

        status("Creating efuse *.c file " + file_c_path + " ...")
        output = output_table.to_c_file(file_name, debug)
        with open(file_c_path, 'w') as f:
            f.write(output)
    else:
        print("Source files do not require updating correspond to csv file.")


def main():
    global quiet
    global max_blk_len
    global idf_target

    parser = argparse.ArgumentParser(description='ESP32 eFuse Manager')
    parser.add_argument('--idf_target', '-t', help='Target chip type', choices=['esp32','esp32s2beta'], default='esp32')
    parser.add_argument('--quiet', '-q', help="Don't print non-critical status messages to stderr", action='store_true')
    parser.add_argument('--debug', help='Create header file with debug info', default=False, action="store_false")
    parser.add_argument('--info', help='Print info about range of used bits', default=False, action="store_true")
    parser.add_argument('--max_blk_len', help='Max number of bits in BLOCKs', type=int, default=256)
    parser.add_argument('common_input', help='Path to common CSV file to parse.', type=argparse.FileType('r'))
    parser.add_argument('custom_input', help='Path to custom CSV file to parse.', type=argparse.FileType('r'), nargs='?', default=None)

    args = parser.parse_args()

    idf_target = args.idf_target

    max_blk_len = args.max_blk_len
    print("Max number of bits in BLK %d" % (max_blk_len))
    if max_blk_len not in [256, 192, 128]:
        raise InputError("Unsupported block length = %d" % (max_blk_len))

    quiet = args.quiet
    debug = args.debug
    info = args.info

    common_table = process_input_file(args.common_input, "common_table")
    two_table = common_table
    if args.custom_input is not None:
        custom_table = process_input_file(args.custom_input, "custom_table")
        two_table += custom_table
        two_table.verify()

    # save files.
    if info is False:
        if args.custom_input is None:
            create_output_files(args.common_input.name, common_table, debug)
        else:
            create_output_files(args.custom_input.name, custom_table, debug)
    else:
        print(two_table.show_range_used_bits())
    return 0


class InputError(RuntimeError):
    def __init__(self, e):
        super(InputError, self).__init__(e)


class ValidationError(InputError):
    def __init__(self, p, message):
        super(ValidationError, self).__init__("Entry %s invalid: %s" % (p.field_name, message))


if __name__ == '__main__':
    try:
        main()
    except InputError as e:
        print(e, file=sys.stderr)
        sys.exit(2)
