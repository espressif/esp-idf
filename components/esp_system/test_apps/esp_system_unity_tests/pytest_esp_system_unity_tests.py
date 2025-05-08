# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'config,target',
    [
        ('default', 'supported_targets'),
        ('pd_vddsdio', 'supported_targets'),
        ('psram', 'esp32'),
        ('psram', 'esp32p4'),
        ('psram', 'esp32s2'),
        ('psram', 'esp32s3'),
        ('psram_with_pd_top', 'esp32p4'),
        ('single_core_esp32', 'esp32'),
    ],
    indirect=['config', 'target'],
)
def test_esp_system(dut: Dut) -> None:
    # esp32p4 32MB PSRAM initialize in startup takes more than 30 sec
    dut.run_all_single_board_cases(timeout=60)


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
