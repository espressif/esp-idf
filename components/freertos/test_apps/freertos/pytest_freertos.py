# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

CONFIGS = [
    pytest.param(
        'default',
        marks=[
            pytest.mark.supported_targets,
        ],
    ),
    pytest.param('freertos_options', marks=[pytest.mark.supported_targets]),
    pytest.param('psram', marks=[pytest.mark.esp32, pytest.mark.esp32s3, pytest.mark.esp32p4, pytest.mark.esp32c5]),
    pytest.param('single_core', marks=[pytest.mark.esp32, pytest.mark.esp32p4]),
    # TODO: [ESP32C5] IDF-10335
    # TODO: [ESP32C61] IDF-11146
    pytest.param(
        'smp',
        marks=[
            pytest.mark.supported_targets,
            pytest.mark.temp_skip_ci(
                targets=['esp32p4', 'esp32c5', 'esp32c61', 'esp32h21'], reason='test failed/TBD IDF-8113'
            ),
        ],
    ),
    pytest.param('tickless_idle', marks=[pytest.mark.supported_targets]),
]


@pytest.mark.generic
@idf_parametrize(
    'config,target,markers',
    [
        ('default', 'supported_targets'),
        ('freertos_options', 'supported_targets'),
        ('tickless_idle', 'supported_targets'),
        ('psram', 'esp32'),
        ('psram', 'esp32c5'),
        ('psram', 'esp32p4'),
        ('psram', 'esp32s3'),
        ('single_core', 'esp32'),
        ('single_core', 'esp32p4'),
        (
            'smp',
            'supported_targets',
            (
                pytest.mark.temp_skip_ci(
                    targets=['esp32p4', 'esp32c5', 'esp32c61', 'esp32h21'], reason='test failed/TBD IDF-8113'
                ),
            ),
        ),
    ],
    indirect=['config', 'target'],
)
def test_freertos(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.flash_suspend
@idf_parametrize(
    'config,target',
    [
        ('flash_auto_suspend', 'esp32c3'),
    ],
    indirect=['config', 'target'],
)
def test_freertos_flash_auto_suspend(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize('config', ['freertos_options'], indirect=True)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_task_notify_too_high_index_fails(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('"Notify too high index fails"')
    dut.expect('assert failed: xTaskGenericNotify', timeout=5)
    dut.expect('uxIndexToNotify < [0-9]+')
    dut.expect_exact('Rebooting...')


@pytest.mark.generic
@pytest.mark.parametrize('config', ['freertos_options'], indirect=True)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_task_notify_wait_too_high_index_fails(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('"Notify Wait too high index fails"')
    dut.expect('assert failed: xTaskGenericNotifyWait', timeout=5)
    dut.expect('uxIndexToWait < [0-9]+', timeout=5)
    dut.expect_exact('Rebooting...')


@pytest.mark.generic
@pytest.mark.parametrize('config', ['default'], indirect=True)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_port_must_assert_in_isr(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('"port must assert if in ISR context"')
    dut.expect('assert failed: vPortAssertIfInISR', timeout=5)
    dut.expect_exact('Rebooting...')
