#!/usr/bin/env python
# SPDX-FileCopyrightText: 2016-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# esp32ulp_mapgen utility converts a symbol list provided by nm into an export script
# for the linker and a header file.
import argparse
import os
import re
import textwrap
import typing

UTIL = os.path.basename(__file__)


def name_mangling(name: str) -> str:
    # Simple and dumb name mangling for namespaced name following GCC algorithm
    ns, n = name.split('::')
    return '_ZN{0}{1}{2}{3}E'.format(len(ns), ns, len(n), n)


def gen_ld_h_from_sym(f_sym: typing.TextIO, f_ld: typing.TextIO, f_h: typing.TextIO, base_addr: int, prefix: str) -> None:
    f_ld.write(textwrap.dedent(
        f"""
        /* ULP variable definitions for the linker.
         * This file is generated automatically by {UTIL} utility.
         */
        """  # noqa: E222
    ))
    cpp_mode = False
    var_prefix = prefix
    namespace = ''
    if '::' in prefix:
        # C++ mode, let's avoid the extern "C" type and instead use namespace
        f_h.write(textwrap.dedent(
            f"""
            /* ULP variable definitions for the compiler.
             * This file is generated automatically by {UTIL} utility.
             */
            #pragma once
            """  # noqa: E222
        ))
        tmp = prefix.split('::')
        namespace = tmp[0]
        var_prefix = '_'.join(tmp[1:])  # Limit to a single namespace here to avoid complex mangling rules
        f_h.write(f'namespace {namespace} {{\n')
        cpp_mode = True
    else:
        f_h.write(textwrap.dedent(
            f"""
            /* ULP variable definitions for the compiler.
             * This file is generated automatically by {UTIL} utility.
             */
            #pragma once
            #ifdef __cplusplus
            extern "C" {{
            #endif\n
            """  # noqa: E222
        ))

    # Format the regular expression to match the readelf output
    expr = re.compile(r'^.*(?P<address>[a-f0-9]{8})\s+(?P<size>\d+) (OBJECT|NOTYPE)\s+GLOBAL\s+DEFAULT\s+[^ ]+ (?P<name>.*)$')
    for line in f_sym:
        # readelf format output has the following structure:
        #   Num:    Value  Size Type    Bind   Vis      Ndx Name
        # So match the line with a regular expression to parse it first
        groups = expr.match(line)

        # Ignore lines that do not match the expected format such as non global symbols
        if groups is None:
            continue

        # Extract the symbol information
        addr = int(groups.group('address'), 16) + base_addr
        size = int(groups.group('size'))
        sym_name = groups.group('name')

        # Add the variable prefix if provided
        var_name = var_prefix + sym_name

        # Get the dimension of the variable if it is an array
        var_dimension = f'[{size // 4}]' if size > 4 else ''  # Use // to automatically perform an integer division

        # Write the variable definition to the header file and the address to the linker script
        f_h.write(f'extern uint32_t {var_name}{var_dimension};\n')

        name_in_ld = name_mangling(namespace + '::' + var_name) if cpp_mode else var_name
        f_ld.write(f'{name_in_ld} = 0x{addr:08x};\n')

    if cpp_mode:
        f_h.write('}\n')
    else:
        f_h.write(textwrap.dedent(
            """
            #ifdef __cplusplus
            }
            #endif
            """
        ))


def main() -> None:
    description = ('This application generates .h and .ld files for symbols defined in input file. '
                   'The input symbols file can be generated using readelf utility like this: '
                   '<PREFIX>readelf -sW <elf_file> > <symbols_file>')

    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('-s', '--symfile', required=True, help='symbols file name', metavar='SYMFILE', type=argparse.FileType('r'))
    parser.add_argument('-o', '--outputfile', required=True, help='destination .h and .ld files name prefix', metavar='OUTFILE')
    parser.add_argument('--base-addr', required=True, help='base address of the ULP memory, to be added to each symbol')
    parser.add_argument('-p', '--prefix', required=False, help='prefix for generated header file', default='ulp_')

    args = parser.parse_args()

    with open(args.outputfile + '.h', 'w') as f_h, open(args.outputfile + '.ld', 'w') as f_ld:
        gen_ld_h_from_sym(args.symfile, f_ld, f_h, int(args.base_addr, 0), args.prefix)


if __name__ == '__main__':
    main()
