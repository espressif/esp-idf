# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
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
