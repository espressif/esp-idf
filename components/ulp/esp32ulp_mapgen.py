#!/usr/bin/env python
# esp32ulp_mapgen utility converts a symbol list provided by nm into an export script
# for the linker and a header file.
#
# SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import print_function

import argparse
import os
import textwrap
import typing

UTIL = os.path.basename(__file__)


def gen_ld_h_from_sym(f_sym: typing.TextIO, f_ld: typing.TextIO, f_h: typing.TextIO, base_addr: int) -> None:
    f_ld.write(textwrap.dedent(
        f"""
        /* ULP variable definitions for the linker.
         * This file is generated automatically by {UTIL} utility.
         */
        """
    ))
    f_h.write(textwrap.dedent(
        f"""
        /* ULP variable definitions for the compiler.
         * This file is generated automatically by {UTIL} utility.
         */
        #pragma once
        #ifdef __cplusplus
        extern "C" {{
        #endif
        """
    ))

    for line in f_sym:
        # NM "posix" format output has the following structure:
        # symbol_name symbol_type addr_hex [size_hex]
        parts = line.split()
        name = parts[0]
        addr = int(parts[2], 16) + base_addr
        f_h.write('extern uint32_t ulp_{0};\n'.format(name))
        f_ld.write('PROVIDE ( ulp_{0} = 0x{1:08x} );\n'.format(name, addr))

    f_h.write(textwrap.dedent(
        """
        #ifdef __cplusplus
        }
        #endif
        """
    ))


def main() -> None:
    description = ('This application generates .h and .ld files for symbols defined in input file. '
                   'The input symbols file can be generated using nm utility like this: '
                   '<PREFIX>nm -g -f posix <elf_file> > <symbols_file>')

    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('-s', '--symfile', required=True, help='symbols file name', metavar='SYMFILE', type=argparse.FileType('r'))
    parser.add_argument('-o', '--outputfile', required=True, help='destination .h and .ld files name prefix', metavar='OUTFILE')
    parser.add_argument('--base-addr', required=True, help='base address of the ULP memory, to be added to each symbol')

    args = parser.parse_args()

    with open(args.outputfile + '.h', 'w') as f_h, open(args.outputfile + '.ld', 'w') as f_ld:
        gen_ld_h_from_sym(args.symfile, f_ld, f_h, int(args.base_addr, 0))


if __name__ == '__main__':
    main()
