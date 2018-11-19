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
        self.md5_digest = ""

    @classmethod
    def from_csv(cls, csv_contents):
        res = FuseTable()
        res.md5_digest = res.calc_md5(csv_contents)
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
        
    def verify(self):        

        '''list_field_names = []'''
        for p in self:
            p.verify()

        self.verify_duplicate_name()
        
        # check for overlaps
        last = None
        for p in sorted(self, key=lambda x:(x.efuse_block, x.bit_start)):
            if last is not None and last.efuse_block == p.efuse_block and p.bit_start < last.bit_start + last.bit_count:
                raise InputError("Field at %s, %s, %s, %s overlaps %s, %s, %s, %s" 
                    % (   p.field_name,    p.efuse_block,    p.bit_start,    p.bit_count, 
                       last.field_name, last.efuse_block, last.bit_start, last.bit_count))
            last = p
    
    def calc_md5(self, csv_contents):
        return hashlib.md5(csv_contents).hexdigest()
        
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
                 "// md5_digest " + self.md5_digest,
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
                 "// md5_digest " + self.md5_digest,
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

    def verify(self):
        '''if self.field_name is None:
            raise ValidationError(self, "field_name field is not set")'''
        if self.efuse_block is None:
            raise ValidationError(self, "efuse_block field is not set")
        if self.bit_count is None:
            raise ValidationError(self, "bit_count field is not set")
        
        max_bits = 256
        if self.efuse_block == "EFUSE_BLK0":
            max_bits = 224
        if self.bit_start + self.bit_count > max_bits:
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

def process_input_file(file):
    status("Parsing efuse CSV input file " + file.name + " ...")
    input = file.read()
    table = FuseTable.from_csv(input)

    status("Verifying efuse table...")
    table.verify()
    return table

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
       
def main():
    global quiet
    
    parser = argparse.ArgumentParser(description='ESP32 eFuse Manager')
    
    parser.add_argument('--quiet', '-q', help="Don't print non-critical status messages to stderr", action='store_true')
    parser.add_argument('--debug', help='Create header file with debug info', default=False, action="store_false")
    parser.add_argument('--info', help='Print info about range of used bits', default=False, action="store_true")
    parser.add_argument('common_input', help='Path to common CSV file to parse.', type=argparse.FileType('rb'))
    parser.add_argument('custom_input', help='Path to custom CSV file to parse.', type=argparse.FileType('rb'), nargs='?', default=None)

    args = parser.parse_args()

    quiet = args.quiet
    debug = args.debug
    info = args.info
    
    common_table = process_input_file(args.common_input)
    if args.custom_input is not None:
        custom_table = process_input_file(args.custom_input)
        common_table += custom_table
        common_table.verify()
        if info == True:
            output_table = common_table
        else:
            output_table = custom_table
        name = args.custom_input.name
    else:
        output_table = common_table
        name = args.common_input.name
    output_table.verify()
    
    # save files.
    if info == False:
        create_output_files(name, output_table, debug)
    else:
        print(output_table.show_range_used_bits())
    
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
