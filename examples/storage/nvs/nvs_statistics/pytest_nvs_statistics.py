# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_examples_nvs_statistics(dut: Dut) -> None:
    dut.expect('Getting post-commit NVS statistics...', timeout=5)
    dut.expect('Newly used entries match expectation.', timeout=5)

    dut.expect('Getting post-commit NVS statistics...', timeout=5)
    dut.expect('Newly used entries match expectation.', timeout=5)

    dut.expect('Returning from app_main().', timeout=5)
