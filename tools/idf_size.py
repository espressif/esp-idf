#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#

import argparse
import os
import subprocess
import sys

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--format')
    parser.add_argument('-l', '--legacy', action='store_true', default=os.environ.get('ESP_IDF_SIZE_LEGACY', '0') == '1')
    args, rest = parser.parse_known_args()

    if not args.legacy and args.format != 'json':
        try:
            import esp_idf_size.ng  # noqa: F401
        except ImportError:
            print('warning: refactored esp-idf-size not installed, using legacy mode', file=sys.stderr)
            args.legacy = True
        else:
            os.environ['ESP_IDF_SIZE_NG'] = '1'

    if args.legacy and args.format in ['json2', 'raw', 'tree']:
        sys.exit(f'Legacy esp-idf-size does not support {args.format} format')

    if args.format is not None:
        rest = ['--format', args.format] + rest

    sys.exit(subprocess.run([sys.executable, '-m', 'esp_idf_size'] + rest).returncode)
