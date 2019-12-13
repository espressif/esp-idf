# Need Python 3 string formatting functions
from __future__ import print_function

import ttfw_idf


# CAN Self Test Example constants
STR_EXPECT = ("CAN Self Test: Driver installed", "CAN Self Test: Driver uninstalled")
EXPECT_TIMEOUT = 20


@ttfw_idf.idf_example_test(env_tag='Example_CAN1')
def test_can_self_test_example(env, extra_data):
    # Get device under test, flash and start example. "dut1" must be defined in EnvConfig
    dut = env.get_dut('dut1', 'examples/peripherals/can/can_self_test')
    dut.start_app()

    for string in STR_EXPECT:
        dut.expect(string, timeout=EXPECT_TIMEOUT)


if __name__ == '__main__':
    test_can_self_test_example()
