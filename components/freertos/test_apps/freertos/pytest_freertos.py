# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


# Test strategy for FreeRTOS coverage (all single board tests):
#
# 1. Run 'default' config on ALL supported targets
#    New targets will automatically be added to the test matrix once they are part of the supported_targets set.
# 2. Distribute other configs across targets focusing on key targets per architecture/core-count/feature-set.
@pytest.mark.generic
@idf_parametrize(
    'config,target,markers',
    [
        # Default configuration: All supported targets
        ('default', 'supported_targets'),
        # FreeRTOS options (Test options across architectures)
        ('freertos_options', 'esp32'),  # Xtensa dual-core
        ('freertos_options', 'esp32s2'),  # Xtensa single-core
        ('freertos_options', 'esp32c6'),  # RISC-V single-core
        ('freertos_options', 'esp32p4'),  # RISC-V dual-core
        # Tickless idle (Test tickless idle on select targets)
        ('tickless_idle', 'esp32c61'),
        ('tickless_idle', 'esp32p4'),
        ('tickless_idle', 'esp32'),
        # PSRAM (Test PSRAM on select targets)
        ('psram', 'esp32s3'),
        ('psram', 'esp32c5'),
        # Single-core mode (Test single-core mode on dual-core targets)
        ('single_core', 'esp32'),
        ('single_core', 'esp32p4'),
        # SMP (Preview feature, limited testing on esp32 only)
        ('smp', 'esp32'),
        # Flash auto-suspend (Specific feature test)
        (
            'flash_auto_suspend',
            'esp32c3',
            (pytest.mark.flash_suspend,),
        ),
    ],
    indirect=['config', 'target'],
)
def test_freertos(dut: Dut) -> None:
    dut.run_all_single_board_cases()


# Assertion failure tests (Distributed to targets with minimal coverage)
@pytest.mark.generic
@pytest.mark.parametrize('config', ['freertos_options'], indirect=True)
@idf_parametrize(
    'target',
    [
        'esp32c2',
    ],
    indirect=['target'],
)
def test_task_notify_too_high_index_fails(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('"Notify too high index fails"')
    dut.expect('assert failed: xTaskGenericNotify', timeout=5)
    dut.expect('uxIndexToNotify < [0-9]+')
    dut.expect_exact('Rebooting...')


@pytest.mark.generic
@pytest.mark.parametrize('config', ['freertos_options'], indirect=True)
@idf_parametrize(
    'target',
    [
        'esp32s2',
    ],
    indirect=['target'],
)
def test_task_notify_wait_too_high_index_fails(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('"Notify Wait too high index fails"')
    dut.expect('assert failed: xTaskGenericNotifyWait', timeout=5)
    dut.expect('uxIndexToWait < [0-9]+', timeout=5)
    dut.expect_exact('Rebooting...')


@pytest.mark.generic
@pytest.mark.parametrize('config', ['default'], indirect=True)
@idf_parametrize(
    'target',
    [
        'esp32s3',  # Xtensa
        'esp32p4',  # RISC-V
    ],
    indirect=['target'],
)
def test_port_must_assert_in_isr(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('"port must assert if in ISR context"')
    dut.expect('assert failed: vPortAssertIfInISR', timeout=5)
    dut.expect_exact('Rebooting...')
