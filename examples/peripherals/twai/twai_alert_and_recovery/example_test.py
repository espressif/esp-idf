# Need Python 3 string formatting functions
from __future__ import print_function

import ttfw_idf

# TWAI Self Test Example constants
STR_EXPECT = ('TWAI Alert and Recovery: Driver installed', 'TWAI Alert and Recovery: Driver uninstalled')
EXPECT_TIMEOUT = 20


@ttfw_idf.idf_example_test(env_tag='Example_TWAI1')
def test_twai_alert_and_recovery_example(env, extra_data):
    dut = env.get_dut('dut1', 'examples/peripherals/twai/twai_alert_and_recovery', dut_class=ttfw_idf.ESP32DUT)
    dut.start_app()

    for string in STR_EXPECT:
        dut.expect(string, timeout=EXPECT_TIMEOUT)


if __name__ == '__main__':
    test_twai_alert_and_recovery_example()
