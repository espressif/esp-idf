# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32c3
@pytest.mark.generic
def test_esp_event(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32
@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.parametrize('qemu_extra_args', [
    '-global driver=timer.esp32.timg,property=wdt_disable,value=true',
], indirect=True)  # need to disable wdt since it is not synchronized with target cpu clock on QEMU for ESP32
def test_esp_event_qemu_esp32(dut: Dut) -> None:
    for case in dut.test_menu:
        if 'qemu-ignore' not in case.groups and not case.is_ignored and case.type == 'normal':
            dut._run_normal_case(case)


@pytest.mark.esp32c3
@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.parametrize('qemu_extra_args', [
    '-icount 3',
], indirect=True)  # need to add -icount 3 to make WDT accurate on QEMU for ESP32-C3
def test_esp_event_qemu_esp32c3(dut: Dut) -> None:
    for case in dut.test_menu:
        if 'qemu-ignore' not in case.groups and not case.is_ignored and case.type == 'normal':
            dut._run_normal_case(case)


@pytest.mark.linux
@pytest.mark.host_test
def test_esp_event_posix_simulator(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('*')
    dut.expect(r'\d{2} Tests 0 Failures 0 Ignored', timeout=120)
