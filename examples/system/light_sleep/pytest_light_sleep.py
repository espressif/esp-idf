# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import logging
import time

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_light_sleep(dut: Dut) -> None:

    ENTERING_SLEEP_STR = 'Entering light sleep'
    EXIT_SLEEP_REGEX = r'Returned from light sleep, reason: (\w+), t=(\d+) ms, slept for (\d+) ms'
    EXIT_SLEEP_PIN_REGEX = r'Returned from light sleep, reason: (pin), t=(\d+) ms, slept for (\d+) ms'
    EXIT_SLEEP_UART_REGEX = r'Returned from light sleep, reason: (uart), t=(\d+) ms, slept for (\d+) ms'
    WAITING_FOR_GPIO_STR = r'Waiting for GPIO\d+ to go high...'

    WAKEUP_INTERVAL_MS = 2000

    # Ensure DTR and RTS are de-asserted for proper control of GPIO0
    dut.serial.proc.setDTR(False)
    dut.serial.proc.setRTS(False)

    # enter sleep first time
    dut.expect_exact(ENTERING_SLEEP_STR, timeout=30)
    # don't check timing here, might be cache dependent
    dut.expect(EXIT_SLEEP_REGEX)
    logging.info('Got first sleep period')

    # enter sleep second time
    dut.expect_exact(ENTERING_SLEEP_STR)
    match = dut.expect(EXIT_SLEEP_REGEX)
    logging.info('Got second sleep period, wakeup from {}, slept for {}'.format(match.group(1), match.group(3)))
    # sleep time error should be less than 1ms
    assert match.group(1).decode('utf8') == 'timer' and int(match.group(3)) >= WAKEUP_INTERVAL_MS - 1 and int(match.group(3)) <= WAKEUP_INTERVAL_MS + 1

    # this time we'll test gpio wakeup
    dut.expect_exact(ENTERING_SLEEP_STR)
    logging.info('Pulling GPIO0 low using DTR')
    dut.serial.proc.setDTR(True)
    time.sleep(1)
    match = dut.expect(EXIT_SLEEP_PIN_REGEX)
    logging.info('Got third sleep period, wakeup from {}, slept for {}'.format(match.group(1), match.group(3)))
    assert int(match.group(3)) < WAKEUP_INTERVAL_MS

    dut.expect(WAITING_FOR_GPIO_STR)
    logging.info('Is waiting for GPIO...')

    dut.serial.proc.setDTR(False)
    dut.expect_exact(ENTERING_SLEEP_STR)
    logging.info('Went to sleep again')

    # Write 'a' to uart, 'a' in ascii is 0x61 which contains 3 rising edges in total (including the stop bit)
    dut.write('a')
    time.sleep(1)
    match = dut.expect(EXIT_SLEEP_UART_REGEX)
    logging.info('Got third sleep period, wakeup from {}, slept for {}'.format(match.group(1), match.group(3)))
    assert int(match.group(3)) < WAKEUP_INTERVAL_MS
    logging.info('Went to sleep again')

    match = dut.expect(EXIT_SLEEP_REGEX)
    assert match.group(1).decode('utf8') == 'timer' and int(match.group(3)) >= WAKEUP_INTERVAL_MS - 1 and int(match.group(3)) <= WAKEUP_INTERVAL_MS + 1
    logging.info('Woke up from timer again')
