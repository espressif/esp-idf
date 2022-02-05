#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""
Generate Kconfig.soc_caps.in with defines from soc_caps.h
"""

import argparse
import inspect
import io
import logging
import sys
from difflib import unified_diff
from os import path
from pathlib import Path
from string import Template

import pyparsing
from pyparsing import (CaselessLiteral, Char, Combine, Group, Literal, OneOrMore,  # pylint: disable=unused-import
                       Optional, ParserElement, QuotedString, Word, alphas, hexnums, nums)

pyparsing.usePackrat = True

try:
    import typing  # noqa: F401 # pylint: disable=unused-import
except ImportError:
    pass


class KconfigWriter():
    PREAMBLE = inspect.cleandoc('''
        #####################################################
        # This file is auto-generated from SoC caps
        # using gen_soc_caps_kconfig.py, do not edit manually
        #####################################################
        ''')

    KCONFIG_ENTRY_TEMPLATE = Template(
        inspect.cleandoc('''
            config $name
                $entry_type
                default $value
            '''))

    def __init__(self):  # type: () -> None
        self.entries = set('')

        self.kconfig_text = io.StringIO('')
        self.kconfig_text.write(self.PREAMBLE)

    def add_entry(self, name, entry_type, value):  # type: (str, str, typing.Any) -> None

        if name in self.entries:
            logging.info('Duplicate entry: {}'.format(name))
            return

        self.entries.add(name)
        self.kconfig_text.write('\n\n')

        # Format values for kconfig
        if entry_type == 'bool':
            value = 'y' if value else 'n'
        elif entry_type == 'string':
            value = '"' + value + '"'

        entry = self.KCONFIG_ENTRY_TEMPLATE.substitute(name=name, entry_type=entry_type, value=value)
        self.kconfig_text.write(entry)

    def update_file(self, kconfig_path, always_write):  # type: (Path, bool) -> bool

        try:
            with open(kconfig_path, 'r') as f:
                old_content = f.readlines()
        except FileNotFoundError:
            old_content = ['']

        self.kconfig_text.seek(0)
        new_content = self.kconfig_text.readlines()
        new_content[-1] += '\n'  # Add final newline to end of file

        file_needs_update = always_write

        # Check if file was updated and print diff for users
        diff = unified_diff(old_content, new_content, fromfile=str(kconfig_path), n=2)
        for line in diff:
            print(line, end='')
            file_needs_update = True

        if file_needs_update:
            print('\n' + 'Updating file: {}'.format(kconfig_path))
            with open(kconfig_path, 'w') as f:
                f.writelines(new_content)

        return file_needs_update


def parse_define(define_line):  # type: (str) -> typing.Any[typing.Type[ParserElement]]

    # Group for parsing literal suffix of a numbers, e.g. 100UL
    literal_symbol = Group(CaselessLiteral('L') | CaselessLiteral('U'))
    literal_suffix = OneOrMore(literal_symbol)

    # Define name
    name = Word(alphas, alphas + nums + '_')

    # Define value, either a hex, int or a string
    hex_value = Combine(Literal('0x') + Word(hexnums) + Optional(literal_suffix).suppress())('hex_value')
    int_value = Word(nums)('int_value') + ~Char('.') + Optional(literal_suffix)('literal_suffix')
    str_value = QuotedString('"')('str_value')

    # Remove optional parenthesis around values
    value = Optional('(').suppress() + (hex_value ^ int_value ^ str_value)('value')  + Optional(')').suppress()

    expr = '#define' + Optional(name)('name') + Optional(value)
    res = expr.parseString(define_line)

    return res


def generate_defines(soc_caps_dir, filename, always_write):  # type: (Path, str, bool) -> bool

    soc_headers = list(soc_caps_dir.glob(filename))
    if soc_headers == []:
        return False

    # Sort header files to make the generated files deterministic
    soc_headers.sort(key=lambda file: file.name)

    defines = []
    for soc_header in soc_headers:
        defines.extend(get_defines(soc_header))

    writer = KconfigWriter()

    for line in defines:

        try:
            res = parse_define(line)
        except pyparsing.ParseException:
            logging.debug('Failed to parse: {}'.format(line))
            continue

        # Add the kconfig entry corresponding to the type we parsed
        if 'str_value' in res:
            writer.add_entry(res.name, 'string', res.str_value)
        elif 'int_value' in res:
            # defines with an integer value of 0 or 1 are
            # added as bool entries as long they have no literal suffix
            if 'literal_suffix' not in res and res.int_value == '0':
                writer.add_entry(res.name, 'bool', False)
            elif 'literal_suffix' not in res and res.int_value == '1':
                writer.add_entry(res.name, 'bool', True)
            else:
                writer.add_entry(res.name, 'int', res.int_value)
        elif 'hex_value' in res:
            writer.add_entry(res.name, 'hex', res.hex_value)

    # Updates output if necessary
    updated = writer.update_file(Path(soc_caps_dir) / 'Kconfig.soc_caps.in', always_write)

    return updated


def get_defines(header_path):  # type: (Path) -> list[str]
    defines = []
    logging.info('Reading macros from {}...'.format(header_path))
    with open(header_path, 'r') as f:
        output = f.read()

    for line in output.split('\n'):
        line = line.strip()
        if len(line):
            defines.append(line)

    return defines


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('-d', '--dir', help='SoC caps folder paths, support wildcards', nargs='+', default=[])
    parser.add_argument('-n', '--filename', nargs='?', default='*caps.h',
                        help='SoC caps filename, support wildcards')
    parser.add_argument('-v', '--verbose', action='count', help='Increase the logging level of the script. Can be specified multiple times.')
    parser.add_argument('--always-write', help='Always generate new output files', action='store_true')
    args = parser.parse_args()

    if not args.verbose:
        log_level = logging.WARNING
    elif args.verbose == 1:
        log_level = logging.INFO
    else:
        log_level = logging.DEBUG

    logging.basicConfig(level=log_level)

    files_updated = []
    for caps_dir in args.dir:
        soc_caps_dirs = Path().glob(caps_dir)
        files_updated += [generate_defines(d, args.filename, args.always_write) for d in soc_caps_dirs if path.isdir(d)]

    print('Updated {} files'.format(sum(files_updated)))

    sys.exit(all(files_updated))
