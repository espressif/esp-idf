#!/usr/bin/env python
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import argparse
import mmap
import re
import fnmatch
import string
import collections
import textwrap

# list files here which should not be parsed
ignore_files = [ 'components/mdns/test_afl_fuzz_host/esp32_compat.h' ]

# macros from here have higher priorities in case of collisions
priority_headers = [ 'components/esp32/include/esp_err.h' ]

err_dict = collections.defaultdict(list) #identified errors are stored here; mapped by the error code
rev_err_dict = dict() #map of error string to error code
unproc_list = list() #errors with unknown codes which depend on other errors

class ErrItem:
    """
    Contains information about the error:
    - name - error string
    - file - relative path inside the IDF project to the file which defines this error
    - comment - (optional) comment for the error
    - rel_str - (optional) error string which is a base for the error
    - rel_off - (optional) offset in relation to the base error
    """
    def __init__(self, name, file, comment, rel_str = "", rel_off = 0):
        self.name = name
        self.file = file
        self.comment = comment
        self.rel_str = rel_str
        self.rel_off = rel_off
    def __str__(self):
        ret = self.name + " from " + self.file
        if (self.rel_str != ""):
            ret += " is (" + self.rel_str + " + " + str(self.rel_off) + ")"
        if self.comment != "":
            ret += " // " + self.comment
        return ret
    def __cmp__(self, other):
        if self.file in priority_headers and other.file not in priority_headers:
            return -1
        elif self.file not in priority_headers and other.file in priority_headers:
            return 1

        base = "_BASE"

        if self.file == other.file:
            if self.name.endswith(base) and not(other.name.endswith(base)):
                return 1
            elif not(self.name.endswith(base)) and other.name.endswith(base):
                return -1

        self_key = self.file + self.name
        other_key = other.file + other.name
        if self_key < other_key:
            return -1
        elif self_key > other_key:
            return 1
        else:
            return 0

class InputError(RuntimeError):
    """
    Represents and error on the input
    """
    def __init__(self, p, e):
        super(InputError, self).__init__(p + ": " + e)

def process(line, idf_path):
    """
    Process a line of text from file idf_path (relative to IDF project).
    Fills the global list unproc_list and dictionaries err_dict, rev_err_dict
    """
    if idf_path.endswith(".c"):
        # We would not try to include a C file
        raise InputError(idf_path, "This line should be in a header file: %s" % line)

    words = re.split(r' +', line, 2)
    # words[1] is the error name
    # words[2] is the rest of the line (value, base + value, comment)
    if len(words) < 2:
        raise InputError(idf_path, "Error at line %s" % line)

    line = ""
    todo_str = words[2]

    comment = ""
    # identify possible comment
    m = re.search(r'/\*!<(.+?(?=\*/))', todo_str)
    if m:
        comment = string.strip(m.group(1))
        todo_str = string.strip(todo_str[:m.start()]) # keep just the part before the comment

    # identify possible parentheses ()
    m = re.search(r'\((.+)\)', todo_str)
    if m:
        todo_str = m.group(1) #keep what is inside the parentheses

    # identify BASE error code, e.g. from the form BASE + 0x01
    m = re.search(r'\s*(\w+)\s*\+(.+)', todo_str)
    if m:
        related = m.group(1) # BASE
        todo_str = m.group(2) # keep and process only what is after "BASE +"

    # try to match a hexadecimal number
    m = re.search(r'0x([0-9A-Fa-f]+)', todo_str)
    if m:
        num = int(m.group(1), 16)
    else:
        # Try to match a decimal number. Negative value is possible for some numbers, e.g. ESP_FAIL
        m = re.search(r'(-?[0-9]+)', todo_str)
        if m:
            num = int(m.group(1), 10)
        elif re.match(r'\w+', todo_str):
            # It is possible that there is no number, e.g. #define ERROR BASE
            related = todo_str # BASE error
            num = 0 # (BASE + 0)
        else:
            raise InputError(idf_path, "Cannot parse line %s" % line)

    try:
        related
    except NameError:
        # The value of the error is known at this moment because it do not depends on some other BASE error code
        err_dict[num].append(ErrItem(words[1], idf_path, comment))
        rev_err_dict[words[1]] = num
    else:
        # Store the information available now and compute the error code later
        unproc_list.append(ErrItem(words[1], idf_path, comment, related, num))

def process_remaining_errors():
    """
    Create errors which could not be processed before because the error code
    for the BASE error code wasn't known.
    This works for sure only if there is no multiple-time dependency, e.g.:
        #define BASE1   0
        #define BASE2   (BASE1 + 10)
        #define ERROR   (BASE2 + 10) - ERROR will be processed successfully only if it processed later than BASE2
    """
    for item in unproc_list:
        if item.rel_str in rev_err_dict:
            base_num = rev_err_dict[item.rel_str]
            base = err_dict[base_num][0]
            num = base_num + item.rel_off
            err_dict[num].append(ErrItem(item.name, item.file, item.comment))
            rev_err_dict[item.name] = num
        else:
            print(item.rel_str + " referenced by " + item.name + " in " + item.file + " is unknown")

    del unproc_list[:]

def path_to_include(path):
    """
    Process the path (relative to the IDF project) in a form which can be used
    to include in a C file. Using just the filename does not work all the
    time because some files are deeper in the tree. This approach tries to
    find an 'include' parent directory an include its subdirectories, e.g.
    "components/XY/include/esp32/file.h" will be transported into "esp32/file.h"
    So this solution works only works when the subdirectory or subdirectories
    are inside the "include" directory. Other special cases need to be handled
    here when the compiler gives an unknown header file error message.
    """
    spl_path = string.split(path, os.sep)
    try:
        i = spl_path.index('include')
    except ValueError:
        # no include in the path -> use just the filename
        return os.path.basename(path)
    else:
        return str(os.sep).join(spl_path[i+1:]) # subdirectories and filename in "include"

def print_warning(error_list, error_code):
    """
    Print warning about errors with the same error code
    """
    print("[WARNING] The following errors have the same code (%d):" % error_code)
    for e in error_list:
        print("    " + str(e))

def max_string_width():
    max = 0
    for k in err_dict.keys():
        for e in err_dict[k]:
            x = len(e.name)
            if x > max:
                max = x
    return max

def generate_output(fin, fout):
    """
    Writes the output to fout based on th error dictionary err_dict and
    template file fin.
    """
    # make includes unique by using a set
    includes = set()
    for k in err_dict.keys():
        for e in err_dict[k]:
            includes.add(path_to_include(e.file))

    # The order in a set in non-deterministic therefore it could happen that the
    # include order will be different in other machines and false difference
    # in the output file could be reported. In order to avoid this, the items
    # are sorted in a list.
    include_list = list(includes)
    include_list.sort()

    max_width = max_string_width() + 17 + 1 # length of "    ERR_TBL_IT()," with spaces is 17
    max_decdig = max(len(str(k)) for k in err_dict.keys())

    for line in fin:
        if re.match(r'@COMMENT@', line):
            fout.write("//Do not edit this file because it is autogenerated by " + os.path.basename(__file__) + "\n")

        elif re.match(r'@HEADERS@', line):
            for i in include_list:
                fout.write("#if __has_include(\"" + i + "\")\n#include \"" + i + "\"\n#endif\n")
        elif re.match(r'@ERROR_ITEMS@', line):
            last_file = ""
            for k in sorted(err_dict.keys()):
                if len(err_dict[k]) > 1:
                    err_dict[k].sort()
                    print_warning(err_dict[k], k)
                for e in err_dict[k]:
                    if e.file != last_file:
                        last_file = e.file
                        fout.write("    // %s\n" % last_file)
                    table_line = ("    ERR_TBL_IT(" + e.name + "), ").ljust(max_width) + "/* " + str(k).rjust(max_decdig)
                    fout.write("#   ifdef      %s\n" % e.name)
                    fout.write(table_line)
                    hexnum_length = 0
                    if k > 0: # negative number and zero should be only ESP_FAIL and ESP_OK
                        hexnum = " 0x%x" % k
                        hexnum_length = len(hexnum)
                        fout.write(hexnum)
                    if e.comment != "":
                        if len(e.comment) < 50:
                            fout.write(" %s" % e.comment)
                        else:
                            indent = " " * (len(table_line) + hexnum_length + 1)
                            w = textwrap.wrap(e.comment, width=120, initial_indent = indent, subsequent_indent = indent)
                            # this couldn't be done with initial_indent because there is no initial_width option
                            fout.write(" %s" % w[0].strip())
                            for i in range(1, len(w)):
                                fout.write("\n%s" % w[i])
                    fout.write(" */\n#   endif\n")
        else:
            fout.write(line)

def main():
    parser = argparse.ArgumentParser(description='ESP32 esp_err_to_name lookup generator for esp_err_t')
    parser.add_argument('input', help='Path to the esp_err_to_name.c.in template input.', default=os.environ['IDF_PATH'] + '/components/esp32/esp_err_to_name.c.in', nargs='?')
    parser.add_argument('output', help='Path to the esp_err_to_name.c output.', default=os.environ['IDF_PATH'] + '/components/esp32/esp_err_to_name.c', nargs='?')
    args = parser.parse_args()

    for root, dirnames, filenames in os.walk(os.environ['IDF_PATH']):
        for filename in fnmatch.filter(filenames, '*.[ch]'):
            full_path = os.path.join(root, filename)
            idf_path = os.path.relpath(full_path, os.environ['IDF_PATH'])
            if idf_path in ignore_files:
                continue
            with open(full_path, "r+b") as f:
                try:
                    map = mmap.mmap(f.fileno(), 0, prot=mmap.ACCESS_READ)
                except ValueError:
                    pass # An empty file cannot be mmaped
                else:
                    for line in iter(map.readline, ""):
                        # match also ESP_OK and ESP_FAIL because some of ESP_ERRs are referencing them
                        if re.match(r"\s*#define\s+(ESP_ERR_|ESP_OK|ESP_FAIL)", line):
                            try:
                                process(str.strip(line), idf_path)
                            except InputError as e:
                                print (e)

    process_remaining_errors()

    with open(args.input, 'r') as fin, open(args.output, 'w') as fout:
        generate_output(fin, fout)

if __name__ == "__main__":
    main()
