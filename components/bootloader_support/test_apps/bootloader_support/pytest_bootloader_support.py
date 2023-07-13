# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.supported_targets
def test_bootloader_support(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.esp32
def test_bootloader_support_qemu_esp32(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.esp32c3
def test_bootloader_support_qemu_esp32c3(dut: Dut) -> None:
    for case in dut.test_menu:
        if 'qemu-ignore-c3' not in case.groups:
            dut._run_normal_case(case)
