# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import logging

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s3
@pytest.mark.generic
def test_example_ulp_fsm_adc(dut: Dut) -> None:

    dut.expect_exact('Not ULP wakeup')
    dut.expect_exact('Entering deep sleep')

    for _ in range(5):
        dut.expect_exact('Deep sleep wakeup', timeout=60)
        measurements_str = dut.expect(r'ULP did (\d+) measurements', timeout=5).group(1)
        assert measurements_str is not None
        measurements = int(measurements_str)
        logging.info('ULP did {} measurements'.format(measurements))
        dut.expect_exact('Thresholds:  low=1500  high=2000', timeout=5)
        value_str = dut.expect(r'Value=(\d+) was (above|below) threshold', timeout=5).group(1)
        assert value_str is not None
        value = int(value_str)
        logging.info('Value {} was outside the boundaries'.format(value))
        dut.expect_exact('Entering deep sleep', timeout=60)
