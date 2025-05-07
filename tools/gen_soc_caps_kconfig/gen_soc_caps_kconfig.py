#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
Generate Kconfig.soc_caps.in with defines from soc_caps.h
"""

import argparse
import inspect
import io
import logging
import os
import sys
from difflib import unified_diff
from os import path
from pathlib import Path
from string import Template

import pyparsing
from pyparsing import CaselessLiteral
from pyparsing import Char
from pyparsing import Combine
from pyparsing import Group
from pyparsing import Keyword
from pyparsing import Literal
from pyparsing import OneOrMore
from pyparsing import Optional
from pyparsing import ParserElement  # noqa: F401 # pylint: disable=unused-import
from pyparsing import QuotedString
from pyparsing import Suppress
from pyparsing import Word
from pyparsing import alphas
from pyparsing import hexnums
from pyparsing import nums
from pyparsing import restOfLine

pyparsing.usePackrat = True

try:
    import typing  # noqa: F401 # pylint: disable=unused-import
except ImportError:
    pass


class KconfigWriter:
    PREAMBLE = inspect.cleandoc("""
        #####################################################
        # This file is auto-generated from SoC caps
        # using gen_soc_caps_kconfig.py, do not edit manually
        #####################################################
        """)

    KCONFIG_ENTRY_TEMPLATE = Template(
        inspect.cleandoc("""
            config $name
                $entry_type
                default $value
            """)
    )
    KCONFIG_CONDITION_SOURCE_TEMPLATE = Template(
        inspect.cleandoc("""
            if $condition
                source "$source_path"
            endif
            """)
    )

    def __init__(self, path, always_write):  # type: (str, bool) -> None
        self.kconfig_text = io.StringIO('')
        self.kconfig_text.write(self.PREAMBLE)

        # NAME: (type, val)
        self.entries = dict()  # type: typing.Dict[str, typing.Tuple[str, typing.Any]]  # key: config name, value: tuple of (config_type, config_value)
        self.kconfig_path = path
        self.always_write = always_write

    def add_entry(self, name, entry_type, value):  # type: (str, str, typing.Any) -> None
        if name in self.entries:
            logging.info('Duplicate entry: {}'.format(name))
            return

        # Format values for kconfig
        if entry_type == 'bool':
            value = 'y' if value else 'n'
        elif entry_type == 'string':
            value = '"' + value + '"'
        self.entries[name] = (entry_type, value)

    def add_source(self, source_path, condition):  # type: (str, str) -> None
        self.kconfig_text.write('\n\n')
        if condition:
            source = self.KCONFIG_CONDITION_SOURCE_TEMPLATE.substitute(source_path=source_path, condition=condition)
            self.kconfig_text.write(source)
        else:
            self.kconfig_text.write('source "' + source_path + '"')

    def update_file(self):  # type: () -> bool
        for entry_name in self.entries:
            self.kconfig_text.write('\n\n')
            config_option = self.KCONFIG_ENTRY_TEMPLATE.substitute(
                name=entry_name, entry_type=self.entries[entry_name][0], value=self.entries[entry_name][1]
            )
            self.kconfig_text.write(config_option)

        try:
            with open(self.kconfig_path, 'r', encoding='utf-8') as f:
                old_content = f.readlines()
        except FileNotFoundError:
            old_content = ['']

        self.kconfig_text.seek(0)
        new_content = self.kconfig_text.readlines()
        new_content[-1] += '\n'  # Add final newline to end of file

        file_needs_update = self.always_write

        # Check if file was updated and print diff for users
        diff = unified_diff(old_content, new_content, fromfile=str(self.kconfig_path), n=2)
        for line in diff:
            print(line, end='')
            file_needs_update = True

        if file_needs_update:
            print('\n' + 'Updating file: {}'.format(self.kconfig_path))
            with open(self.kconfig_path, 'w', encoding='utf-8') as f:
                f.writelines(new_content)

        return file_needs_update


def parse_include(inc_line):  # type: (str) -> typing.Any[typing.Type[ParserElement]]
    # Comment with recursive pattern
    recursive_deli = OneOrMore(Group(Literal(' ') | Literal(',') | Literal('.')))
    recursive = CaselessLiteral('// recursive')('recursive') + Optional(recursive_deli)

    # Comment with condition pattern
    condition_deli = OneOrMore(Group(Literal(' ') | Literal(':')))
    condition = CaselessLiteral('condition') + Optional(condition_deli) + restOfLine('condition')

    # Parse the include line
    # e.g. #include "../../beta3/include/soc/soc_caps.h"  // recursive, condition: IDF_TARGET_ESP32C5_VERSION_BETA3
    expr = Suppress('#include') + QuotedString('"')('inc_path') + recursive + Optional(condition)
    res = expr.parseString(inc_line)
    return res


def parse_define(define_line):  # type: (str) -> typing.Any[typing.Type[ParserElement]]
    # Group for parsing literal suffix of a numbers, e.g. 100UL
    literal_symbol = Group(CaselessLiteral('L') | CaselessLiteral('U'))
    literal_suffix = OneOrMore(literal_symbol)

    # Define name
    name = Word(alphas, alphas + nums + '_')

    # Define value, either a hex, int or a string
    hex_value = Combine(Literal('0x') + Word(hexnums) + Optional(literal_suffix).suppress())('hex_value')
    int_value = (
        Combine(Optional('-') + Word(nums))('int_value') + ~Char('.') + Optional(literal_suffix)('literal_suffix')
    )
    str_value = QuotedString('"')('str_value')

    # Remove optional parenthesis around values
    value = Optional('(').suppress() + (hex_value ^ int_value ^ str_value)('value') + Optional(')').suppress()

    # Define ignore pragma (if present, define will be ignored)
    ignore_pragma = Literal('//') + Keyword('[gen_soc_caps:ignore]')

    expr = (
        '#define' + Optional(name)('name') + Optional(value) + Optional(ignore_pragma).set_results_name('ignore_pragma')
    )
    res = expr.parseString(define_line)

    return res


def generate_defines(soc_caps_dir, filename, always_write):  # type: (Path, str, bool) -> typing.Union[KconfigWriter, None]
    soc_headers = list(soc_caps_dir.glob(filename))
    if soc_headers == []:
        return None

    # Sort header files to make the generated files deterministic
    soc_headers.sort(key=lambda file: file.name)

    defines = []
    for soc_header in soc_headers:
        defines.extend(get_defines(soc_header))

    writer = KconfigWriter(os.path.join(soc_caps_dir, 'Kconfig.soc_caps.in'), always_write)

    for line in defines:
        try:
            inc = parse_include(line)
            if 'inc_path' in inc and 'recursive' in inc and inc['inc_path'][-6:] == 'caps.h':
                source_path = path.join(
                    '$IDF_PATH', str(soc_caps_dir), path.dirname(inc['inc_path']), 'Kconfig.soc_caps.in'
                )
                condition = inc['condition'].strip(' ') if 'condition' in inc else ''
                writer.add_source(source_path, condition)
                sub_soc_cap_dir = soc_caps_dir.joinpath(str(inc['inc_path'])).parent
                generate_defines(sub_soc_cap_dir, filename, always_write)
                continue
        except pyparsing.ParseException:
            pass

        try:
            res = parse_define(line)
        except pyparsing.ParseException:
            logging.debug('Failed to parse: {}'.format(line))
            continue

        if res.ignore_pragma:
            # Ignore this define
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

    return writer


def get_defines(header_path):  # type: (Path) -> list[str]
    defines = []
    logging.info('Reading macros from {}...'.format(header_path))
    with open(header_path, 'r', encoding='utf-8') as f:
        output = f.read()

    for line in output.split('\n'):
        line = line.strip()
        if len(line):
            defines.append(line)

    return defines


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('-d', '--dir', help='SoC caps folder paths, support wildcards', nargs='+', default=[])
    parser.add_argument('-n', '--filename', nargs='?', default='*caps.h', help='SoC caps filename, support wildcards')
    parser.add_argument(
        '-v',
        '--verbose',
        action='count',
        help='Increase the logging level of the script. Can be specified multiple times.',
    )
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
    writers = []  # type: typing.List[typing.Optional[KconfigWriter]]
    for caps_dir in args.dir:
        soc_caps_dirs = Path().glob(caps_dir)
        new_writers = [generate_defines(d, args.filename, args.always_write) for d in soc_caps_dirs if path.isdir(d)]
        writers.extend(new_writers)

    configs = dict()  # type: typing.Dict[str, str] # key: config name, value: config_type
    configs_with_differing_types = dict()  # type: typing.Dict[str, typing.Set[str]] # key: config name, value: set of types
    for writer in writers:
        if writer is None:
            continue
        for config_name in writer.entries:
            if config_name in configs:
                type_in_writer = writer.entries[config_name][0]
                type_in_configs = configs[config_name]
                if type_in_writer != type_in_configs:
                    if config_name not in configs_with_differing_types:
                        configs_with_differing_types[config_name] = set()
                    configs_with_differing_types[config_name].add(type_in_writer)
                    configs_with_differing_types[config_name].add(type_in_configs)

            else:
                configs[config_name] = writer.entries[config_name][0]

    if configs_with_differing_types:
        print(
            'The following macro constants would be translated to config options with different types'
            ' for different targets (which may lead to unexpected behavior).'
        )

        for config_name in configs_with_differing_types:
            print(
                f'  {config_name} has types'
                f'{", ".join(config_type for config_type in configs_with_differing_types[config_name])}'
            )

        print('Please ensure all the macro constants will translate to the same config type for all targets.')

        sys.exit(1)

    files_updated = [writer.update_file() for writer in writers if writer is not None]
    print('Updated {} files'.format(sum(files_updated)))

    sys.exit(all(files_updated))
