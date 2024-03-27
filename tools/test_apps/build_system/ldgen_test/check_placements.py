#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# Check placements in this test app for main
# specified in main/linker.lf
import argparse
import subprocess

from pyparsing import alphanums
from pyparsing import hexnums
from pyparsing import LineEnd
from pyparsing import LineStart
from pyparsing import Literal
from pyparsing import Optional
from pyparsing import Word

argparser = argparse.ArgumentParser()

argparser.add_argument('objdump')
argparser.add_argument('elf')
argparser.add_argument('--no-rtc', action='store_true')
argparser.add_argument('--non-contiguous-sram', action='store_true')

args = argparser.parse_args()

contents = subprocess.check_output([args.objdump, '-t', args.elf]).decode()


def check_location(symbol, expected):
    pattern = (LineStart() + Word(hexnums).setResultsName('address')
               + Optional(Word(alphanums, exact=1))
               + Optional(Word(alphanums,exact=1))
               + Word(alphanums + '._*').setResultsName('actual')
               + Word(hexnums)
               + Literal(symbol)
               + LineEnd())

    try:
        results = pattern.searchString(contents)[0]
    except IndexError:
        raise Exception("check placement fail: '%s' was not found" % (symbol))

    if results.actual != expected:
        raise Exception("check placement fail: '%s' was placed in '%s', not in '%s'" % (symbol, results.actual, expected))

    print("check placement pass: '%s' was successfully placed in '%s'" % (symbol, results.actual))
    return int(results.address, 16)


# src1:func1 (noflash) - explicit mapping for func2 using 'rtc' scheme
# should have been dropped since it is unreferenced.
func1 = check_location('func1', '.iram0.text')

sym1_start = check_location('_sym1_start', '*ABS*')
sym1_end = check_location('_sym1_end', '*ABS*')

assert func1 >= sym1_start, 'check placement fail: func1 comes before __sym1_start'
assert func1 < sym1_end, 'check placement fail: func1 comes after __sym1_end'
assert sym1_start % 9 == 0, '_sym1_start is not aligned as specified in linker fragment'
assert sym1_end % 12 == 0, '_sym1_end is not aligned as specified in linker fragment'
print('check placement pass: _sym1_start < func1 < __sym1_end and alignments checked')

func0 = check_location('func0', '.iram0.text')

if args.non_contiguous_sram:
    assert func1 < func0, 'check placement fail: func1 comes after func0'
    print('check placement pass: func1 < func0 checked')
else:
    assert func1 > func0, 'check placement fail: func0 comes after func1'
    print('check placement pass: func1 > func0 checked')

# src1:func2 (rtc) - explicit mapping for func2 using 'rtc' scheme
if not args.no_rtc:
    check_location('func2', '.rtc.text')

# src1 (default) - only func3 in src1 remains that has not been
# mapped using a different scheme
check_location('func3', '.flash.text')

check_location('func4', '.iram0.text')
