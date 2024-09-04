# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import logging

import pytest
from pytest_embedded import Dut

STARTING_TIMERS_REGEX = r'Started timers, time since boot: (\d+) us'

# name, period, next_alarm, times_started, times_fired, times_skipped, cb_exec_time
TIMER_DUMP_LINE_REGEX = r'([\w-]+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)'

PERIODIC_TIMER_REGEX = r'Periodic timer called, time since boot: (\d+) us'

LIGHT_SLEEP_ENTER_REGEX = r'Entering light sleep for 0\.5s, time since boot: (\d+) us'
LIGHT_SLEEP_EXIT_REGEX = r'Woke up from light sleep, time since boot: (\d+) us'

ONE_SHOT_REGEX = r'One\-shot timer called, time since boot: (\d+) us'

RESTART_REGEX = r'Restarted periodic timer with 1s period, time since boot: (\d+) us'

STOP_REGEX = r'Stopped and deleted timers'

INITIAL_TIMER_PERIOD = 500000
FINAL_TIMER_PERIOD = 1000000
LIGHT_SLEEP_TIME = 500000
ONE_SHOT_TIMER_PERIOD = 5000000


@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'rtc',
    ],
    indirect=True
)
def test_esp_timer(dut: Dut) -> None:

    match = dut.expect(STARTING_TIMERS_REGEX)
    start_time = int(match.group(1))
    logging.info('Start time: {} us'.format(start_time))

    match = dut.expect(TIMER_DUMP_LINE_REGEX, timeout=2)
    assert match.group(1).decode('utf8') == 'periodic' and int(match.group(2)) == INITIAL_TIMER_PERIOD
    match = dut.expect(TIMER_DUMP_LINE_REGEX, timeout=2)
    assert match.group(1).decode('utf8') == 'one-shot' and int(match.group(2)) == 0

    for i in range(0, 5):
        match = dut.expect(PERIODIC_TIMER_REGEX, timeout=2)
        cur_time = int(match.group(1))
        diff = start_time + (i + 1) * INITIAL_TIMER_PERIOD - cur_time
        logging.info('Callback #{}, time: {} us, diff: {} us'.format(i, cur_time, diff))
        assert abs(diff) < 100

    match = dut.expect(ONE_SHOT_REGEX, timeout=3)
    one_shot_timer_time = int(match.group(1))
    diff = start_time + ONE_SHOT_TIMER_PERIOD - one_shot_timer_time
    logging.info('One-shot timer, time: {} us, diff: {}'.format(one_shot_timer_time, diff))
    assert abs(diff) < 400

    match = dut.expect(RESTART_REGEX, timeout=3)
    start_time = int(match.group(1))
    logging.info('Timer restarted, time: {} us'.format(start_time))

    for i in range(0, 5):
        match = dut.expect(PERIODIC_TIMER_REGEX, timeout=2)
        cur_time = int(match.group(1))
        diff = start_time + (i + 1) * FINAL_TIMER_PERIOD - cur_time
        logging.info('Callback #{}, time: {} us, diff: {} us'.format(i, cur_time, diff))
        assert abs(diff) < 100

    match = dut.expect(LIGHT_SLEEP_ENTER_REGEX, timeout=2)
    sleep_enter_time = int(match.group(1))
    match = dut.expect(LIGHT_SLEEP_EXIT_REGEX, timeout=2)
    sleep_exit_time = int(match.group(1))
    sleep_time = sleep_exit_time - sleep_enter_time

    logging.info('Enter sleep: {}, exit sleep: {}, slept: {}'.format(
        sleep_enter_time, sleep_exit_time, sleep_time))

    assert abs(sleep_time - LIGHT_SLEEP_TIME) < 1200

    for i in range(5, 7):
        match = dut.expect(PERIODIC_TIMER_REGEX, timeout=2)
        cur_time = int(match.group(1))
        diff = abs(start_time + (i + 1) * FINAL_TIMER_PERIOD - cur_time)
        logging.info('Callback #{}, time: {} us, diff: {} us'.format(i, cur_time, diff))
        assert diff < 100

    dut.expect(STOP_REGEX, timeout=2)
