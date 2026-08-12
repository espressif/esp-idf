# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.eth_ip101
@pytest.mark.parametrize('config', ['default_generic'], indirect=True)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_sublayer_events(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='sublayer_events', timeout=120)


@pytest.mark.eth_ip101
@pytest.mark.parametrize('config', ['default_generic'], indirect=True)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_sublayer_hooks(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='sublayer_hooks', timeout=120)


@pytest.mark.eth_ip101
@pytest.mark.parametrize('config', ['default_generic_esp32p4'], indirect=True)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_sublayer_events_esp32p4(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='sublayer_events', timeout=120)


@pytest.mark.eth_ip101
@pytest.mark.parametrize('config', ['default_generic_esp32p4'], indirect=True)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_sublayer_hooks_esp32p4(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='sublayer_hooks', timeout=120)
