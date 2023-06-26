# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c2
@pytest.mark.ethernet
def test_mqtt_client(dut: Dut) -> None:
    dut.expect_unity_test_output()
