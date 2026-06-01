# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import logging

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize(
    'target',
    soc_filtered_targets(
        'SOC_LP_CORE_SUPPORTED == 1 and SOC_ULP_LP_UART_SUPPORTED == 1 and SOC_DEEP_SLEEP_SUPPORTED == 1'
    ),
    indirect=['target'],
)
def test_lp_core_pcnt(dut: Dut) -> None:
    res = dut.expect(r'ULP will wake up processor after every (\d+) pulses')
    wakeup_limit = res.group(1).decode('utf-8')
    assert int(wakeup_limit) > 0
    logging.info(f'Wake-up limit: {wakeup_limit} pulses')

    dut.expect_exact('Not a ULP wakeup, initializing it!')
    dut.expect_exact('Entering in deep sleep')

    dut.expect_exact('ULP woke up the main CPU!')

    res = dut.expect(r'Pulse count: (\d+)')
    pulse_count = res.group(1).decode('utf-8')
    logging.info(f'Pulse count: {pulse_count}')

    # Check that pulse count is correct, we could have gotten pulses between triggering
    # the wakeup signal and printing the count, but it should at be equal to or greater
    assert int(pulse_count) >= int(wakeup_limit)
