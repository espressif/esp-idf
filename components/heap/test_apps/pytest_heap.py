# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.supported_targets
@pytest.mark.parametrize(
    'config',
    [
        'no_poisoning',
        'light_poisoning',
        'comprehensive_poisoning'
    ]
)
def test_heap_poisoning(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('*')
    dut.expect_unity_test_output(timeout=300)


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.parametrize(
    'config',
    [
        'psram',
        'psram_all_ext'
    ]
)
def test_heap(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('*')
    dut.expect_unity_test_output(timeout=300)


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.parametrize(
    'config',
    [
        'abort_alloc_fail'
    ]
)
def test_heap_abort_on_alloc_failure(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"When enabled, allocation operation failure generates an abort"')
    dut.expect('Backtrace: ')


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.parametrize(
    'config',
    [
        '8bit_access'
    ]
)
def test_heap_8bit_access(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"IRAM_8BIT capability test"')
    dut.expect_unity_test_output(timeout=300)
