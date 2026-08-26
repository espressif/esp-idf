# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
from time import sleep

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_nvs_console(dut: Dut) -> None:
    # Wait until the console prompt appears
    dut.expect('nvs> ')

    # Write CLI command "version"
    dut.write('version')
    sleep(0.5)

    # Check if following strings are present in the "version" command output
    dut.expect('IDF Version')
    dut.expect('Chip info')
