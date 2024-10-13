# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.generic
def test_nvsgen_example(dut: Dut) -> None:
    dut.expect('Reading values from NVS', timeout=10)
    dut.expect('Reading values from NVS done - all OK', timeout=10)
