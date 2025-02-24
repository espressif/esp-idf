# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os
import subprocess
import sys

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@idf_parametrize('config', ['default'], indirect=['config'])
@idf_parametrize(
    'target,markers',
    [
        (
            'supported_targets',
            (pytest.mark.generic, pytest.mark.temp_skip(targets=['esp32c2'], reason='must have 4MB')),
        ),
        ('esp32c2', (pytest.mark.generic, pytest.mark.flash_4mb)),
    ],
    indirect=['target'],
)
def test_otatool_example(dut: Dut) -> None:
    # Verify factory firmware
    dut.expect('OTA Tool Example')
    dut.expect('Example end')

    # Close connection to DUT
    dut.serial.close()

    script_path = os.path.join(str(os.getenv('IDF_PATH')), 'examples', 'system', 'ota', 'otatool', 'otatool_example.py')
    binary_path = ''

    for flash_file in dut.app.flash_files:
        if 'otatool.bin' in flash_file[1]:
            binary_path = flash_file[1]
            break
    subprocess.check_call([sys.executable, script_path, '--binary', binary_path])
