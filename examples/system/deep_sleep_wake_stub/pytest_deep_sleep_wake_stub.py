# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import logging
import time

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'default',
    ],
    indirect=True,
)
@idf_parametrize(
    'target', ['esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c6', 'esp32c5', 'esp32h2'], indirect=['target']
)
def test_deep_sleep_wake_stub(config: str, dut: Dut) -> None:
    if config == 'default':
        dut.expect_exact('Enabling timer wakeup, 10s', timeout=10)
        dut.expect_exact('Entering deep sleep', timeout=10)

        start_sleep = time.time()
        logging.info('Waiting for wakeup...')
        dut.expect_exact('wake stub: going to deep sleep', timeout=15)

        sleep_time = time.time() - start_sleep
        logging.info('Host measured sleep time at {:.2f}s'.format(sleep_time))
        assert 8 < sleep_time < 12  # note: high tolerance as measuring time on the host may have some timing skew
