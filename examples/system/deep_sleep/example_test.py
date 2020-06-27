from __future__ import unicode_literals
import re
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_deep_sleep(env, extra_data):

    dut = env.get_dut('deep_sleep', 'examples/system/deep_sleep')
    dut.start_app()

    def expect_enable_deep_sleep():
        dut.expect_all('Enabling timer wakeup, 20s',
                       re.compile(r'Touch pad #8 average: \d+, wakeup threshold set to \d+.'),
                       re.compile(r'Touch pad #9 average: \d+, wakeup threshold set to \d+.'),
                       'Enabling touch pad wakeup',
                       'Entering deep sleep',
                       timeout=10)

    dut.expect('Not a deep sleep reset', timeout=30)
    expect_enable_deep_sleep()

    # Check that it spent 2xxxxms in deep sleep, i.e at least 20 seconds:
    dut.expect(re.compile(r'Wake up from timer. Time spent in deep sleep: 2\d{4}ms'), timeout=30)
    expect_enable_deep_sleep()


if __name__ == '__main__':
    test_examples_deep_sleep()
