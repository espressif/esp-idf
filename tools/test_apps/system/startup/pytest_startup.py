# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.parametrize('config', [
    pytest.param('flash_80m_qio',  marks=[pytest.mark.supported_targets]),
    pytest.param('no_vfs', marks=[pytest.mark.supported_targets]),
    pytest.param('no_vfs_partial', marks=[pytest.mark.supported_targets]),
    pytest.param('opt_o0', marks=[pytest.mark.supported_targets]),
    pytest.param('opt_o2', marks=[pytest.mark.supported_targets]),
    pytest.param('stack_check_verbose_log', marks=[pytest.mark.supported_targets]),
    pytest.param('sram1_iram', marks=[pytest.mark.esp32]),
    pytest.param('main_task_cpu1', marks=[pytest.mark.esp32, pytest.mark.esp32s3]),
], indirect=True)
def test_sys_startup(dut: Dut) -> None:
    dut.expect_exact('app_main running')


@pytest.mark.esp32
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize('config', [
    'single_core_variant',
], indirect=True)
def test_sys_startup_single_core_variant(dut: Dut) -> None:
    dut.expect('Running on single core variant of a chip, but app is built with multi-core support.')
    dut.expect(r'abort\(\) was called at PC [0-9xa-f]+ on core 0')
