# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        pytest.param('default', marks=[pytest.mark.supported_targets]),
        pytest.param('pd_vddsdio', marks=[pytest.mark.supported_targets]),
        pytest.param('psram', marks=[pytest.mark.esp32, pytest.mark.esp32s2, pytest.mark.esp32s3, pytest.mark.esp32p4]),
        pytest.param('single_core_esp32', marks=[pytest.mark.esp32]),
    ]
)
def test_esp_system(dut: Dut) -> None:
    # esp32p4 32MB PSRAM initialize in startup takes more than 30 sec
    dut.run_all_single_board_cases(timeout=60)


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        pytest.param('default', marks=[pytest.mark.supported_targets]),
    ]
)
def test_stack_smash_protection(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"stack smashing protection"')
    dut.expect_exact('Stack smashing protect failure!')
    dut.expect_exact('Rebooting...')
