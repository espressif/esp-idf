# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import time

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32c5'], reason='c5 eco2 does not support top pd')
@idf_parametrize(
    'config,target',
    [
        ('default', 'supported_targets'),
        ('pd_vddsdio', 'supported_targets'),
        *(('psram', target) for target in soc_filtered_targets('SOC_SPIRAM_SUPPORTED == 1')),
        *(
            ('psram_with_pd_top', target)
            for target in soc_filtered_targets('SOC_SPIRAM_SUPPORTED == 1 and SOC_PM_SUPPORT_TOP_PD == 1')
        ),
        ('single_core_esp32', 'esp32'),
    ],
    indirect=['config', 'target'],
)
def test_esp_system(dut: Dut) -> None:
    # esp32p4 32MB PSRAM initialize in startup takes more than 30 sec
    dut.run_all_single_board_cases(timeout=60)


def esp_reset_and_wait_ready(dut: Dut) -> None:
    dut.serial.hard_reset()
    time.sleep(0.5)
    dut.expect_exact('Press ENTER to see the list of tests')


@pytest.mark.generic
@idf_parametrize(
    'config',
    [
        'pd_vddsdio',
        'psram',
    ],
    indirect=['config'],
)
@idf_parametrize('target', ['esp32c5'], indirect=['target'])
def test_esp_system_esp32c5(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=60)


def esp_reset_and_wait_ready_esp32c5(dut: Dut) -> None:
    dut.serial.hard_reset()
    time.sleep(0.5)
    dut.expect_exact('Press ENTER to see the list of tests')


@pytest.mark.generic
@pytest.mark.esp32c5_eco3
@idf_parametrize(
    'config',
    [
        'default',
        'psram_with_pd_top',
    ],
    indirect=['config'],
)
@idf_parametrize('target', ['esp32c5'], indirect=['target'])
def test_esp_system_esp32c5_eco3(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=60)


def esp_reset_and_wait_ready_esp32c5_eco3(dut: Dut) -> None:
    dut.serial.hard_reset()
    time.sleep(0.5)
    dut.expect_exact('Press ENTER to see the list of tests')


@pytest.mark.generic
@idf_parametrize('config', ['default'], indirect=['config'])
@idf_parametrize(
    'target',
    [target for target in soc_filtered_targets('SOC_LIGHT_SLEEP_SUPPORTED == 1')],
    indirect=['target'],
)
def test_sleep_uart_handling(dut: Dut) -> None:
    """Test UART handling modes during light sleep."""
    # Test FLUSH mode output
    esp_reset_and_wait_ready(dut)
    dut.write('"UART flush mode output verification"')
    dut.expect_exact('<<<FLUSH_START>>>')
    for i in range(10):
        dut.expect_exact(f'FLUSH_DATA_{i}')
    dut.expect_exact('<<<FLUSH_SLEEP>>>')
    dut.expect_exact('<<<FLUSH_END>>>')

    # Test SUSPEND mode output - verify data continues after wakeup with sleep delay
    esp_reset_and_wait_ready(dut)
    dut.write('"UART suspend mode output verification"')
    dut.expect_exact('<<<SUSPEND_START>>>')
    start_time = time.time()
    for i in range(10):
        dut.expect_exact(f'SUSPEND_DATA_{i}')
    dut.expect_exact('<<<SUSPEND_SLEEP>>>')
    dut.expect_exact('<<<SUSPEND_END>>>')
    end_time = time.time()
    elapsed = end_time - start_time
    # Sleep duration is 1 second, so total time should be >= 1s
    assert elapsed >= 1.0, f'SUSPEND mode: expected >= 1s delay due to sleep, but got {elapsed:.2f}s'

    # Test DISCARD mode output - verify data is discarded
    esp_reset_and_wait_ready(dut)
    dut.write('"UART discard mode output verification"')
    dut.expect_exact('<<<DISCARD_START>>>')
    # Capture output between START and END, verify DISCARD_DATA_9_SHOULD_BE_LOST is NOT present
    output = dut.expect(r'<<<DISCARD_END>>>', timeout=10)
    assert '<<<DISCARD_DATA_9_SHOULD_BE_LOST>>>' not in output.group().decode(), (
        'DISCARD mode failed: data should have been discarded but DISCARD_DATA_9_SHOULD_BE_LOST marker appeared'
    )


@pytest.mark.generic
@idf_parametrize('config', ['default'], indirect=['config'])
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_stack_smash_protection(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"stack smashing protection"')
    dut.expect_exact('Stack smashing protect failure!')
    dut.expect_exact('Rebooting...')


@pytest.mark.generic
@idf_parametrize('config', ['framepointer'], indirect=['config'])
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_frame_pointer_backtracing(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"Backtrace detects corrupted frames"')
    dut.expect_exact('Guru Meditation Error')
    # The backtrace should be composed of a single entry
    dut.expect(r'Backtrace: 0x[0-9a-f]{8}:0x[0-9a-f]{8}\s*[\r]?\n')
    dut.expect_exact('Rebooting...')

    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"Backtrace detects ROM functions"')
    dut.expect_exact('Guru Meditation Error')
    # The backtrace should have two entries
    dut.expect(r'Backtrace: 0x[0-9a-f]{8}:0x[0-9a-f]{8} 0x[0-9a-f]{8}:0x[0-9a-f]{8}\s*[\r]?\n')
    dut.expect_exact('Rebooting...')
