# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.supported_targets
@pytest.mark.parametrize(
    'config',
    [
        'default',
    ],
    indirect=True,
)
def test_pthread(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='!thread-specific', timeout=300)


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        pytest.param('single_core_esp32', marks=[pytest.mark.esp32]),
        pytest.param('single_core_esp32s3', marks=[pytest.mark.esp32s3]),
    ],
    indirect=True,
)
def test_pthread_single_core(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='!thread-specific', timeout=300)


@pytest.mark.generic
@pytest.mark.supported_targets
@pytest.mark.parametrize(
    'config',
    [
        'tls',
    ],
    indirect=True,
)
def test_pthread_tls(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='thread-specific', timeout=300)


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        pytest.param('single_core_esp32_tls', marks=[pytest.mark.esp32]),
    ],
    indirect=True,
)
def test_pthread_single_core_tls(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='thread-specific', timeout=300)


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.esp32
def test_pthread_qemu(dut: Dut) -> None:
    for case in dut.test_menu:
        if 'qemu-ignore' not in case.groups and case.type == 'normal':
            dut._run_normal_case(case, timeout=75)


@pytest.mark.linux
@pytest.mark.host_test
def test_pthread_linux(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)
