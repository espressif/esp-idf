# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32c3
@pytest.mark.generic
def test_esp_app_format(dut: Dut) -> None:
    dut.expect_unity_test_output()


@pytest.mark.linux
@pytest.mark.host_test
def test_esp_app_format_linux(dut: Dut) -> None:
    dut.expect_unity_test_output()
