# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
from __future__ import print_function

import os
import subprocess
import sys

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.esp32c6
@pytest.mark.generic
def test_otatool_example(dut: Dut) -> None:
    # Verify factory firmware
    dut.expect('OTA Tool Example')
    dut.expect('Example end')

    # Close connection to DUT
    dut.serial.proc.close()

    script_path = os.path.join(str(os.getenv('IDF_PATH')), 'examples', 'system', 'ota', 'otatool', 'otatool_example.py')
    binary_path = ''

    for flash_file in dut.app.flash_files:
        if 'otatool.bin' in flash_file[1]:
            binary_path = flash_file[1]
            break
    subprocess.check_call([sys.executable, script_path, '--binary', binary_path])
