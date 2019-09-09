from __future__ import print_function
import re
import os
import sys
import time

try:
    import IDF
    from IDF.IDFDUT import ESP32DUT
except ImportError:
    test_fw_path = os.getenv('TEST_FW_PATH')
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF

ENTERING_SLEEP_STR = 'Entering light sleep'
EXIT_SLEEP_REGEX = re.compile(r'Returned from light sleep, reason: (\w+), t=(\d+) ms, slept for (\d+) ms')
WAITING_FOR_GPIO_STR = 'Waiting for GPIO0 to go high...'

WAKEUP_INTERVAL_MS = 2000


@IDF.idf_example_test(env_tag='Example_WIFI')
def test_examples_system_light_sleep(env, extra_data):
    dut = env.get_dut('light_sleep_example', 'examples/system/light_sleep', dut_class=ESP32DUT)
    dut.start_app()

    # Ensure DTR and RTS are de-asserted for proper control of GPIO0
    dut.port_inst.setDTR(False)
    dut.port_inst.setRTS(False)

    # enter sleep first time
    dut.expect(ENTERING_SLEEP_STR, timeout=30)
    # don't check timing here, might be cache dependent
    dut.expect(EXIT_SLEEP_REGEX)
    print('Got first sleep period')

    # enter sleep second time
    dut.expect(ENTERING_SLEEP_STR)
    groups = dut.expect(EXIT_SLEEP_REGEX)
    print('Got second sleep period, wakeup from {}, slept for {}'.format(groups[0], groups[2]))
    # sleep time error should be less than 1ms
    assert(groups[0] == 'timer' and int(groups[2]) == WAKEUP_INTERVAL_MS)

    # this time we'll test gpio wakeup
    dut.expect(ENTERING_SLEEP_STR)
    print('Pulling GPIO0 low using DTR')
    dut.port_inst.setDTR(True)
    time.sleep(1)
    groups = dut.expect(EXIT_SLEEP_REGEX)
    print('Got third sleep period, wakeup from {}, slept for {}'.format(groups[0], groups[2]))
    assert(groups[0] == 'pin' and int(groups[2]) < WAKEUP_INTERVAL_MS)

    dut.expect(WAITING_FOR_GPIO_STR)
    print('Is waiting for GPIO...')

    dut.port_inst.setDTR(False)
    dut.expect(ENTERING_SLEEP_STR)
    print('Went to sleep again')
    groups = dut.expect(EXIT_SLEEP_REGEX)
    assert(groups[0] == 'timer' and int(groups[2]) == WAKEUP_INTERVAL_MS)
    print('Woke up from timer again')


if __name__ == '__main__':
    test_examples_system_light_sleep()
