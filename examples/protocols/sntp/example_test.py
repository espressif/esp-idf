from __future__ import unicode_literals

import datetime
import re

import ttfw_idf
from tiny_test_fw import Utility


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_Protocols')
def test_examples_sntp(env, extra_data):

    dut = env.get_dut('sntp', 'examples/protocols/sntp')
    dut.start_app()

    dut.expect_all('Time is not set yet. Connecting to WiFi and getting time over NTP.',
                   'Initializing SNTP',
                   'Waiting for system time to be set... (1/10)',
                   'Notification of a time synchronization event',
                   timeout=60)

    TIME_FORMAT = '%a %b %d %H:%M:%S %Y'
    TIME_FORMAT_REGEX = r'\w+\s+\w+\s+\d{1,2}\s+\d{2}:\d{2}:\d{2} \d{4}'
    TIME_DIFF = datetime.timedelta(seconds=10 + 2)  # cpu spends 10 seconds in deep sleep
    NY_time = None
    SH_time = None

    def check_time(prev_NY_time, prev_SH_time):
        NY_str = dut.expect(re.compile(r'The current date/time in New York is: ({})'.format(TIME_FORMAT_REGEX)))[0]
        SH_str = dut.expect(re.compile(r'The current date/time in Shanghai is: ({})'.format(TIME_FORMAT_REGEX)))[0]
        Utility.console_log('New York: "{}"'.format(NY_str))
        Utility.console_log('Shanghai: "{}"'.format(SH_str))
        dut.expect('Entering deep sleep for 10 seconds')
        Utility.console_log('Sleeping...')
        new_NY_time = datetime.datetime.strptime(NY_str, TIME_FORMAT)
        new_SH_time = datetime.datetime.strptime(SH_str, TIME_FORMAT)

        # The initial time is not checked because datetime has problems with timezones
        assert prev_NY_time is None or new_NY_time - prev_NY_time < TIME_DIFF
        assert prev_SH_time is None or new_SH_time - prev_SH_time < TIME_DIFF

        return (new_NY_time, new_SH_time)

    NY_time, SH_time = check_time(NY_time, SH_time)
    for i in range(2, 4):
        dut.expect('example: Boot count: {}'.format(i), timeout=30)
        NY_time, SH_time = check_time(NY_time, SH_time)


if __name__ == '__main__':
    test_examples_sntp()
