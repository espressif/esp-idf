#!/usr/bin/env python
#
# Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#

# Check placements in this test app for main
# specified in main/linker.lf

import argparse
import subprocess

from pyparsing import LineEnd, LineStart, Literal, Optional, Word, alphanums, hexnums

argparser = argparse.ArgumentParser()

argparser.add_argument('objdump')
argparser.add_argument('elf')

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

# src1:func2 (rtc) - explicit mapping for func2 using 'rtc' scheme
check_location('func2', '.rtc.text')

# src1 (default) - only func3 in src1 remains that has not been
# mapped using a different scheme
check_location('func3', '.flash.text')

check_location('func4', '.iram0.text')
