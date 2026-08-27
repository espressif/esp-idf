# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.flaky(reruns=2, reruns_delay=5)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_esp_partition(dut: Dut) -> None:
    # Erase + whole-flash get_sha256 can exceed the default Unity expect window
    dut.expect_unity_test_output(timeout=120)
