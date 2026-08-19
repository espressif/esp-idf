# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import logging
import time

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

touch_wake_up_support = ['esp32', 'esp32s2']


@pytest.mark.generic
@idf_parametrize(
    'config,target', [('esp32_singlecore', 'esp32'), ('basic', 'supported_targets')], indirect=['config', 'target']
)
def test_deep_sleep(dut: Dut) -> None:
    def expect_enable_deep_sleep_touch() -> None:
        # different targets configure different wake pin(s)
        wake_pads = {
            'esp32': [8, 9],
            'esp32s2': [9],
        }[dut.target]

        logging.info(f'Expecting to see wakeup configured on pad(s): {wake_pads}')

        expect_items = ['Enabling timer wakeup, 20s']
        for pad in wake_pads:
            expect_items += [rf'Touch pad #{pad} average: \d+, wakeup threshold set to \d+.']
        expect_items += ['Enabling touch pad wakeup']

        for exp in expect_items:
            dut.expect(exp, timeout=10)

    def expect_enable_deep_sleep_no_touch() -> None:
        dut.expect_exact('Enabling timer wakeup, 20s', timeout=10)

    if dut.target in touch_wake_up_support:
        expect_enable_deep_sleep = expect_enable_deep_sleep_touch
    else:
        expect_enable_deep_sleep = expect_enable_deep_sleep_no_touch

    expect_enable_deep_sleep()
    dut.expect_exact('Not a deep sleep reset')
    dut.expect_exact('Entering deep sleep')

    start_sleep = time.time()
    logging.info('Waiting for wakeup...')
    dut.expect_exact('boot: ESP-IDF')  # first output that's the same on all chips

    sleep_time = time.time() - start_sleep
    logging.info(f'Host measured sleep time at {sleep_time:.2f}s')
    assert 18 < sleep_time < 22  # note: high tolerance as measuring time on the host may have some timing skew

    dut.expect_exact('boot: Fast booting app from partition', timeout=2)

    # Check that it measured 2xxxxms in deep sleep, i.e at least 20 seconds:
    expect_enable_deep_sleep()
    dut.expect(r'Wake up from timer. Time spent in deep sleep: 2\d{4}ms', timeout=2)
    dut.expect_exact('Entering deep sleep')
