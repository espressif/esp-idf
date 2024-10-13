# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os
import subprocess
import sys

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.generic
def test_examples_parttool(dut: Dut) -> None:
    # Verify factory firmware
    dut.expect('Partitions Tool Example')
    dut.expect('Example end')

    # Close connection to DUT
    dut.serial.close()

    # Run the example python script
    idf_path = os.getenv('IDF_PATH')
    assert idf_path is not None
    script_path = os.path.join(idf_path, 'examples', 'storage', 'parttool', 'parttool_example.py')
    binary_path = os.path.join(dut.app.binary_path, 'parttool.bin')
    subprocess.check_call([sys.executable, script_path, '--binary', binary_path, '--port', dut.serial.port])

    # following tests check the external interface (parsing) of the parttool commands
    with open('custom.bin', 'wb') as f:
        f.write(b'0' * 1024 * 4)

    PARTTOOL = os.path.join(idf_path, 'components', 'partition_table', 'parttool.py')
    BASE_CMD = [sys.executable, PARTTOOL, '--port', dut.serial.port]

    cmds = ['read_partition --partition-type=data --partition-subtype=nvs --output custom1.bin',
            'erase_partition --partition-name=custom',
            'write_partition --partition-name=custom --input custom.bin',
            'get_partition_info --partition-boot-default --info size']

    for cmd in cmds:
        subprocess.check_call(BASE_CMD + cmd.split())

    clean_files = ['custom.bin', 'custom1.bin']
    for clean_file in clean_files:
        os.unlink(clean_file)
