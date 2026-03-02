#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# Generate RST documentation of ESP error codes.
#
# Previously this script also generated the C lookup table.
# That path has been replaced by the composable
# error-code registration system (link-time arrays + err_codes_extract.py).
# The RST generation is kept so the esp_docs Sphinx extension
# (esp_docs.idf_extensions.esp_err_definitions) continues to work
# unchanged.

import argparse
import os
import sys
from typing import TextIO

# Allow importing err_codes_extract from the same directory
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from err_codes_extract import extract_all  # noqa: E402
from err_codes_extract import search_headers  # noqa: E402


def generate_rst_output(idf_path: str, fout: TextIO) -> None:
    """
    Generate RST output using the composable error code extraction system
    (err_codes_extract.py).  Produces the format expected by the
    ``.. include-build-file:: inc/esp_err_defs.inc`` directive.
    """
    components_dir = os.path.join(idf_path, 'components')
    headers = search_headers([components_dir], idf_path)
    err_codes = extract_all(headers, idf_path)
    entries = [(ec.name, ec.value, ec.comment) for ec in err_codes if ec.value is not None]

    # Sort: negative values first (ascending), then non-negative (ascending)
    entries.sort(key=lambda e: (e[1], e[0]))

    for name, value, comment in entries:
        fout.write(f':c:macro:`{name}` ')
        if value > 0:
            fout.write(f'**(0x{value:x})**')
        else:
            fout.write(f'({value:d})')
        if comment:
            fout.write(f': {comment}')
        fout.write('\n\n')


def main() -> None:
    if 'IDF_PATH' in os.environ:
        idf_path = os.environ['IDF_PATH']
    else:
        idf_path = os.path.realpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))

    parser = argparse.ArgumentParser(description='ESP-IDF error code RST documentation generator')
    parser.add_argument('--rst_output', required=True, help='Generate .rst output and save it into this file')
    args = parser.parse_args()

    with open(args.rst_output, 'w', encoding='utf-8') as fout:
        generate_rst_output(idf_path, fout)


if __name__ == '__main__':
    main()
