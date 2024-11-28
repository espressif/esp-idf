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
        f_h.write('namespace {0} {{\n'.format(namespace))
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
            #endif
            """  # noqa: E222
        ))

    expr = re.compile('^\\s*\\d+: ([a-f0-9]{8})\\s+(\\d+) OBJECT\\s+GLOBAL\\s+DEFAULT\\s+[^ ]+ (.*)$')
    already_defined = 'this_symbol_is_already_defined_please_use_prefix_in_ulp_embed_binary'
    for line in f_sym:
        # readelf format output has the following structure:
        #   index: addr_hex     size TYPE  SCOPE DEFAULT    junk symbol_name
        # So match the line with a regular expression to parse it first
        groups = expr.match(line)
        if groups is None:  # Ignore non global or non object
            continue
        addr = int(groups.group(1), 16) + base_addr
        size = int(groups.group(2))
        name = var_prefix + groups.group(3)
        f_h.write('extern uint32_t {0}{1};\n'.format(name, '[{0}]'.format(int(size / 4)) if size > 4 else ''))
        f_ld.write('{0} = DEFINED({0}) ? {2} : 0x{1:08x};\n'.format(
            name_mangling(namespace + '::' + name) if cpp_mode else name, addr, already_defined))

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
