# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import logging
import time

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_example_ulp_fsm(dut: Dut) -> None:
    dut.expect_exact('Not ULP wakeup')
    dut.expect_exact('Entering deep sleep')

    def generate_gpio0_events() -> None:
        for _ in range(5):
            dut.serial.proc.setDTR(True)  # Pulling GPIO0 low using DTR
            time.sleep(0.25)
            dut.serial.proc.setDTR(False)
            time.sleep(0.25)

    nvs_value = None

    for _ in range(5):
        generate_gpio0_events()
        dut.expect_exact('ULP wakeup, saving pulse count', timeout=5)
        logging.info('Woke up...')
        init_count = int(dut.expect(r'Read pulse count from NVS:\s+(\d+)', timeout=5).group(1), 10)
        assert nvs_value in (init_count, None), ('Read count is {} and previously written value is {}'
                                                 ''.format(init_count, nvs_value))

        inc = int(dut.expect(r'Pulse count from ULP:\s+(\d+)', timeout=5).group(1), 10)
        assert inc in (5, 6), 'pulse count is {}'.format(inc)

        new_count = int(dut.expect(r'Wrote updated pulse count to NVS:\s+(\d+)', timeout=5).group(1), 10)
        assert init_count + inc == new_count, '{} + {} != {}'.format(init_count, inc, new_count)

        nvs_value = new_count
        logging.info('Pulse count written to NVS: {}. Entering deep sleep...'.format(nvs_value))
        dut.expect_exact('Entering deep sleep', timeout=5)
