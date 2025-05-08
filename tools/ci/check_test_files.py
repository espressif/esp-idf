#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os
import sys
from pathlib import Path

import pytest


sys.path.insert(0, os.path.dirname(__file__))
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

from idf_ci_utils import IDF_PATH  # noqa: E402

os.environ['IDF_PATH'] = IDF_PATH
os.environ['PYTEST_IGNORE_COLLECT_IMPORT_ERROR'] = '1'

from idf_pytest.plugin import IdfPytestEmbedded  # noqa: E402


def main() -> None:
    parser = argparse.ArgumentParser(description='Pytest linter check')
    parser.add_argument(
        'files',
        nargs='*',
        help='Python files to check (full paths separated by space)',
    )
    args = parser.parse_args()

    # Convert input files to pytest-compatible paths
    pytest_scripts = [str(Path(f).resolve()) for f in args.files]

    cmd = [
        '--collect-only',
        *pytest_scripts,
        '--target', 'all',
        '-p', 'test_linter',
    ]

    res = pytest.main(cmd, plugins=[IdfPytestEmbedded('all')])
    sys.exit(res)


if __name__ == '__main__':
    main()
