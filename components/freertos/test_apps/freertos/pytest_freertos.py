# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded import Dut

CONFIGS = [
    pytest.param('default', marks=[pytest.mark.supported_targets, pytest.mark.temp_skip_ci(targets=['esp32h2'], reason='test failed')]),
    pytest.param('freertos_options', marks=[pytest.mark.supported_targets, pytest.mark.temp_skip_ci(targets=['esp32h2'], reason='test failed')]),
    pytest.param('psram', marks=[pytest.mark.esp32]),
    pytest.param('release', marks=[pytest.mark.supported_targets]),
    pytest.param('single_core', marks=[pytest.mark.esp32]),
    pytest.param('smp', marks=[pytest.mark.supported_targets, pytest.mark.temp_skip_ci(targets=['esp32h2'], reason='test failed')]),
]


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
def test_freertos(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize('config', ['freertos_options'], indirect=True)
def test_task_notify_too_high_index_fails(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('\"Notify too high index fails\"')
    dut.expect('assert failed: xTaskGenericNotify', timeout=5)
    dut.expect('uxIndexToNotify < [0-9]+')
    dut.expect_exact('Rebooting...')


@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize('config', ['freertos_options'], indirect=True)
def test_task_notify_wait_too_high_index_fails(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('\"Notify Wait too high index fails\"')
    dut.expect('assert failed: xTaskGenericNotifyWait', timeout=5)
    dut.expect('uxIndexToWait < [0-9]+', timeout=5)
    dut.expect_exact('Rebooting...')
