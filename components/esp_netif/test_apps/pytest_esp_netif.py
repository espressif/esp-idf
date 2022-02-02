# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.generic
def test_esp_netif(dut: Dut) -> None:
    dut.expect_unity_test_output()
