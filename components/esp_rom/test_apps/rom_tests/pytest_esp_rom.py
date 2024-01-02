# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32c3
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.generic
def test_esp_rom(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32c3
@pytest.mark.host_test
@pytest.mark.qemu
def test_esp_rom_qemu(dut: Dut) -> None:
    dut.run_all_single_board_cases()
