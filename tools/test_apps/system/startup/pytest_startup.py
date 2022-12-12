# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.preview_targets
@pytest.mark.generic
@pytest.mark.parametrize('config', [
    'default',
    'flash_80m_qio',
    'no_vfs',
    'no_vfs_partial',
    'opt_o0',
    'opt_o2',
    'stack_check',
    'verbose_log',
], indirect=True)
def test_sys_startup(dut: Dut) -> None:
    dut.expect_exact('app_main running')


@pytest.mark.esp32
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize('config', ['single_core_variant'], indirect=True)
def test_sys_startup_single_core_variant(dut: Dut) -> None:
    dut.expect('Running on single core variant of a chip, but app is built with multi-core support.')
    dut.expect(r'abort\(\) was called at PC [0-9xa-f]+ on core 0')
