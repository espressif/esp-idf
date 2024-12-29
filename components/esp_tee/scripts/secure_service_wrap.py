#!/usr/bin/env python
# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import re
from typing import List


def main() -> None:
    parser = argparse.ArgumentParser(description='Generate secure service wrap list')
    parser.add_argument('secure_service_tbl', type=str, help='Path to secure service table file')
    args = parser.parse_args()

    pattern: re.Pattern = re.compile(r'^[0-9A-Fa-fXx]+\s+IDF\s+(\S+)\s+\d+')

    with open(args.secure_service_tbl, 'r') as f:
        wrap_list: List[str] = [f'-Wl,--wrap={match.group(1)}'
                                for line in f if (match := pattern.match(line))]

    print(' '.join(wrap_list), end='')


if __name__ == '__main__':
    main()
