# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import logging
import time

import pytest
from pytest_embedded import Dut

touch_wake_up_support = ['esp32', 'esp32s2']

CONFIGS = [
    pytest.param('esp32_singlecore', marks=[pytest.mark.esp32]),
    pytest.param('generic', marks=[pytest.mark.esp32, pytest.mark.esp32c3]),  # Disabled for now: IDF-4801
]


@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@pytest.mark.generic
def test_deep_sleep(dut: Dut) -> None:

    def expect_enable_deep_sleep_touch() -> None:
        # different targets configure different wake pin(s)
        wake_pads = {
            'esp32': [8,9],
            'esp32s2': [9],
        }[dut.target]

        logging.info('Expecting to see wakeup configured on pad(s): {}'.format(wake_pads))

        expect_items = ['Enabling timer wakeup, 20s']
        for pad in wake_pads:
            expect_items += [r'Touch pad #{} average: \d+, wakeup threshold set to \d+.'.format(pad)]
        expect_items += ['Enabling touch pad wakeup',
                         'Entering deep sleep']

        for exp in expect_items:
            dut.expect(exp, timeout=10)

    def expect_enable_deep_sleep_no_touch() -> None:
        dut.expect_exact('Enabling timer wakeup, 20s', timeout=10)
        dut.expect_exact('Entering deep sleep', timeout=10)

    if dut.target in touch_wake_up_support:
        expect_enable_deep_sleep = expect_enable_deep_sleep_touch
    else:
        expect_enable_deep_sleep = expect_enable_deep_sleep_no_touch

    dut.expect_exact('Not a deep sleep reset')
    expect_enable_deep_sleep()

    start_sleep = time.time()
    logging.info('Waiting for wakeup...')
    dut.expect_exact('boot: ESP-IDF')  # first output that's the same on all chips

    sleep_time = time.time() - start_sleep
    logging.info('Host measured sleep time at {:.2f}s'.format(sleep_time))
    assert 18 < sleep_time < 22  # note: high tolerance as measuring time on the host may have some timing skew

    # This line indicates that the CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP option set in sdkconfig.defaults
    # has correctly allowed skipping verification on wakeup
    dut.expect_exact('boot: Fast booting app from partition', timeout=2)

    # Check that it measured 2xxxxms in deep sleep, i.e at least 20 seconds:
    dut.expect(r'Wake up from timer. Time spent in deep sleep: 2\d{4}ms', timeout=2)
    expect_enable_deep_sleep()
