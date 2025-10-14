# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_bootloader_support(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.host_test
@pytest.mark.qemu
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_bootloader_support_qemu_esp32(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.host_test
@pytest.mark.qemu
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_bootloader_support_qemu_esp32c3(dut: Dut) -> None:
    for case in dut.test_menu:
        if 'qemu-ignore-c3' not in case.groups:
            dut._run_normal_case(case)
