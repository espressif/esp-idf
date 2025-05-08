# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'global_dns',
        'dns_per_netif',
        'loopback',  # test config without LWIP
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s2', 'esp32c3'], indirect=['target'])
def test_esp_netif(dut: Dut) -> None:
    dut.expect_unity_test_output()
