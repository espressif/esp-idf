# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize(
    'config,target',
    [
        *(('default', target) for target in soc_filtered_targets('IDF_TARGET not in ["esp32c5"]')),
        *(('pd_vddsdio', target) for target in soc_filtered_targets('IDF_TARGET not in ["esp32c5"]')),
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
