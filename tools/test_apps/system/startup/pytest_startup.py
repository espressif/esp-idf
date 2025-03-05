# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'config,target',
    [
        ('flash_80m_qio', 'supported_targets'),
        ('no_vfs', 'supported_targets'),
        ('no_vfs_partial', 'supported_targets'),
        ('opt_o0', 'supported_targets'),
        ('opt_o2', 'supported_targets'),
        ('stack_check_verbose_log', 'supported_targets'),
        ('sram1_iram', 'esp32'),
        ('main_task_cpu1', 'esp32'),
        ('main_task_cpu1', 'esp32s3'),
    ],
    indirect=['config', 'target'],
)
def test_sys_startup(dut: Dut) -> None:
    dut.expect_exact('app_main running')


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'single_core_variant',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32s3'], indirect=['target'])
def test_sys_startup_single_core_variant(dut: Dut) -> None:
    dut.expect('Running on single core variant of a chip, but app is built with multi-core support.')
    dut.expect(r'abort\(\) was called at PC [0-9xa-f]+ on core 0')
