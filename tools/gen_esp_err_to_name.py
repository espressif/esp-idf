#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import collections
import fnmatch
import functools
import os
import re
import textwrap
from io import open
from typing import Any, List, Optional, TextIO

# list files here which should not be parsed
ignore_files: list  = list()

# add directories here which should not be parsed, this is a tuple since it will be used with *.startswith()
ignore_dirs = (os.path.join('components', 'cmock', 'CMock', 'test'),
               os.path.join('components', 'spi_flash', 'sim'))

# macros from here have higher priorities in case of collisions
priority_headers = [os.path.join('components', 'esp_common', 'include', 'esp_err.h')]

# The following headers won't be included. This is useful if they are permanently included from esp_err_to_name.c.in.
dont_include = [os.path.join('soc', 'soc.h'),
                os.path.join('esp_err.h')]

# Don't search the following directories, e.g. test directories
exclude_search_dirs = ['test_apps', 'unit-test-app']

err_dict = collections.defaultdict(list)  # identified errors are stored here; mapped by the error code
rev_err_dict = dict()  # map of error string to error code
unproc_list = list()  # errors with unknown codes which depend on other errors


class ErrItem(object):
    """
    Contains information about the error:
    - name - error string
    - file - relative path inside the IDF project to the file which defines this error
    - include_as - (optional) overwrites the include determined from file
    - comment - (optional) comment for the error
    - rel_str - (optional) error string which is a base for the error
    - rel_off - (optional) offset in relation to the base error
    """
    def __init__(self, name: str, file: str, include_as: Optional[Any]=None, comment: str='', rel_str: str='', rel_off: int=0) -> None:
        self.name = name
        self.file = file
        self.include_as = include_as
        self.comment = comment
        self.rel_str = rel_str
        self.rel_off = rel_off

    def __str__(self) -> str:
        ret = self.name + ' from ' + self.file
        if (self.rel_str != ''):
            ret += ' is (' + self.rel_str + ' + ' + str(self.rel_off) + ')'
        if self.comment != '':
            ret += ' // ' + self.comment
        return ret

    def __cmp__(self, other) -> int:
        if self.file in priority_headers and other.file not in priority_headers:
            return -1
        elif self.file not in priority_headers and other.file in priority_headers:
            return 1

        base = '_BASE'

        if self.file == other.file:
            if self.name.endswith(base) and not other.name.endswith(base):
                return 1
            elif not self.name.endswith(base) and other.name.endswith(base):
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
    def __init__(self, p: str, e: str) -> None:
        super(InputError, self).__init__(p + ': ' + e)


def process(line: str, idf_path: str, include_as: Any) -> None:
    """
    Process a line of text from file idf_path (relative to IDF project).
    Fills the global list unproc_list and dictionaries err_dict, rev_err_dict
    """
    if idf_path.endswith('.c'):
        # We would not try to include a C file
        raise InputError(idf_path, 'This line should be in a header file: %s' % line)

    words = re.split(r' +', line, 2)
    # words[1] is the error name
    # words[2] is the rest of the line (value, base + value, comment)
    if len(words) < 3:
        raise InputError(idf_path, 'Error at line %s' % line)

    line = ''
    todo_str = words[2]

    comment = ''
    # identify possible comment
    m = re.search(r'/\*!<(.+?(?=\*/))', todo_str)
    if m:
        comment = m.group(1).strip()
        todo_str = todo_str[:m.start()].strip()  # keep just the part before the comment

    # identify possible parentheses ()
    m = re.search(r'\((.+)\)', todo_str)
    if m:
        todo_str = m.group(1)  # keep what is inside the parentheses

    # identify BASE error code, e.g. from the form BASE + 0x01
    m = re.search(r'\s*(\w+)\s*\+(.+)', todo_str)
    if m:
        related = m.group(1)  # BASE
        todo_str = m.group(2)  # keep and process only what is after "BASE +"

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
            related = todo_str  # BASE error
            num = 0  # (BASE + 0)
        else:
            raise InputError(idf_path, 'Cannot parse line %s' % line)

    try:
        related
    except NameError:
        # The value of the error is known at this moment because it do not depends on some other BASE error code
        err_dict[num].append(ErrItem(words[1], idf_path, include_as, comment))
        rev_err_dict[words[1]] = num
    else:
        # Store the information available now and compute the error code later
        unproc_list.append(ErrItem(words[1], idf_path, include_as, comment, related, num))


def process_remaining_errors() -> None:
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
            num = base_num + item.rel_off
            err_dict[num].append(ErrItem(item.name, item.file, item.include_as, item.comment))
            rev_err_dict[item.name] = num
        else:
            print(item.rel_str + ' referenced by ' + item.name + ' in ' + item.file + ' is unknown')

    del unproc_list[:]


def path_to_include(path: str) -> str:
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
    spl_path = path.split(os.sep)
    try:
        i = spl_path.index('include')
    except ValueError:
        # no include in the path -> use just the filename
        return os.path.basename(path)
    else:
        return os.sep.join(spl_path[i + 1:])  # subdirectories and filename in "include"


def print_warning(error_list: List, error_code: int) -> None:
    """
    Print warning about errors with the same error code
    """
    print('[WARNING] The following errors have the same code (%d):' % error_code)
    for e in error_list:
        print('    ' + str(e))


def max_string_width() -> int:
    max = 0
    for k in err_dict:
        for e in err_dict[k]:
            x = len(e.name)
            if x > max:
                max = x
    return max


def generate_c_output(fin: TextIO, fout: TextIO) -> None:
    """
    Writes the output to fout based on th error dictionary err_dict and
    template file fin.
    """
    # make includes unique by using a set
    includes = set()
    for k in err_dict:
        for e in err_dict[k]:
            if e.include_as:
                includes.add(e.include_as)
            else:
                includes.add(path_to_include(e.file))

    # The order in a set in non-deterministic therefore it could happen that the
    # include order will be different in other machines and false difference
    # in the output file could be reported. In order to avoid this, the items
    # are sorted in a list.
    include_list = list(includes)
    include_list.sort()

    max_width = max_string_width() + 17 + 1  # length of "    ERR_TBL_IT()," with spaces is 17
    max_decdig = max(len(str(k)) for k in err_dict)

    for line in fin:
        if re.match(r'@COMMENT@', line):
            fout.write('//Do not edit this file because it is autogenerated by ' + os.path.basename(__file__) + '\n')

        elif re.match(r'@HEADERS@', line):
            for i in include_list:
                if i not in dont_include:
                    fout.write("#if __has_include(\"" + i + "\")\n#include \"" + i + "\"\n#endif\n")
        elif re.match(r'@ERROR_ITEMS@', line):
            last_file = ''
            for k in sorted(err_dict.keys()):
                if len(err_dict[k]) > 1:
                    err_dict[k].sort(key=functools.cmp_to_key(ErrItem.__cmp__))
                    print_warning(err_dict[k], k)
                for e in err_dict[k]:
                    if e.file != last_file:
                        last_file = e.file
                        fout.write('    // %s\n' % last_file)
                    table_line = ('    ERR_TBL_IT(' + e.name + '), ').ljust(max_width) + '/* ' + str(k).rjust(max_decdig)
                    fout.write('#   ifdef      %s\n' % e.name)
                    fout.write(table_line)
                    hexnum_length = 0
                    if k > 0:  # negative number and zero should be only ESP_FAIL and ESP_OK
                        hexnum = ' 0x%x' % k
                        hexnum_length = len(hexnum)
                        fout.write(hexnum)
                    if e.comment != '':
                        if len(e.comment) < 50:
                            fout.write(' %s' % e.comment)
                        else:
                            indent = ' ' * (len(table_line) + hexnum_length + 1)
                            w = textwrap.wrap(e.comment, width=120, initial_indent=indent, subsequent_indent=indent)
                            # this couldn't be done with initial_indent because there is no initial_width option
                            fout.write(' %s' % w[0].strip())
                            for i in range(1, len(w)):
                                fout.write('\n%s' % w[i])
                    fout.write(' */\n#   endif\n')
        else:
            fout.write(line)


def generate_rst_output(fout: TextIO) -> None:
    for k in sorted(err_dict.keys()):
        v = err_dict[k][0]
        fout.write(':c:macro:`{}` '.format(v.name))
        if k > 0:
            fout.write('**(0x{:x})**'.format(k))
        else:
            fout.write('({:d})'.format(k))
        if len(v.comment) > 0:
            fout.write(': {}'.format(v.comment))
        fout.write('\n\n')


def main() -> None:
    if 'IDF_PATH' in os.environ:
        idf_path = os.environ['IDF_PATH']
    else:
        idf_path = os.path.realpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))

    parser = argparse.ArgumentParser(description='ESP32 esp_err_to_name lookup generator for esp_err_t')
    parser.add_argument('--c_input', help='Path to the esp_err_to_name.c.in template input.',
                        default=idf_path + '/components/esp_common/src/esp_err_to_name.c.in')
    parser.add_argument('--c_output', help='Path to the esp_err_to_name.c output.', default=idf_path + '/components/esp_common/src/esp_err_to_name.c')
    parser.add_argument('--rst_output', help='Generate .rst output and save it into this file')
    args = parser.parse_args()

    include_as_pattern = re.compile(r'\s*//\s*{}: [^"]* "([^"]+)"'.format(os.path.basename(__file__)))
    define_pattern = re.compile(r'\s*#define\s+(ESP_ERR_|ESP_OK|ESP_FAIL)')

    for root, dirnames, filenames in os.walk(os.path.join(idf_path, 'components'), topdown=True):
        # When topdown is True, we can modify the dirnames list in-place
        # walk() will only recurse into the subdirectories whose names remain in dirnames
        dirnames[:] = [d for d in dirnames if d not in exclude_search_dirs]
        for filename in fnmatch.filter(filenames, '*.[ch]'):
            full_path = os.path.join(root, filename)
            path_in_idf = os.path.relpath(full_path, idf_path)
            if path_in_idf in ignore_files or path_in_idf.startswith(ignore_dirs):
                continue
            with open(full_path, encoding='utf-8') as f:
                try:
                    include_as = None
                    for line in f:
                        line = line.strip()
                        m = include_as_pattern.search(line)
                        if m:
                            include_as = m.group(1)
                        # match also ESP_OK and ESP_FAIL because some of ESP_ERRs are referencing them
                        elif define_pattern.match(line):
                            try:
                                process(line, path_in_idf, include_as)
                            except InputError as e:
                                print(e)
                except UnicodeDecodeError:
                    raise ValueError('The encoding of {} is not Unicode.'.format(path_in_idf))

    process_remaining_errors()

    if args.rst_output is not None:
        with open(args.rst_output, 'w', encoding='utf-8') as fout:
            generate_rst_output(fout)
    else:
        with open(args.c_input, 'r', encoding='utf-8') as fin, open(args.c_output, 'w', encoding='utf-8') as fout:
            generate_c_output(fin, fout)


if __name__ == '__main__':
    main()
