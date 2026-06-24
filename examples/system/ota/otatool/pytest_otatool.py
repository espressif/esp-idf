# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import logging
import os
import subprocess
import sys
import time

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

    # Allow the OS to fully release the serial port. pytest-embedded's
    # QueueFeederThread may still hold the port FD when close() returns.
    time.sleep(2)

    script_path = os.path.join(str(os.getenv('IDF_PATH')), 'examples', 'system', 'ota', 'otatool', 'otatool_example.py')
    binary_path = ''

    for flash_file in dut.app.flash_files:
        if 'otatool.bin' in flash_file[1]:
            binary_path = flash_file[1]
            break

    # Retry the subprocess to handle transient serial port contention.
    # The otatool_example.py subprocess opens the serial port independently
    # via esptool, and may fail if pytest-embedded's QueueFeederThread has
    # not fully released the port file descriptor yet.
    last_err = None
    for attempt in range(3):
        try:
            subprocess.check_call([sys.executable, script_path, '--binary', binary_path])
            return
        except subprocess.CalledProcessError as e:
            last_err = e
            logging.warning('otatool subprocess attempt %d/3 failed: %s', attempt + 1, e)
            time.sleep(3)

    assert last_err is not None
    raise last_err
