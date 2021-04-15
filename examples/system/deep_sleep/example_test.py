from __future__ import unicode_literals

import re
import time

import ttfw_idf

touch_wake_up_support = ['esp32', 'esp32s2']


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32', 'esp32s2', 'esp32c3'])
def test_examples_deep_sleep(env, extra_data):

    dut = env.get_dut('deep_sleep', 'examples/system/deep_sleep')
    dut.start_app()

    def expect_enable_deep_sleep_touch():
        # different targets configure different wake pin(s)
        wake_pads = {
            'esp32': [8,9],
            'esp32s2': [9],
        }[dut.TARGET]

        print('Expecting to see wakeup configured on pad(s): {}'.format(wake_pads))

        expect_items = ['Enabling timer wakeup, 20s']
        for pad in wake_pads:
            expect_items += [re.compile(r'Touch pad #{} average: \d+, wakeup threshold set to \d+.'.format(pad))]
        expect_items += ['Enabling touch pad wakeup',
                         'Entering deep sleep']

        dut.expect_all(*expect_items, timeout=10)

    def expect_enable_deep_sleep_no_touch():
        dut.expect_all('Enabling timer wakeup, 20s',
                       'Entering deep sleep',
                       timeout=10)

    if dut.TARGET in touch_wake_up_support:
        expect_enable_deep_sleep = expect_enable_deep_sleep_touch
    else:
        expect_enable_deep_sleep = expect_enable_deep_sleep_no_touch

    dut.expect('Not a deep sleep reset', timeout=30)
    expect_enable_deep_sleep()

    start_sleep = time.time()
    print('Waiting for wakeup...')
    dut.expect('boot: ESP-IDF', timeout=30)  # first output that's the same on all chips

    sleep_time = time.time() - start_sleep
    print('Host measured sleep time at {:.2f}s'.format(sleep_time))
    assert 19 < sleep_time < 22  # note: high tolerance as measuring time on the host may have some timing skew

    # This line indicates that the CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP option set in sdkconfig.defaults
    # has correctly allowed skipping verification on wakeup
    dut.expect('boot: Fast booting app from partition', timeout=2)

    # Check that it measured 2xxxxms in deep sleep, i.e at least 20 seconds:
    dut.expect(re.compile(r'Wake up from timer. Time spent in deep sleep: 2\d{4}ms'), timeout=2)
    expect_enable_deep_sleep()


if __name__ == '__main__':
    test_examples_deep_sleep()
