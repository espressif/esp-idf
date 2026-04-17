# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32c2', 'esp32c3', 'esp32c6', 'esp32h2', 'esp32p4'], indirect=['target'])
def test_efuse(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.qemu
@pytest.mark.host_test
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_efuse_qemu(dut: Dut) -> None:
    dut.run_all_single_board_cases()
