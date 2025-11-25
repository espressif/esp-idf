# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'default',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_pthread(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='!thread-specific', timeout=300)


@pytest.mark.generic
@idf_parametrize(
    'config,target', [('single_core_esp32', 'esp32'), ('single_core_esp32s3', 'esp32s3')], indirect=['config', 'target']
)
def test_pthread_single_core(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='!thread-specific', timeout=300)


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'tls',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_pthread_tls(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='thread-specific', timeout=300)


@pytest.mark.generic
@idf_parametrize('config', ['single_core_esp32_tls'], indirect=['config'])
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_pthread_single_core_tls(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='thread-specific', timeout=300)


@pytest.mark.host_test
@pytest.mark.qemu
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_pthread_qemu(dut: Dut) -> None:
    for case in dut.test_menu:
        if 'qemu-ignore' not in case.groups and case.type == 'normal':
            dut._run_normal_case(case, timeout=75)


@pytest.mark.host_test
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_pthread_linux(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)
