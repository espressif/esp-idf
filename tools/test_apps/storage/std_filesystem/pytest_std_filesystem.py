# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.qemu
@pytest.mark.host_test
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_std_filesystem(dut: Dut) -> None:
    dut.expect_exact('All tests passed', timeout=200)
