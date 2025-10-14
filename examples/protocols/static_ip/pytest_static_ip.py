# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.eth_ip101
@pytest.mark.parametrize(
    'config',
    [
        'default_ip101',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_static_ip(dut: Dut) -> None:
    dut.expect(r'Resolved IPv4 address: (\d+\.\d+\.\d+\.\d+)')
