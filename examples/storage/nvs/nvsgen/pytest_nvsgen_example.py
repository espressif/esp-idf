# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_nvsgen_example(dut: Dut) -> None:
    dut.expect('Reading values from NVS', timeout=10)
    dut.expect('Reading values from NVS done - all OK', timeout=10)
