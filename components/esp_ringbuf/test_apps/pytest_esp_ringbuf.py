# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32s2
@pytest.mark.parametrize('config', [
    'default',
    'ringbuf_flash',
])
def test_esp_ringbuf(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.parametrize('target', [
    'esp32',
    'esp32c3',
])
@pytest.mark.parametrize('config', [
    'default',
    'ringbuf_flash',
])
def test_esp_ringbuf_qemu(dut: Dut) -> None:
    for case in dut.test_menu:
        if 'qemu-ignore' not in case.groups and not case.is_ignored and case.type == 'normal':
            dut._run_normal_case(case)


@pytest.mark.linux
@pytest.mark.host_test
def test_esp_ringbuf_posix_simulator(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='linux')
