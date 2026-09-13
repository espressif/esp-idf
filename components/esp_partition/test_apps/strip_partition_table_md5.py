#!/usr/bin/env python
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Remove the MD5 checksum record from a partition table binary.

Produces the table layout used before ESP-IDF v3.1, which is what
CONFIG_PARTITION_TABLE_MD5_TOLERATE_MISSING exists to load. The build cannot
generate one directly: PARTITION_TABLE_MD5 has to stay enabled for that option to
be selectable, so the record is stripped from the generated binary instead.

This runs on every build while the table itself is only regenerated when its CSV
changes, so a table that already carries no record is left alone rather than
treated as an error.
"""
import sys

ENTRY_SIZE = 32
MD5_MAGIC = b'\xeb\xeb'


def main() -> None:
    path = sys.argv[1]
    with open(path, 'rb') as f:
        table = bytearray(f.read())

    for offset in range(0, len(table), ENTRY_SIZE):
        if table[offset:offset + ENTRY_SIZE][:2] != MD5_MAGIC:
            continue
        # Drop the record and pull the terminator up into its place, so the table
        # ends immediately after the last real entry.
        del table[offset:offset + ENTRY_SIZE]
        table += b'\xff' * ENTRY_SIZE
        with open(path, 'wb') as f:
            f.write(table)
        break


if __name__ == '__main__':
    main()
