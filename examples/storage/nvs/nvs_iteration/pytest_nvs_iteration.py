# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_examples_nvs_iteration(dut: Dut) -> None:
    dut.expect("Iterating over NVS_TYPE_U32 data in namespace '_mock_data'...", timeout=5)
    dut.expect('Iterated over 7 entries.', timeout=5)
    dut.expect("Iterating over NVS_TYPE_I8 data in namespace '_mock_data'...", timeout=5)
    dut.expect('Iterated over 7 entries.', timeout=5)
    dut.expect("Iterating over NVS_TYPE_ANY data in namespace '_mock_data'...", timeout=5)
    dut.expect('Iterated over 14 entries.', timeout=5)
