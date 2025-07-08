# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.host_test
@pytest.mark.parametrize(
    'config',
    [
        'no_poisoning',
        'light_poisoning',
        'comprehensive_poisoning',
    ],
)
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_multi_heap_linux(dut: Dut) -> None:
    dut.expect_exact('All tests passed', timeout=180)
