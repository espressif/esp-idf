from __future__ import print_function
import re
import os
import sys

try:
    import IDF
    from IDF.IDFDUT import ESP32DUT
except ImportError:
    # this is a test case write with tiny-test-fw.
    # to run test cases outside tiny-test-fw,
    # we need to set environment variable `TEST_FW_PATH`,
    # then get and insert `TEST_FW_PATH` to sys path before import FW module
    test_fw_path = os.getenv('TEST_FW_PATH')
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF

STARTING_TIMERS_REGEX = re.compile(r'Started timers, time since boot: (\d+) us')

# name, period, next_alarm, times_started, times_fired, cb_exec_time
TIMER_DUMP_LINE_REGEX = re.compile(r'([\w-]+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)')

PERIODIC_TIMER_REGEX = re.compile(r'Periodic timer called, time since boot: (\d+) us')

LIGHT_SLEEP_ENTER_REGEX = re.compile(r'Entering light sleep for 0\.5s, time since boot: (\d+) us')
LIGHT_SLEEP_EXIT_REGEX = re.compile(r'Woke up from light sleep, time since boot: (\d+) us')

ONE_SHOT_REGEX = re.compile(r'One\-shot timer called, time since boot: (\d+) us')

RESTART_REGEX = re.compile(r'Restarted periodic timer with 1s period, time since boot: (\d+) us')

STOP_REGEX = re.compile(r'Stopped and deleted timers')

INITIAL_TIMER_PERIOD = 500000
FINAL_TIMER_PERIOD = 1000000
LIGHT_SLEEP_TIME = 500000
ONE_SHOT_TIMER_PERIOD = 5000000


@IDF.idf_example_test(env_tag='Example_WIFI')
def test_examples_system_esp_timer(env, extra_data):
    dut = env.get_dut('esp_timer_example', 'examples/system/esp_timer', dut_class=ESP32DUT)
    # start test
    dut.start_app()
    groups = dut.expect(STARTING_TIMERS_REGEX, timeout=30)
    start_time = int(groups[0])
    print('Start time: {} us'.format(start_time))

    groups = dut.expect(TIMER_DUMP_LINE_REGEX, timeout=2)
    assert(groups[0] == 'periodic' and int(groups[1]) == INITIAL_TIMER_PERIOD)
    groups = dut.expect(TIMER_DUMP_LINE_REGEX, timeout=2)
    assert(groups[0] == 'one-shot' and int(groups[1]) == 0)

    for i in range(0, 5):
        groups = dut.expect(PERIODIC_TIMER_REGEX, timeout=2)
        cur_time = int(groups[0])
        diff = start_time + (i + 1) * INITIAL_TIMER_PERIOD - cur_time
        print('Callback #{}, time: {} us, diff: {} us'.format(i, cur_time, diff))
        assert(abs(diff) < 100)

    groups = dut.expect(ONE_SHOT_REGEX, timeout=3)
    one_shot_timer_time = int(groups[0])
    diff = start_time + ONE_SHOT_TIMER_PERIOD - one_shot_timer_time
    print('One-shot timer, time: {} us, diff: {}'.format(one_shot_timer_time, diff))
    assert(abs(diff) < 200)

    groups = dut.expect(RESTART_REGEX, timeout=3)
    start_time = int(groups[0])
    print('Timer restarted, time: {} us'.format(start_time))

    for i in range(0, 5):
        groups = dut.expect(PERIODIC_TIMER_REGEX, timeout=2)
        cur_time = int(groups[0])
        diff = start_time + (i + 1) * FINAL_TIMER_PERIOD - cur_time
        print('Callback #{}, time: {} us, diff: {} us'.format(i, cur_time, diff))
        assert(abs(diff) < 100)

    groups = dut.expect(LIGHT_SLEEP_ENTER_REGEX, timeout=2)
    sleep_enter_time = int(groups[0])
    groups = dut.expect(LIGHT_SLEEP_EXIT_REGEX, timeout=2)
    sleep_exit_time = int(groups[0])
    sleep_time = sleep_exit_time - sleep_enter_time

    print('Enter sleep: {}, exit sleep: {}, slept: {}'.format(
        sleep_enter_time, sleep_exit_time, sleep_time))

    assert(abs(sleep_time - LIGHT_SLEEP_TIME) < 1000)

    for i in range(5, 7):
        groups = dut.expect(PERIODIC_TIMER_REGEX, timeout=2)
        cur_time = int(groups[0])
        diff = abs(start_time + (i + 1) * FINAL_TIMER_PERIOD - cur_time)
        print('Callback #{}, time: {} us, diff: {} us'.format(i, cur_time, diff))
        assert(diff < 100)

    dut.expect(STOP_REGEX, timeout=2)


if __name__ == '__main__':
    test_examples_system_esp_timer()
