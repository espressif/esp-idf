# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import platform
import subprocess

import pytest
from pytest_embedded import Dut


@pytest.mark.parametrize(
    'config, target',
    [
        pytest.param('defaults', 'esp32', marks=[pytest.mark.eth_ip101]),
        pytest.param('lan8720_esp32', 'esp32', marks=[pytest.mark.eth_lan8720]),
        pytest.param('defaults', 'esp32p4', marks=[pytest.mark.eth_ip101]),
    ],
    indirect=['target'],
)
def test_esp_eth_basic(dut: Dut) -> None:
    # wait for ip received
    dut_ip = dut.expect(r'esp_netif_handlers: .+ ip: (\d+\.\d+\.\d+\.\d+),').group(1)
    # ping it once
    param = '-n' if platform.system().lower() == 'windows' else '-c'
    command = ['ping', param, '1', dut_ip]
    output = subprocess.run(command, capture_output=True)
    if 'unreachable' in str(output.stdout):
        raise RuntimeError('Host unreachable')
