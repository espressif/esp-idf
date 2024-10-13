# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32c3
@pytest.mark.generic
@pytest.mark.parametrize('config', [
    'global_dns',
    'dns_per_netif',
    'loopback',             # test config without LWIP
], indirect=True)
def test_esp_netif(dut: Dut) -> None:
    dut.expect_unity_test_output()
