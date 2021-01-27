# Need Python 3 string formatting functions
from __future__ import print_function

import ttfw_idf

# TWAI Self Test Example constants
STR_EXPECT = ('TWAI Self Test: Driver installed', 'TWAI Self Test: Driver uninstalled')
EXPECT_TIMEOUT = 20


@ttfw_idf.idf_example_test(env_tag='Example_TWAI1')
def test_twai_self_test_example(env, extra_data):
    # Get device under test, flash and start example. "dut1" must be defined in EnvConfig
    dut = env.get_dut('dut1', 'examples/peripherals/twai/twai_self_test', dut_class=ttfw_idf.ESP32DUT)
    dut.start_app()

    for string in STR_EXPECT:
        dut.expect(string, timeout=EXPECT_TIMEOUT)


if __name__ == '__main__':
    test_twai_self_test_example()
