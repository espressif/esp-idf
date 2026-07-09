# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

from typing import TYPE_CHECKING

import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

# Import for type-checking only — at runtime eth_test_runner pulls in dependencies that
# are not available during pytest collection. The real import is deferred to the fixture body.
if TYPE_CHECKING:
    from conftest import EthTestRunner


# ----------- IP101 -----------
@pytest.mark.ethernet
@pytest.mark.parametrize('config', ['default_generic', 'release_generic', 'single_core_generic'], indirect=True)
@pytest.mark.flaky(reruns=3, reruns_delay=5)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_ethernet(dut: IdfDut, eth_test_runner: 'EthTestRunner') -> None:
    eth_test_runner.run_ethernet_test_apps(dut)


@pytest.mark.ethernet
@pytest.mark.parametrize(
    'config',
    [
        'default_generic',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_emac(dut: IdfDut, eth_test_runner: 'EthTestRunner') -> None:
    dut.run_all_single_board_cases(group='esp_emac', timeout=240)
    dut.serial.hard_reset()
    eth_test_runner.run_ethernet_heap_alloc_test(dut)


@pytest.mark.eth_ip101
@pytest.mark.parametrize(
    'config',
    [
        'default_generic',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_eth_ip101(dut: IdfDut, eth_test_runner: 'EthTestRunner') -> None:
    eth_test_runner.run_ethernet_l2_test(dut)


# ----------- IP101 ESP32P4 -----------
@pytest.mark.parametrize(
    'config, target',
    [
        pytest.param('default_generic_esp32p4', 'esp32p4', marks=[pytest.mark.eth_ip101]),
        pytest.param('default_generic_esp32p4v1', 'esp32p4', marks=[pytest.mark.eth_ip101, pytest.mark.esp32p4_rev1]),
    ],
    indirect=['target'],
)
def test_esp32p4_ethernet(dut: IdfDut, eth_test_runner: 'EthTestRunner') -> None:
    eth_test_runner.run_ethernet_test_apps(dut)
    dut.serial.hard_reset()
    eth_test_runner.run_ethernet_l2_test(dut)


@pytest.mark.parametrize(
    'config, target',
    [
        pytest.param('default_generic_esp32p4', 'esp32p4', marks=[pytest.mark.eth_ip101]),
        pytest.param('default_generic_esp32p4v1', 'esp32p4', marks=[pytest.mark.eth_ip101, pytest.mark.esp32p4_rev1]),
    ],
    indirect=['target'],
)
def test_esp32p4_emac(dut: IdfDut, eth_test_runner: 'EthTestRunner') -> None:
    dut.run_all_single_board_cases(group='esp_emac', timeout=240)
    dut.serial.hard_reset()
    eth_test_runner.run_ethernet_heap_alloc_test(dut)


@pytest.mark.parametrize(
    'config, target',
    [
        pytest.param('rmii_clko_esp32p4', 'esp32p4', marks=[pytest.mark.eth_ip101]),
        pytest.param('rmii_clko_esp32p4v1', 'esp32p4', marks=[pytest.mark.eth_ip101, pytest.mark.esp32p4_rev1]),
    ],
    indirect=['target'],
)
def test_esp32p4_emac_clko(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='esp_emac_clk_out')


# ----------- LAN8720 -----------
@pytest.mark.eth_lan8720
@pytest.mark.parametrize(
    'config',
    [
        'default_lan8720',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_eth_lan8720(dut: IdfDut, eth_test_runner: 'EthTestRunner') -> None:
    eth_test_runner.run_ethernet_test_apps(dut)
    dut.serial.hard_reset()
    eth_test_runner.run_ethernet_l2_test(dut)


# ----------- EMAC Sleep Retention -----------
@pytest.mark.eth_ip101
@pytest.mark.parametrize(
    'config',
    [
        'emac_sleep_retention',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_emac_sleep_retention(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='sleep_retention', timeout=120)
