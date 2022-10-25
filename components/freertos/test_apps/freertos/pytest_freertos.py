# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded import Dut

CONFIGS = [
    pytest.param('default', marks=[pytest.mark.supported_targets]),
    pytest.param('freertos_options', marks=[pytest.mark.supported_targets]),
    pytest.param('psram', marks=[pytest.mark.esp32]),
    pytest.param('release', marks=[pytest.mark.supported_targets]),
    pytest.param('single_core', marks=[pytest.mark.esp32]),
]


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
def test_freertos(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('![ignore]')
    # All of the FreeRTOS tests combined take > 60s to run. So we use a 120s timeout
    dut.expect_unity_test_output(timeout=120)
