# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import os
import subprocess
import sys

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
def test_examples_parttool(dut: Dut) -> None:
    # Verify factory firmware
    dut.expect('Partitions Tool Example')
    dut.expect('Example end')

    # Close connection to DUT
    dut.pexpect_proc.terminate()
    dut.serial.stop_redirect_thread()

    # Run the example python script
    idf_path = os.getenv('IDF_PATH')
    assert idf_path is not None
    script_path = os.path.join(idf_path, 'examples', 'storage', 'parttool', 'parttool_example.py')
    binary_path = os.path.join(dut.app.binary_path, 'parttool.bin')
    subprocess.check_call([sys.executable, script_path, '--binary', binary_path, '--port', dut.serial.port])
