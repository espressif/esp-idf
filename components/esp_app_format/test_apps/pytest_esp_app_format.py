# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32s2', 'esp32c3'], indirect=['target'])
def test_esp_app_format(dut: Dut) -> None:
    dut.expect_unity_test_output()


@pytest.mark.host_test
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_esp_app_format_linux(dut: Dut) -> None:
    dut.expect_unity_test_output()
