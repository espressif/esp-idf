#!/usr/bin/env python
# esp32ulp_mapgen utility converts a symbol list provided by nm into an export script
# for the linker and a header file.
#
# Copyright (c) 2016-2017 Espressif Systems (Shanghai) PTE LTD.
# Distributed under the terms of Apache License v2.0 found in the top-level LICENSE file.

from __future__ import print_function

import sys
from optparse import OptionParser

BASE_ADDR = 0x50000000


def gen_ld_h_from_sym(f_sym, f_ld, f_h):
    f_ld.write('/* Variable definitions for ESP32ULP linker\n')
    f_ld.write(' * This file is generated automatically by esp32ulp_mapgen.py utility.\n')
    f_ld.write(' */\n\n')
    f_h.write('// Variable definitions for ESP32ULP\n')
    f_h.write('// This file is generated automatically by esp32ulp_mapgen.py utility\n\n')
    f_h.write('#pragma once\n\n')

    for line in f_sym:
        name, _, addr_str = line.split(' ', 2)
        addr = int(addr_str, 16) + BASE_ADDR
        f_h.write('extern uint32_t ulp_{0};\n'.format(name))
        f_ld.write('PROVIDE ( ulp_{0} = 0x{1:08x} );\n'.format(name, addr))


def gen_ld_h_from_sym_riscv(f_sym, f_ld, f_h):
    f_ld.write('/* Variable definitions for ESP32ULP linker\n')
    f_ld.write(' * This file is generated automatically by esp32ulp_mapgen.py utility.\n')
    f_ld.write(' */\n\n')
    f_h.write('// Variable definitions for ESP32ULP\n')
    f_h.write('// This file is generated automatically by esp32ulp_mapgen.py utility\n\n')
    f_h.write('#pragma once\n\n')

    for line in f_sym:
        addr_str, _, name = line.split()
        addr = int(addr_str, 16) + BASE_ADDR
        f_h.write('extern uint32_t ulp_{0};\n'.format(name))
        f_ld.write('PROVIDE ( ulp_{0} = 0x{1:08x} );\n'.format(name, addr))


def main():
    if sys.version_info[0] < 3:
        print('WARNING: Support for Python 2 is deprecated and will be removed in future versions.', file=sys.stderr)
    elif sys.version_info[0] == 3 and sys.version_info[1] < 6:
        print('WARNING: Python 3 versions older than 3.6 are not supported.', file=sys.stderr)
    description = ('This application generates .h and .ld files for symbols defined in input file. '
                   'The input symbols file can be generated using nm utility like this: '
                   'esp32-ulp-nm -g -f posix <elf_file> > <symbols_file>')

    parser = OptionParser(description=description)
    parser.add_option('-s', '--symfile', dest='symfile',
                      help='symbols file name', metavar='SYMFILE')
    parser.add_option('-o', '--outputfile', dest='outputfile',
                      help='destination .h and .ld files name prefix', metavar='OUTFILE')

    parser.add_option('--riscv', action='store_true', help='use format for ulp riscv .sym file')

    (options, args) = parser.parse_args()
    if options.symfile is None:
        parser.print_help()
        return 1

    if options.outputfile is None:
        parser.print_help()
        return 1

    if options.riscv:
        with open(options.outputfile + '.h', 'w') as f_h, open(options.outputfile + '.ld', 'w') as f_ld, open(options.symfile) as f_sym:
            gen_ld_h_from_sym_riscv(f_sym, f_ld, f_h)
        return 0

    with open(options.outputfile + '.h', 'w') as f_h, open(options.outputfile + '.ld', 'w') as f_ld, open(options.symfile) as f_sym:
        gen_ld_h_from_sym(f_sym, f_ld, f_h)
    return 0


if __name__ == '__main__':
    exit(main())
