from __future__ import unicode_literals

import re

import ttfw_idf
from tiny_test_fw import Utility


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32'])
def test_examples_ulp_adc(env, extra_data):

    dut = env.get_dut('ulp_adc', 'examples/system/ulp_fsm/ulp_adc')
    dut.start_app()

    dut.expect_all('Not ULP wakeup',
                   'Entering deep sleep',
                   timeout=30)

    for _ in range(5):
        dut.expect('Deep sleep wakeup', timeout=60)
        measurements_str = dut.expect(re.compile(r'ULP did (\d+) measurements'), timeout=5)[0]
        assert measurements_str is not None
        measurements = int(measurements_str)
        Utility.console_log('ULP did {} measurements'.format(measurements))
        dut.expect('Thresholds:  low=1500  high=2000', timeout=5)
        value_str = dut.expect(re.compile(r'Value=(\d+) was (above|below) threshold'), timeout=5)[0]
        assert value_str is not None
        value = int(value_str)
        Utility.console_log('Value {} was outside the boundaries'.format(value))
        dut.expect('Entering deep sleep', timeout=60)


if __name__ == '__main__':
    test_examples_ulp_adc()
