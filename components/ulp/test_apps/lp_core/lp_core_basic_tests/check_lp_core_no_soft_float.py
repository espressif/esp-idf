#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import argparse
from pathlib import Path

SOFT_FLOAT_SYMBOLS = {
    '__adddf3',
    '__addsf3',
    '__divdf3',
    '__divsf3',
    '__eqdf2',
    '__eqsf2',
    '__extendsfdf2',
    '__fixdfsi',
    '__fixsfsi',
    '__fixunsdfsi',
    '__fixunssfsi',
    '__floatunsidf',
    '__floatunsisf',
    '__floatsidf',
    '__floatsisf',
    '__gedf2',
    '__gesf2',
    '__gtdf2',
    '__gtsf2',
    '__ledf2',
    '__lesf2',
    '__ltdf2',
    '__ltsf2',
    '__muldf3',
    '__mulsf3',
    '__nedf2',
    '__nesf2',
    '__subdf3',
    '__subsf3',
    '__truncdfsf2',
}


def main() -> None:
    parser = argparse.ArgumentParser(description='Check that an LP core app does not link soft-float helpers')
    parser.add_argument('symbol_file', type=Path)
    args = parser.parse_args()

    linked_symbols = set()
    for line in args.symbol_file.read_text().splitlines():
        fields = line.split()
        if fields:
            linked_symbols.add(fields[-1])

    linked_soft_float = sorted(linked_symbols & SOFT_FLOAT_SYMBOLS)
    if linked_soft_float:
        symbols = ', '.join(linked_soft_float)
        raise SystemExit(f'{args.symbol_file}: LP core app links soft-float helper symbols: {symbols}')


if __name__ == '__main__':
    main()
