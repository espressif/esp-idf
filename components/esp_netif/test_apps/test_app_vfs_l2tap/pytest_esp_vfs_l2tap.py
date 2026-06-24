# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.parametrize(
    'config, target',
    [
        pytest.param('default_esp32', 'esp32', marks=[pytest.mark.eth_ip101]),
        pytest.param('default_esp32p4', 'esp32p4', marks=[pytest.mark.eth_ip101]),
    ],
    indirect=['target'],
)
def test_esp_netif_vfs_l2tp(dut: Dut) -> None:
    dut.run_all_single_board_cases()
