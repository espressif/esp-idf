# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.supported_targets
@pytest.mark.parametrize(
    'config',
    [
        'default',
    ],
    indirect=True,
)
def test_pthread(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('*')
    dut.expect_unity_test_output(timeout=300)


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        pytest.param('single_core_esp32', marks=[pytest.mark.esp32]),
        pytest.param('single_core_esp32s3', marks=[pytest.mark.esp32s3]),
    ],
    indirect=True,
)
def test_pthread_single_core(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('*')
    dut.expect_unity_test_output(timeout=300)
