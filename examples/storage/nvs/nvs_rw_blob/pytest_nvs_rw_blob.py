# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_examples_nvs_rw_blob(dut: Dut) -> None:
    # Save and read test data
    dut.expect('Saving test data blob...', timeout=20)
    # Save array data
    # Read updated data
    dut.expect('Reading updated blob data:', timeout=20)
    dut.expect('Reading test data blob:', timeout=20)
    # Verify array data reading
    dut.expect('Reading array data blob:', timeout=20)
    dut.expect('Blob operations completed.', timeout=20)
