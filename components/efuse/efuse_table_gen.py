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
import struct
import sys
import hashlib
import binascii
import ntpath

__version__ = '1.0'

quiet = False
coding_scheme = 0
custom_table_use_BLK1 = False
custom_table_use_BLK2 = False
common_table_fix_size = False

CODE_SCHEME = {
    "NONE"      : 0,
    "3/4"       : 1,
    "REPEAT"    : 2,
}

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
    def from_csv(cls, csv_contents, type_table):
        res = FuseTable()
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
                res.append(FuseDefinition.from_csv(line))
            except InputError as e:
                raise InputError("Error at line %d: %s" % (line_no+1, e))
            except Exception:
                critical("Unexpected error parsing line %d: %s" % (line_no+1, line))
                raise

        # fix up missing bit_start
        last_enfl_error = 0
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
                raise InputError("Error at line %d: %s missing field name" % (line_no+1, e))
            elif e.field_name == "" and last_field is not None:
                e.field_name = last_field.field_name
            last_field = e
        
        # fill group
        names = [ p.field_name for p in res ]
        duplicates = set( n for n in names if names.count(n) > 1 )
        if len(duplicates) != 0:
            i_count = 0
            for p in res:
                if len(duplicates.intersection([p.field_name])) != 0:
                    p.group = str(i_count)
                    i_count += 1
                else:
                    i_count = 0
        res.verify_duplicate_name()
        
        # fix size due to coding scheme
        if type_table == "common_table":
            if common_table_fix_size == True and (custom_table_use_BLK1 == False or custom_table_use_BLK2 == False):
                res.fix_size_fields_from_blk1_blk2();
            if custom_table_use_BLK1 == True or custom_table_use_BLK2 == True:
                res.keys_from_blk1_blk2_make_empty();
        
        # clac md5 for table
        res.calc_md5()
        
        return res

    def verify_duplicate_name(self):
        # check on duplicate name
        names = [ p.field_name for p in self ]
        duplicates = set( n for n in names if names.count(n) > 1 )
        
        # print sorted duplicate partitions by name
        if len(duplicates) != 0:
            fl_error = False
            for p in self:
                field_name = p.field_name + p.group
                if field_name != "" and len(duplicates.intersection([field_name])) != 0:
                    fl_error = True
                    print ("Field at %s, %s, %s, %s have dublicate field_name" 
                            % ( p.field_name, p.efuse_block, p.bit_start, p.bit_count))
            if fl_error == True:
                raise InputError("Field names must be unique")
        
    def verify(self, type_table = None):
        for p in self:
            p.verify(type_table)

        self.verify_duplicate_name()
        
        # check for overlaps
        last = None
        for p in sorted(self, key=lambda x:(x.efuse_block, x.bit_start)):
            if last is not None and last.efuse_block == p.efuse_block and p.bit_start < last.bit_start + last.bit_count:
                raise InputError("Field at %s, %s, %s, %s overlaps %s, %s, %s, %s" 
                    % (   p.field_name,    p.efuse_block,    p.bit_start,    p.bit_count, 
                       last.field_name, last.efuse_block, last.bit_start, last.bit_count))
            last = p
    
    def fix_size_fields_from_blk1_blk2(self):
        for p in self:
            if (p.efuse_block == "EFUSE_BLK1" and custom_table_use_BLK1 == False) or (p.efuse_block == "EFUSE_BLK2" and custom_table_use_BLK2 == False):
                max_bits = p.get_max_bits_of_block()
                if p.bit_start == 0 and p.bit_count > max_bits:
                    print("Fix size `%s` field from %d to %d" %(p.field_name, p.bit_count, max_bits))
                    p.bit_count = max_bits
    
    def keys_from_blk1_blk2_make_empty(self):
        for p in self:
            if (p.efuse_block == "EFUSE_BLK1" and custom_table_use_BLK1 == True) or (p.efuse_block == "EFUSE_BLK2" and custom_table_use_BLK2 == True):
                p.bit_count = 0
                print("efuse: `%s` field was changed from %d to 0" %(p.field_name, p.bit_count))
        
    def calc_md5(self):
        txt_table = ''
        for p in self:
            txt_table += "%s %s %d %d %s" % (p.field_name, p.efuse_block, p.bit_start, p.bit_count, p.comment) + "\n"
        self.md5_digest_table = hashlib.md5(txt_table).hexdigest()
        
    def show_range_used_bits(self):        
        # print used and free bits
        rows = ''
        rows += 'Sorted efuse table:\n'
        num = 1
        for p in sorted(self, key=lambda x:(x.efuse_block, x.bit_start)):
            rows += "#%d \t%s \t\t%s \t\t%d \t\t%d" % (num, p.field_name, p.efuse_block, p.bit_start, p.bit_count) + "\n"
            num += 1
        
        rows += '\nUsed bits in efuse table:\n'
        last = None
        for p in sorted(self, key=lambda x:(x.efuse_block, x.bit_start)):
            if last is None:
                rows += '%s \n[%d ' % (p.efuse_block, p.bit_start)
            if  last is not None:
                if last.efuse_block != p.efuse_block:
                    rows += '%d] \n\n%s \n[%d ' % (last.bit_start + last.bit_count - 1, p.efuse_block, p.bit_start)
                elif last.bit_start + last.bit_count != p.bit_start:
                    rows += '%d] [%d ' % (last.bit_start + last.bit_count - 1, p.bit_start)
            last = p
        rows += '%d] \n' % (last.bit_start + last.bit_count - 1)
        rows += '\nNote: Not printed ranges are free for using.\n'
        return rows
        
    def to_header(self, file_name):
        rows = [ copyright ]
        rows += [ "#ifdef __cplusplus",
                 'extern "C" {',
                 "#endif",
                 "",
                 "",
                 "// md5_digest_table " + self.md5_digest_table,
                 "// This file was generated automatically from the file " + file_name + ".csv. DO NOT CHANGE THIS FILE MANUALLY.",
                 "// If you want to change some fields, you need to change " + file_name + ".csv file then build system will generate this header file",
                 "// To show efuse_table run the command 'make show_efuse_table'.",
                 "",
                 ""]
        
        last_field_name = ''
        for p in self:
            if (p.field_name != last_field_name):
                rows += [ "extern const esp_efuse_desc_t* " + "ESP_EFUSE_" + p.field_name + "[];" ]
                last_field_name = p.field_name
        
        rows += [ "",
                  "#ifdef __cplusplus",
                  "}",
                  "#endif",
                  ""]
        return '\n'.join(rows) + "\n"

    def to_c_file(self, file_name, debug):
        rows = [ copyright ]
        rows += [ '#include "esp_efuse.h"',
                 '#include "' + file_name + '.h"',
                 "",
                 "// md5_digest_table " + self.md5_digest_table,
                 "// This file was generated automatically from the file " + file_name + ".csv. DO NOT CHANGE THIS FILE MANUALLY.",
                 "// If you want to change some fields, you need to change " + file_name + ".csv file then build system will generate this header file",
                 "// To show efuse_table run the command 'make show_efuse_table'.",
                 "",
                 ""]
        
        last_name = ''
        for p in self:
            if (p.field_name != last_name):
                if last_name != '':
                    rows += [ "};\n"]
                rows += [ "static const esp_efuse_desc_t " + p.field_name + "[] = {" ]
                last_name = p.field_name
            rows += [ p.to_struct(debug) + "," ]
        rows += [ "};\n" ]
        
        rows += ["\n\n\n"]
        
        last_name = ''
        for p in self:
            if (p.field_name != last_name):
                if last_name != '':
                    rows += ["    NULL",
                             "};\n" ]
                rows += [ "const esp_efuse_desc_t* " + "ESP_EFUSE_" + p.field_name + "[] = {" ]
            last_name = p.field_name
            index = str(0) if str(p.group) == "" else str(p.group)
            rows += [ "    &" + p.field_name + "[" + index + "],    \t\t// " + p.comment ]
        rows += ["    NULL",
                 "};\n" ]
        
        return '\n'.join(rows) + "\n"
    
class FuseDefinition(object):

    def __init__(self):
        self.field_name = ""
        self.group = ""
        self.efuse_block = ""
        self.bit_start = None
        self.bit_count = None
        self.comment = ""

    @classmethod
    def from_csv(cls, line):
        """ Parse a line from the CSV """
        line_w_defaults = line + ",,,,"  # lazy way to support default fields
        fields = [ f.strip() for f in line_w_defaults.split(",") ]

        res = FuseDefinition()
        res.field_name = fields[0]
        res.efuse_block = res.parse_block(fields[1])
        res.bit_start = res.parse_num(fields[2])
        res.bit_count = res.parse_num(fields[3])
        if res.bit_count is None or res.bit_count == 0:
            raise InputError("Field bit_count can't be empty")
        res.comment = fields[4]
        return res

    def parse_num(self, strval):
        if strval == "":
            return None  # Field will fill in default
        return self.parse_int(strval)
    
    def parse_int(self, v):
        try:
            return int(v, 0)
        except ValueError:
            raise InputError("Invalid field value %s" % v)
        
    def parse_block(self, strval):
        if strval == "":
            raise InputError("Field 'efuse_block' can't be left empty.")
        if strval != "EFUSE_BLK0" and strval != "EFUSE_BLK1" and strval != "EFUSE_BLK2" and strval != "EFUSE_BLK3":
            raise InputError("Field 'efuse_block' should consist from EFUSE_BLK0..EFUSE_BLK3")
        return strval

    def get_max_bits_of_block(self):
        '''common_table: EFUSE_BLK0, EFUSE_BLK1, EFUSE_BLK2, EFUSE_BLK3
           custom_table: ----------, ----------, ----------, EFUSE_BLK3(some reserved in common_table)
        '''
        max_bits = 0
        if coding_scheme == CODE_SCHEME["NONE"] or self.efuse_block == "EFUSE_BLK0":
            max_bits = 256
        elif coding_scheme == CODE_SCHEME["3/4"]:
            max_bits = 192
        elif coding_scheme == CODE_SCHEME["REPEAT"]:
            max_bits = 128
        else:
            ValidationError(self, "Unknown coding scheme")
        return max_bits
        
    def verify(self, type_table):
        if self.efuse_block is None:
            raise ValidationError(self, "efuse_block field is not set")
        if self.bit_count is None:
            raise ValidationError(self, "bit_count field is not set")
        
        if type_table is not None:
            if type_table == "custom_table":
                if self.efuse_block != "EFUSE_BLK3":
                    ValidationError(self, "custom_table should use only EFUSE_BLK3")
        
        max_bits = self.get_max_bits_of_block()

        if self.bit_start + self.bit_count > max_bits:
            print("(%d + %d) > %d" % (self.bit_start, self.bit_count, max_bits))
            raise ValidationError(self, "The field is outside the boundaries of the %s block" % (self.efuse_block))
    
    def get_full_name(self):
        def get_postfix(group):
            postfix = ""
            if group != "":
                postfix = "_PART_" + group
            return postfix
        
        return self.field_name + get_postfix(self.group)
    
    def to_struct(self, debug):
        start = "    {"
        if (debug == True):
            start = "    {" + '"' + self.field_name + '" ,'
        return ", ".join([start + self.efuse_block,
                         str(self.bit_start),
                         str(self.bit_count) + "}, \t // " + self.comment])

def process_input_file(file, type_table):
    status("Parsing efuse CSV input file " + file.name + " ...")
    input = file.read()
    table = FuseTable.from_csv(input, type_table)
    status("Verifying efuse table...")
    table.verify(type_table)
    return table

def ckeck_md5_in_file(md5, filename):
    if os.path.exists(filename):
        with open(filename,'r') as f:
            for line in f:
                if md5 in line:
                    return True
    return False

def touch(fname, times=None):
    with open(fname, 'a'):
        os.utime(fname, times)
    
def create_output_files(name, output_table, debug):
    file_name = os.path.splitext(os.path.basename(name))[0]
    gen_dir = os.path.dirname(name)
    
    dir_for_file_h = gen_dir + "/include"
    try:
        os.stat(dir_for_file_h)
    except:
        os.mkdir(dir_for_file_h)
    
    file_h_path = os.path.join(dir_for_file_h, file_name + ".h")
    file_c_path = os.path.join(gen_dir, file_name + ".c")
    
    # src files are the same
    if ckeck_md5_in_file(output_table.md5_digest_table, file_c_path) == False:
        status("Creating efuse *.h file " + file_h_path + " ...")
        output = output_table.to_header(file_name)
        with open(file_h_path, 'w') as f:
            f.write(output)
            f.close()
        
        status("Creating efuse *.c file " + file_c_path + " ...")
        output = output_table.to_c_file(file_name, debug)
        with open(file_c_path, 'w') as f:
            f.write(output)
            f.close()
    else:
        print("touch: %s.c" % (file_name))
        touch(file_c_path)
           
def main():
    global quiet
    global coding_scheme
    global custom_table_use_BLK1
    global custom_table_use_BLK2
    global common_table_fix_size
    
    parser = argparse.ArgumentParser(description='ESP32 eFuse Manager')
    
    parser.add_argument('--quiet', '-q', help="Don't print non-critical status messages to stderr", action='store_true')
    parser.add_argument('--debug', help='Create header file with debug info', default=False, action="store_false")
    parser.add_argument('--info', help='Print info about range of used bits', default=False, action="store_true")
    parser.add_argument('--custom_table_use_BLK1', help='BLK1 is used for custom purpose', default=False, action="store_true")
    parser.add_argument('--custom_table_use_BLK2', help='BLK2 is used for custom purpose', default=False, action="store_true")
    parser.add_argument('--common_table_fix_size_secure_key_and_encrypt_key', help='Size of secure_key and encrypt_key will limit to coding scheme', default=False, action="store_true")
    parser.add_argument('--coding_scheme', help='Coding scheme', type=int, default=None)
    parser.add_argument('common_input', help='Path to common CSV file to parse.', type=argparse.FileType('rb'))
    parser.add_argument('custom_input', help='Path to custom CSV file to parse.', type=argparse.FileType('rb'), nargs='?', default=None)

    args = parser.parse_args()

    coding_scheme = args.coding_scheme
    if CODE_SCHEME["NONE"] == coding_scheme:
        print("eFuse coding scheme: NONE")
    elif CODE_SCHEME["3/4"] == coding_scheme:
        print("eFuse coding scheme: 3/4")
    elif CODE_SCHEME["REPEAT"] == coding_scheme:
        print("eFuse coding scheme: REPEAT")
        
    custom_table_use_BLK1 = args.custom_table_use_BLK1
    custom_table_use_BLK2 = args.custom_table_use_BLK2
    common_table_fix_size = args.common_table_fix_size_secure_key_and_encrypt_key
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
    if info == False:
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
        super(ValidationError, self).__init__(
            "Entry %s invalid: %s" % (p.field_name, message))

if __name__ == '__main__':
    try:
        main()
    except InputError as e:
        print(e, file=sys.stderr)
        sys.exit(2)
