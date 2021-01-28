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

from pyparsing import LineEnd, Literal, ParseException, SkipTo, Word, alphanums, hexnums

argparser = argparse.ArgumentParser()

argparser.add_argument('objdump')
argparser.add_argument('elf')

args = argparser.parse_args()

contents = subprocess.check_output([args.objdump, '-t', args.elf]).decode()


def check_location(symbol, expected):
    pattern = Word(alphanums + '._').setResultsName('actual') + Word(hexnums) + Literal(symbol) + LineEnd()
    pattern = SkipTo(pattern) + pattern

    try:
        results = pattern.parseString(contents)
    except ParseException:
        print("check placement fail: '%s' was not found" % (symbol))
        exit(1)

    if results.actual != expected:
        print("check placement fail: '%s' was placed in '%s', not in '%s'" % (symbol, results.actual, expected))
        exit(1)

    print("check placement pass: '%s' was successfully placed in '%s'" % (symbol, results.actual))


# src1:func1 (noflash) - explicit mapping for func2 using 'rtc' scheme
check_location('func1', '.iram0.text')

# src1:func2 (rtc) - explicit mapping for func2 using 'rtc' scheme
check_location('func2', '.rtc.text')

# src1 (default) - only func3 in src1 remains that has not been
# mapped using a different scheme
check_location('func3', '.flash.text')

# * (noflash) - no explicit mapping for src2
check_location('func4', '.iram0.text')
