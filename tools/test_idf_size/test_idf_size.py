#!/usr/bin/env python
# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import sys
from pathlib import Path
from subprocess import DEVNULL
from subprocess import run


def test_idf_size() -> None:
    # Simple test to make sure that the idf_size.py wrapper is compatible
    # with idf.py minimum required python version.
    logging.info('idf_size.py python compatibility check')
    idf_size_path = Path(os.environ['IDF_PATH']) / 'tools' / 'idf_size.py'
    run([sys.executable, idf_size_path, '--help'], stdout=DEVNULL, stderr=DEVNULL, check=True)
