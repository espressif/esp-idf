# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import platform
import subprocess

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.parametrize('config', [
    pytest.param('default_ip101', marks=[pytest.mark.ethernet_router]),
    pytest.param('default_generic', marks=[pytest.mark.ethernet_router]),
    pytest.param('default_dm9051', marks=[pytest.mark.eth_dm9051]),
], indirect=True)
def test_esp_eth_basic(
    dut: Dut
) -> None:
    # wait for ip received
    dut_ip = dut.expect(r'esp_netif_handlers: .+ ip: (\d+\.\d+\.\d+\.\d+),').group(1)
    # ping it once
    param = '-n' if platform.system().lower() == 'windows' else '-c'
    command = ['ping', param, '1', dut_ip]
    output = subprocess.run(command, capture_output=True)
    assert 'unreachable' not in str(output.stdout)
