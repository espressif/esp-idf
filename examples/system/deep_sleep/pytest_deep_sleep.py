# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import logging
import time

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'config,target', [('esp32_singlecore', 'esp32'), ('basic', 'supported_targets')], indirect=['config', 'target']
)
def test_deep_sleep(dut: Dut) -> None:
    dut.expect_exact('Enabling timer wakeup, 20s', timeout=10)
    dut.expect_exact('Not a deep sleep reset')
    dut.expect_exact('Entering deep sleep')

    start_sleep = time.time()
    logging.info('Waiting for wakeup...')
    dut.expect_exact('boot: ESP-IDF')  # first output that's the same on all chips

    sleep_time = time.time() - start_sleep
    logging.info('Host measured sleep time at {:.2f}s'.format(sleep_time))
    assert 18 < sleep_time < 22  # note: high tolerance as measuring time on the host may have some timing skew

    dut.expect_exact('boot: Fast booting app from partition', timeout=2)

    # Check that it measured 2xxxxms in deep sleep, i.e at least 20 seconds:
    dut.expect_exact('Enabling timer wakeup, 20s', timeout=10)
    dut.expect(r'Wake up from timer. Time spent in deep sleep: 2\d{4}ms', timeout=2)
    dut.expect_exact('Entering deep sleep')
