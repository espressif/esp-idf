# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import logging

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

STARTING_TIMERS_REGEX = r'Started timers, time since boot: (\d+) us'

# name, period, next_alarm, times_started, times_fired, times_skipped, cb_exec_time
TIMER_DUMP_LINE_REGEX = r'([\w-]+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)'

PERIODIC_TIMER_REGEX = r'Periodic timer called, time since boot: (\d+) us'
TIMED_PERIODIC_TIMER_REGEX = r'Timed periodic timer called, time since boot: (\d+) us'

LIGHT_SLEEP_ENTER_REGEX = r'Entering light sleep for 0\.5s, time since boot: (\d+) us'
LIGHT_SLEEP_EXIT_REGEX = r'Woke up from light sleep, time since boot: (\d+) us'

ONE_SHOT_REGEX = r'One\-shot timer called, time since boot: (\d+) us'
TIMED_ONE_SHOT_REGEX = r'Timed one-shot timer called, time since boot: (\d+) us'

RESTART_REGEX = r'Restarted periodic timer with 1s period, time since boot: (\d+) us'
TIMED_RESTART_REGEX = r'Restarted timed periodic timer with 1s period, time since boot: (\d+) us'

STOP_REGEX = r'Stopped and deleted timers'

INITIAL_TIMER_PERIOD = 500000
FINAL_TIMER_PERIOD = 1000000
LIGHT_SLEEP_TIME = 500000
ONE_SHOT_TIMER_PERIOD = 5000000
TIMED_PERIODIC_START_TIME = 3000000
TIMED_ONE_SHOT_TIME = 6000000
TIMED_RESTART_TIME = 7000000


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'rtc',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_esp_timer(dut: Dut) -> None:
    match = dut.expect(STARTING_TIMERS_REGEX)
    start_time = int(match.group(1))
    logging.info(f'Start time: {start_time} us')

    match = dut.expect(TIMER_DUMP_LINE_REGEX, timeout=2)
    assert match.group(1).decode('utf8') == 'periodic' and int(match.group(2)) == INITIAL_TIMER_PERIOD
    match = dut.expect(TIMER_DUMP_LINE_REGEX, timeout=2)
    assert match.group(1).decode('utf8') == 'timed_periodic' and int(match.group(2)) == INITIAL_TIMER_PERIOD
    match = dut.expect(TIMER_DUMP_LINE_REGEX, timeout=2)
    assert match.group(1).decode('utf8') == 'one-shot' and int(match.group(2)) == 0
    match = dut.expect(TIMER_DUMP_LINE_REGEX, timeout=2)
    assert match.group(1).decode('utf8') == 'timed_one-shot' and int(match.group(2)) == 0

    for i in range(0, 5):
        match = dut.expect(PERIODIC_TIMER_REGEX, timeout=2)
        cur_time = int(match.group(1))
        diff = start_time + (i + 1) * INITIAL_TIMER_PERIOD - cur_time
        logging.info(f'Callback #{i}, time: {cur_time} us, diff: {diff} us')
        assert abs(diff) < 100

    for i in range(0, 5):
        match = dut.expect(TIMED_PERIODIC_TIMER_REGEX, timeout=2)
        cur_time = int(match.group(1))
        diff = TIMED_PERIODIC_START_TIME + i * INITIAL_TIMER_PERIOD - cur_time
        logging.info(f'Callback #{i}, time: {cur_time} us, diff: {diff} us')
        assert abs(diff) < 100

    match = dut.expect(ONE_SHOT_REGEX, timeout=3)
    one_shot_timer_time = int(match.group(1))
    diff = start_time + ONE_SHOT_TIMER_PERIOD - one_shot_timer_time
    logging.info(f'One-shot timer, time: {one_shot_timer_time} us, diff: {diff}')
    assert abs(diff) < 400

    match = dut.expect(RESTART_REGEX, timeout=3)
    start_time = int(match.group(1))
    logging.info(f'Timer restarted, time: {start_time} us')

    match = dut.expect(TIMED_ONE_SHOT_REGEX, timeout=3)
    timed_one_shot_timer_time = int(match.group(1))
    diff = TIMED_ONE_SHOT_TIME - timed_one_shot_timer_time
    logging.info(f'Timed one-shot timer, time: {timed_one_shot_timer_time} us, diff: {diff}')

    match = dut.expect(TIMED_RESTART_REGEX, timeout=3)
    timed_start_time = int(match.group(1))
    logging.info(f'Timed timer restarted, time: {timed_start_time} us')

    # First callback after restart
    match = dut.expect(PERIODIC_TIMER_REGEX, timeout=2)
    cur_time = int(match.group(1))
    diff = start_time + FINAL_TIMER_PERIOD - cur_time
    logging.info(f'Callback #{0}, time: {cur_time} us, diff: {diff} us')
    assert abs(diff) < 100

    # Callbacks 2 to 5 after restart (now both timers are running)
    for i in range(1, 5):
        timed_match = dut.expect(TIMED_PERIODIC_TIMER_REGEX, timeout=2)
        timed_cur_time = int(timed_match.group(1))
        timed_diff = TIMED_RESTART_TIME + (i - 1) * FINAL_TIMER_PERIOD - timed_cur_time
        logging.info(f'Timed Callback #{i}, time: {timed_cur_time} us, diff: {timed_diff} us')
        assert abs(timed_diff) < 100

        match = dut.expect(PERIODIC_TIMER_REGEX, timeout=2)
        cur_time = int(match.group(1))
        diff = start_time + (i + 1) * FINAL_TIMER_PERIOD - cur_time
        logging.info(f'Callback #{i}, time: {cur_time} us, diff: {diff} us')
        assert abs(diff) < 100

    if dut.app.sdkconfig.get('SOC_LIGHT_SLEEP_SUPPORTED'):
        match = dut.expect(LIGHT_SLEEP_ENTER_REGEX, timeout=2)
        sleep_enter_time = int(match.group(1))
        match = dut.expect(LIGHT_SLEEP_EXIT_REGEX, timeout=2)
        sleep_exit_time = int(match.group(1))
        sleep_time = sleep_exit_time - sleep_enter_time

        logging.info(f'Enter sleep: {sleep_enter_time}, exit sleep: {sleep_exit_time}, slept: {sleep_time}')

        assert -2000 < sleep_time - LIGHT_SLEEP_TIME < 1000

    for i in range(5, 7):
        timed_match = dut.expect(TIMED_PERIODIC_TIMER_REGEX, timeout=2)
        timed_cur_time = int(timed_match.group(1))
        timed_diff = TIMED_RESTART_TIME + (i - 1) * FINAL_TIMER_PERIOD - timed_cur_time
        logging.info(f'Timed Callback #{i}, time: {timed_cur_time} us, diff: {timed_diff} us')
        assert abs(timed_diff) < 100

        match = dut.expect(PERIODIC_TIMER_REGEX, timeout=2)
        cur_time = int(match.group(1))
        diff = abs(start_time + (i + 1) * FINAL_TIMER_PERIOD - cur_time)
        logging.info(f'Callback #{i}, time: {cur_time} us, diff: {diff} us')
        assert diff < 100

    dut.expect(STOP_REGEX, timeout=2)
