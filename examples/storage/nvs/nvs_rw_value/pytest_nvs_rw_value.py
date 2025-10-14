# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_examples_nvs_rw_value(dut: Dut) -> None:
    dut.serial.erase_flash()
    dut.serial.flash()

    dut.expect('Opening Non-Volatile Storage \\(NVS\\) handle...', timeout=20)
    dut.expect('Writing counter to NVS...', timeout=20)
    dut.expect('Reading counter from NVS...', timeout=20)
    dut.expect('Writing string to NVS...', timeout=20)
    dut.expect('Reading string from NVS...', timeout=20)
    dut.expect('Finding keys in NVS...', timeout=20)
    dut.expect('Deleting key from NVS...', timeout=20)
    dut.expect('Committing updates in NVS...', timeout=20)
    dut.expect('NVS handle closed.', timeout=20)
    dut.expect('Returned to app_main', timeout=20)
